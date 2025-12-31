#!/usr/bin/env bash
# SPRT CI test with cutechess-cli.
# - Two engines (first two in engines file by default, or --a/--b).
# - Openings: EPD or PGN; both colors per opening (-repeat + -games 2).
# - Default SPRT: elo0=0 elo1=5 alpha=0.05 beta=0.05 (stop on decision).
# - Default TC: 10+0.1.
# - Writes logs/pgn with timestamped names. Prints decision and games played.

set -euo pipefail

# --- Defaults ---
TC_DEFAULT="10+0.1"
THREADS_DEFAULT=1
BOOK_PLIES_DEFAULT=8
ADJ_RESIGN_SCORE=700
ADJ_RESIGN_MOVES=8
ADJ_DRAW_MOVENUM=60
ADJ_DRAW_MOVES=10
ADJ_DRAW_SCORE=10

ELO0=0
ELO1=5
ALPHA=0.05
BETA=0.05

CONCURRENCY_OVERRIDE=""
A_NAME_OVERRIDE=""
B_NAME_OVERRIDE=""
#SEED=""
ENGINES_TSV=""
OPENINGS_FILE=""
GLOBAL_THREADS=""
TC="$TC_DEFAULT"
ROUNDS=""   # not set by default; SPRT decides
GAMES=""    # optional override
DEBUG=false # depuración desactivada por defecto

die(){ echo "ERROR: $*" >&2; exit 1; }
need(){ command -v "$1" >/dev/null 2>&1 || die "'$1' not found in PATH."; }
timestamp(){ date +"%Y%m%d_%H%M%S"; }

normalize_proto(){
  local p="${1:-}" name="${2:-}"
  case "${p,,}" in
    ""|uci) echo "uci";;
    xboard|winboard|wb) echo "xboard";;
    *)
      if [[ -n "$name" ]]; then
        die "Unknown proto '$p' for engine '$name' (use uci or xboard/winboard)."
      else
        die "Unknown proto '$p' (use uci or xboard/winboard)."
      fi
      ;;
  esac
}

read_sep_fields(){
  local line="$1"
  shift
  local us=$'\x1f'
  IFS="$us" read -r "$@" <<<"${line//;/$us}"
}

usage(){
  cat <<EOF
Usage: $0 --engines engines.tsv --openings file.(epd|pgn) [options]
Required:
  --engines <path.tsv>         Engines file (semicolon-separated: name;cmd;proto;hash; proto optional: uci|xboard)
  --openings <path.epd|pgn>
Optional:
  --a <name>  --b <name>      Pick engines by 'name' column
  --tc <tc>                   Default: ${TC_DEFAULT}
  --threads <N>               Global threads (optional)
  --book-depth <ply>          If EPD, default ${BOOK_PLIES_DEFAULT}
  --concurrency <N>           Default: auto max(1,nproc-1)
  --debug <true|false>        Default: false (protocol I/O con timestamps a logs/<tag>.debug)
  --seed <int>                RNG seed for -srand
  --adjudicate                Enable resign/draw (conservative defaults)
  --sprt-elo0 <n>             H0 (default ${ELO0})
  --sprt-elo1 <n>             H1 (default ${ELO1})
  --sprt-alpha <p>            (default ${ALPHA})
  --sprt-beta <p>             (default ${BETA})
  --rounds <N>                Override (not typical for SPRT)
  --games <N>                 Override games per round (default 2 with -repeat)
EOF
}

# --- Parse CLI ---
BOOK_PLIES="$BOOK_PLIES_DEFAULT"
ADJUDICATE=false
while [[ $# -gt 0 ]]; do
  case "$1" in
    --engines) ENGINES_TSV="${2:-}"; shift 2;;
    --openings) OPENINGS_FILE="${2:-}"; shift 2;;
    --a) A_NAME_OVERRIDE="${2:-}"; shift 2;;
    --b) B_NAME_OVERRIDE="${2:-}"; shift 2;;
    --tc) TC="${2:-}"; shift 2;;
    --threads) GLOBAL_THREADS="${2:-$THREADS_DEFAULT}"; shift 2;;
    --book-depth) BOOK_PLIES="${2:-}"; shift 2;;
    --concurrency) CONCURRENCY_OVERRIDE="${2:-}"; shift 2;;
    #--seed) SEED="${2:-}"; shift 2;;
    --adjudicate) ADJUDICATE=true; shift;;
    --debug)
      case "${2:-}" in
        true|TRUE|True|1|yes|on) DEBUG=true;;
        *) DEBUG=false;;
      esac
      shift 2;;
    --sprt-elo0) ELO0="${2:-}"; shift 2;;
    --sprt-elo1) ELO1="${2:-}"; shift 2;;
    --sprt-alpha) ALPHA="${2:-}"; shift 2;;
    --sprt-beta) BETA="${2:-}"; shift 2;;
    --rounds) ROUNDS="${2:-}"; shift 2;;
    --games) GAMES="${2:-}"; shift 2;;
    -h|--help) usage; exit 0;;
    *) die "Unknown option: $1";;
  esac
done

need cutechess-cli; need awk; need nproc; need tee; need stdbuf
[[ -n "$ENGINES_TSV" && -f "$ENGINES_TSV" ]] || { usage; die "--engines required"; }
[[ -n "$OPENINGS_FILE" && -f "$OPENINGS_FILE" ]] || { usage; die "--openings required"; }

mkdir -p ./logs ./pgn

# Concurrency
if [[ -n "$CONCURRENCY_OVERRIDE" ]]; then
  CONCURRENCY="$CONCURRENCY_OVERRIDE"
else
  CPU="$(nproc)"; CONCURRENCY=$(( CPU>1 ? CPU-1 : 1 ))
fi

# Seed
#if [[ -z "$SEED" ]]; then SEED="$(( (RANDOM<<16) ^ RANDOM ))"; fi

# Engines file lines (header optional; if present, columns: name cmd proto hash)
mapfile -t LINES < <(awk -F';' '
  BEGIN{OFS=";"}
  NR==1{
    for(i=1;i<=NF;i++){h[$i]=i}
    if(h["name"] && h["cmd"]){ has_header=1; next }
    has_header=0
  }
  NF && $0 !~ /^[ \t]*#/ {
    if(has_header){
      name = (h["name"] ? $(h["name"]) : "")
      cmd = (h["cmd"] ? $(h["cmd"]) : "")
      p = (h["proto"] ? h["proto"] : (h["protocol"] ? h["protocol"] : 0))
      proto = (p ? $(p) : "")
      hash = (h["hash"] ? $(h["hash"]) : "")
      print name, cmd, proto, hash
    } else {
      name=$1; cmd=$2; proto=(NF>=3?$3:""); hash=(NF>=4?$4:"")
      print name, cmd, proto, hash
    }
  }
' "$ENGINES_TSV")
[[ ${#LINES[@]} -ge 2 ]] || die "Need at least two engines in engines file."

declare -A ENGINE_BY_NAME
for ln in "${LINES[@]}"; do n="$(echo -e "$ln"|awk -F';' '{print $1}')"; ENGINE_BY_NAME["$n"]="$ln"; done

resolve_line(){ local n="$1"; local l="${ENGINE_BY_NAME[$n]:-}"; [[ -n "$l" ]] || die "Engine '$n' not in engines file"; echo "$l"; }
pick_two(){ echo -e "${LINES[0]}\n${LINES[1]}"; }

if [[ -n "$A_NAME_OVERRIDE" && -n "$B_NAME_OVERRIDE" ]]; then
  A_LINE="$(resolve_line "$A_NAME_OVERRIDE")"
  B_LINE="$(resolve_line "$B_NAME_OVERRIDE")"
else
  mapfile -t PAIR < <(pick_two)
  A_LINE="${PAIR[0]}"; B_LINE="${PAIR[1]}"
  A_NAME_OVERRIDE="$(echo -e "$A_LINE"|awk -F';' '{print $1}')"
  B_NAME_OVERRIDE="$(echo -e "$B_LINE"|awk -F';' '{print $1}')"
fi

# Build -engine (same logic as smoke)
build_engine(){
  local line="$1"
  local gthr="${GLOBAL_THREADS:-}"
  local proto=""
  read_sep_fields "$line" name cmd proto hash
  proto="$(normalize_proto "$proto" "$name")"
  [[ -x "$cmd" ]] || die "Engine binary not executable: $cmd"
  local threads="${gthr:-}"
  local hashmb="${hash:-}"
  local optstr=""
  if [[ -n "${hashmb:-}" ]]; then
    optstr+=" option.Hash=${hashmb}"
  fi
  if [[ -n "${threads:-}" ]]; then
    optstr+=" option.Threads=${threads}"
  fi
  echo "-engine name=${name} cmd=${cmd} proto=${proto}${optstr}"
}

ENG_A="$(build_engine "$A_LINE")"
ENG_B="$(build_engine "$B_LINE")"

# Optional safety: ensure we actually built both engines
[[ -n "$ENG_A" && -n "$ENG_B" ]] || die "Empty engine clauses; check build_engine() / engines file input." #Remove if some param is used

# Openings: detect format and set flags
ext="${OPENINGS_FILE##*.}"; lower_ext="$(echo "$ext" | tr '[:upper:]' '[:lower:]')"
OPEN_FLAGS=()
case "$lower_ext" in
  epd) OPEN_FLAGS=( -openings "file=${OPENINGS_FILE}" "format=epd" "order=random" "plies=${BOOK_PLIES}" );;
  pgn) OPEN_FLAGS=( -openings "file=${OPENINGS_FILE}" "format=pgn" "policy=round" );;
  *) die "Openings must be .epd or .pgn";;
esac

ADJ_FLAGS=()
if $ADJUDICATE; then
  ADJ_FLAGS+=( -resign "movecount=${ADJ_RESIGN_MOVES}" "score=${ADJ_RESIGN_SCORE}" )
  ADJ_FLAGS+=( -draw "movenumber=${ADJ_DRAW_MOVENUM}" "movecount=${ADJ_DRAW_MOVES}" "score=${ADJ_DRAW_SCORE}" )
fi

ts="$(timestamp)"
LOGFILE="./logs/sprt_${A_NAME_OVERRIDE}_vs_${B_NAME_OVERRIDE}_${ts}.log"
PGNFILE="./pgn/sprt_${A_NAME_OVERRIDE}_vs_${B_NAME_OVERRIDE}_${ts}.pgn"
DEBUGFILE="./logs/sprt_${A_NAME_OVERRIDE}_vs_${B_NAME_OVERRIDE}_${ts}.debug"

echo "=== SPRT configuration ==="
echo "Engines file:  $ENGINES_TSV"
echo "A vs B:        $A_NAME_OVERRIDE vs $B_NAME_OVERRIDE"
echo "Openings:      $OPENINGS_FILE (format: $lower_ext) with color-swap"
echo "TC:            $TC"
echo "SPRT:          elo0=${ELO0} elo1=${ELO1} alpha=${ALPHA} beta=${BETA}"
[[ -n "$ROUNDS" ]] && echo "Rounds:        $ROUNDS"
[[ -n "$GAMES"  ]] && echo "Games/round:   $GAMES"
echo "Concurrency:   $CONCURRENCY"
#echo "Seed:          $SEED"
echo "Logs:          $LOGFILE"
echo "PGN:           $PGNFILE"
echo "Debug:         $([[ $DEBUG == true ]] && echo "$DEBUGFILE (enabled, protocol I/O to file)" || echo "off")"

#trap "echo 'Aborting…'; kill 0 || true" INT TERM

CMD=( cutechess-cli
  $ENG_A
  $ENG_B
  -each tc="$TC" timemargin=50 ponder=false
  "${OPEN_FLAGS[@]}"
  -repeat
  -concurrency "$CONCURRENCY"
  #-srand "$SEED"
  -sprt "elo0=${ELO0}" "elo1=${ELO1}" "alpha=${ALPHA}" "beta=${BETA}"
  "${ADJ_FLAGS[@]}"
  -pgnout "$PGNFILE"
)
# optional rounds/games overrides
[[ -n "$ROUNDS" ]] && CMD+=( -rounds "$ROUNDS" )
if [[ -n "$GAMES" ]]; then CMD+=( -games "$GAMES" ); else CMD+=( -games 2 ); fi

$DEBUG && CMD+=( -debug all)

# --- Ejecución con/ sin captura de protocol I/O ---
if $DEBUG; then
  {
    echo "=== DEBUG $(date) ==="
    echo "Workdir: $(pwd)"
    echo "nproc: $(nproc)"
    echo "--- Full command ---"
    printf '%q ' "${CMD[@]}"; echo
    echo "--- BEGIN PROTOCOL I/O ---"
  } > "$DEBUGFILE"

  stdbuf -oL -eL "${CMD[@]}" 2>&1 \
    | awk -v dbg="$DEBUGFILE" '
        function ts(   t,cmd){
          cmd="date +\"%Y-%m-%d %H:%M:%S,%3N\" 2>/dev/null"
          cmd|getline t
          close(cmd)
          return t
        }
        {
          line=$0
          # Formato de cutechess -debug: "NNN <engine>(idx): ..." ó "NNN >engine(idx): ..."
          if (line ~ /^[[:space:]]*[0-9]+[[:space:]][<>]/) {
            print ts(), line >> dbg
            fflush(dbg)
            next
          }
          print
        }
      ' | tee "$LOGFILE"
else
  "${CMD[@]}" 2>&1 | tee "$LOGFILE"
fi

# Print quick summary: count games + grep for sprt decision
if [[ -f "$PGNFILE" ]]; then
  G=$(grep -c '^\[Result ' "$PGNFILE" || true)
  echo "=== SPRT finished (or interrupted) ==="
  echo "Games played: $G"
fi
if grep -qiE 'SPRT:.*(H1|H0).*accepted' "$LOGFILE"; then
  grep -iE 'SPRT:.*accepted' "$LOGFILE" | tail -1
else
  echo "SPRT decision not found in log (may have been interrupted)."
fi
