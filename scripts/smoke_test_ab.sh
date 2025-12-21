#!/usr/bin/env bash
# Fast A/B smoke test with cutechess-cli.
# - Reads engines from TSV, picks first two by default (or --a/--b).
# - EPD openings, plays both colors per position (-repeat + -games 2).
# - Sensible defaults: --rounds 50 (-> 100 effective games), --tc 3+0.05.
# - Writes logs to ./logs and PGN to ./pgn with timestamped names.
# - Portable Bash, strict mode, clear failure on missing prerequisites.
# - All echoes/logs are in English.

set -euo pipefail

# --- Defaults ---
ROUNDS_DEFAULT=50
TC_DEFAULT="3+0.05"
HASH_DEFAULT=256
THREADS_DEFAULT=1
BOOK_PLIES_DEFAULT=8
ADJ_RESIGN_SCORE=600     # resign if eval <= -600
ADJ_RESIGN_MOVES=8
ADJ_DRAW_MOVENUM=40
ADJ_DRAW_MOVES=8
ADJ_DRAW_SCORE=10
CONCURRENCY_OVERRIDE=""
A_NAME_OVERRIDE=""
B_NAME_OVERRIDE=""
DEBUG=false
#SEED=""
ENGINES_TSV=""
OPENINGS_FILE=""
GLOBAL_THREADS=""
GLOBAL_HASH=""
TC="$TC_DEFAULT"

# --- Helpers ---
die() { echo "ERROR: $*" >&2; exit 1; }
need() { command -v "$1" >/dev/null 2>&1 || die "'$1' not found in PATH."; }

timestamp() { date +"%Y%m%d_%H%M%S"; }

usage() {
  cat <<EOF
Usage: $0 --engines engines.tsv --openings file.epd [options]
Required:
  --engines <path.tsv>     Engines TSV (name,cmd,args,threads,hash,uci_options)
  --openings <path.epd>    Openings EPD (will randomize; both colors per line)
Optional:
  --a <name>               Engine A name (must match TSV 'name')
  --b <name>               Engine B name (must match TSV 'name')
  --rounds <N>             Rounds (default: ${ROUNDS_DEFAULT})
  --tc <tc>                Time control (cutechess format; default: ${TC_DEFAULT})
  --threads <N>            Global threads if TSV empty (optional)
  --hash <MB>              Global hash if TSV empty (optional)
  --book-depth <ply>       EPD plies (default: ${BOOK_PLIES_DEFAULT})
  --concurrency <N>        Concurrency (default: auto = max(1,nproc-1))
  --adjudicate             Enable resign/draw adjudication (defaults baked in)
  --seed <int>             RNG seed (uses -srand)
  --debug <true|false>     If true, enable cutechess -debug and write UCI I/O to logs/<tag>.debug
EOF
}

# --- Parse CLI ---
ROUNDS="$ROUNDS_DEFAULT"
BOOK_PLIES="$BOOK_PLIES_DEFAULT"
ADJUDICATE=false
while [[ $# -gt 0 ]]; do
  case "$1" in
    --engines) ENGINES_TSV="${2:-}"; shift 2;;
    --openings) OPENINGS_FILE="${2:-}"; shift 2;;
    --rounds) ROUNDS="${2:-}"; shift 2;;
    --tc) TC="${2:-}"; shift 2;;
    --threads) GLOBAL_THREADS="${2:-$THREADS_DEFAULT}"; shift 2;;
    --hash) GLOBAL_HASH="${2:-$HASH_DEFAULT}"; shift 2;;
    --book-depth) BOOK_PLIES="${2:-}"; shift 2;;
    --concurrency) CONCURRENCY_OVERRIDE="${2:-}"; shift 2;;
    --adjudicate) ADJUDICATE=true; shift;;
    #--seed) SEED="${2:-}"; shift 2;;
    --a) A_NAME_OVERRIDE="${2:-}"; shift 2;;
    --b) B_NAME_OVERRIDE="${2:-}"; shift 2;;
    --debug)
      case "${2:-}" in
        true|TRUE|True|1|yes|on) DEBUG=true;;
        *) DEBUG=false;;
      esac
      shift 2;;
    -h|--help) usage; exit 0;;
    *) die "Unknown option: $1";;
  esac
done

# --- Preconditions ---
need bash
need awk
need grep
need sed
need date
need mkdir
need nproc
need tee
need stdbuf
need cutechess-cli

[[ -n "$ENGINES_TSV" ]] || { usage; die "--engines is required."; }
[[ -n "$OPENINGS_FILE" ]] || { usage; die "--openings is required."; }
[[ -f "$ENGINES_TSV" ]] || die "Engines TSV not found: $ENGINES_TSV"
[[ -f "$OPENINGS_FILE" ]] || die "Openings file not found: $OPENINGS_FILE"

mkdir -p ./logs ./pgn

# --- Concurrency auto ---
if [[ -n "$CONCURRENCY_OVERRIDE" ]]; then
  CONCURRENCY="$CONCURRENCY_OVERRIDE"
else
  CPU="$(nproc)"
  if (( CPU > 1 )); then CONCURRENCY=$((CPU-1)); else CONCURRENCY=1; fi
fi

# --- Seed ---
#if [[ -z "$SEED" ]]; then
  # random 31-bit seed
#  SEED="$(( (RANDOM<<16) ^ RANDOM ))"
#fi

# --- TSV parsing to arrays ---
mapfile -t LINES < <(awk -F'\t' 'BEGIN{OFS="\t"} NR==1{
  # Expect header: name  cmd  args  threads  hash  uci_options
  for(i=1;i<=NF;i++){h[$i]=i} 
  if(!(h["name"]&&h["cmd"])) { print "Bad header in TSV"; exit 2}
  next
}
NF && $0 !~ /^[ \t]*#/ {print $0}' "$ENGINES_TSV") || die "Failed to parse TSV."

[[ ${#LINES[@]} -ge 1 ]] || die "No engines found in TSV."

get_field() { # usage: get_field "$line" "colname"
  local line="$1" key="$2"
  echo -e "$line" | awk -F'\t' -v k="$key" 'BEGIN{OFS="\t"} NR==1{next}{print $0}' # dummy
}

# Build an associative map by name -> full line for quick lookup
declare -A ENGINE_BY_NAME
for ln in "${LINES[@]}"; do
  name="$(echo -e "$ln" | awk -F'\t' '{print $1}')"
  ENGINE_BY_NAME["$name"]="$ln"
done

# Pick A/B
pick_first_two() {
  local first="${LINES[0]}"
  local second="${LINES[1]:-}"
  [[ -n "$second" ]] || die "TSV must contain at least two engines or specify --a/--b."
  echo -e "$first\n$second"
}

resolve_engine_line() {
  local name="$1"
  local line="${ENGINE_BY_NAME[$name]:-}"
  [[ -n "$line" ]] || die "Engine '$name' not found in TSV."
  echo "$line"
}

if [[ -n "$A_NAME_OVERRIDE" && -n "$B_NAME_OVERRIDE" ]]; then
  A_LINE="$(resolve_engine_line "$A_NAME_OVERRIDE")"
  B_LINE="$(resolve_engine_line "$B_NAME_OVERRIDE")"
else
  mapfile -t PAIR < <(pick_first_two)
  A_LINE="${PAIR[0]}"
  B_LINE="${PAIR[1]}"
  A_NAME_OVERRIDE="$(echo -e "$A_LINE" | awk -F'\t' '{print $1}')"
  B_NAME_OVERRIDE="$(echo -e "$B_LINE" | awk -F'\t' '{print $1}')"
fi

# --- Build -engine clause from TSV line ---
# TSV cols: name  cmd  args  threads  hash  uci_options
build_engine_clause() {
  local line="$1"
  local g_threads="${GLOBAL_THREADS:-}"
  local g_hash="${GLOBAL_HASH:-}"
  local name cmd args thr hash uci raw opts optstr=""
  local has_hash_opt=false
  local has_threads_opt=false

  IFS=$'\t' read -r name cmd args thr hash uci <<<"$line"

  [[ -x "$cmd" || -f "$cmd" ]] || die "Engine binary not found: $cmd"
  [[ -x "$cmd" ]] || die "Engine is not executable: $cmd"

  # per-engine overrides
  local threads="${thr:-$g_threads}"
  local hashmb="${hash:-$g_hash}"

  # uci_options: Key=Val;Key2=Val2  -> option.Key=Val option.Key2=Val2
  if [[ -n "${uci:-}" ]]; then
    IFS=';' read -ra opts <<<"$uci"
    for kv in "${opts[@]}"; do
      [[ -z "$kv" ]] && continue
      key="${kv%%=*}"; val="${kv#*=}"
      case "${key,,}" in
        hash) has_hash_opt=true;;
        threads) has_threads_opt=true;;
      esac
      optstr+=" option.${key}=${val}"
    done
  fi

  # Attach Hash/Threads only if provided and not already set in uci_options
  if [[ -n "${hashmb:-}" && $has_hash_opt == false ]]; then
    optstr+=" option.Hash=${hashmb}"
  fi
  if [[ -n "${threads:-}" && $has_threads_opt == false ]]; then
    optstr+=" option.Threads=${threads}"
  fi

  # Extra args (optional)
  local argstr=""
  if [[ -n "${args:-}" ]]; then
    argstr=" args=${args}"
  fi

  echo "-engine name=${name} cmd=${cmd}${argstr} proto=uci${optstr}"
}

ENG_A="$(build_engine_clause "$A_LINE")"
ENG_B="$(build_engine_clause "$B_LINE")"

# --- Try to print engine versions (best effort) ---
print_engine_id() {
  local name="$1" cmd="$2"
  if command -v timeout >/dev/null 2>&1; then
    echo "Probing engine '$name' id (name/author)..."
    if timeout 3s bash -c "printf 'uci\nquit\n' | \"$cmd\" 2>/dev/null | head -n 50 | grep -E '^(id name|id author)'" || true; then
      :
    fi
  fi
}

print_engine_id "$A_NAME_OVERRIDE" "$(echo -e "$A_LINE" | awk -F'\t' '{print $2}')"
print_engine_id "$B_NAME_OVERRIDE" "$(echo -e "$B_LINE" | awk -F'\t' '{print $2}')"

# --- Openings handling (EPD) ---
ext="${OPENINGS_FILE##*.}"
shopt -s nocasematch
[[ "$ext" =~ ^epd$ ]] || die "Smoke test expects EPD openings."
shopt -u nocasematch

[[ -r "$OPENINGS_FILE" ]] || die "Openings file not readable: $OPENINGS_FILE"

# --- Paths for outputs ---
ts="$(timestamp)"
LOGFILE="./logs/smoke_${A_NAME_OVERRIDE}_vs_${B_NAME_OVERRIDE}_${ts}.log"
PGNFILE="./pgn/smoke_${A_NAME_OVERRIDE}_vs_${B_NAME_OVERRIDE}_${ts}.pgn"
DEBUGFILE="./logs/smoke_${A_NAME_OVERRIDE}_vs_${B_NAME_OVERRIDE}_${ts}.debug"

# --- Adjudication flags ---
ADJ_FLAGS=()
if $ADJUDICATE; then
  ADJ_FLAGS+=( -resign "movecount=${ADJ_RESIGN_MOVES}" "score=${ADJ_RESIGN_SCORE}" )
  ADJ_FLAGS+=( -draw "movenumber=${ADJ_DRAW_MOVENUM}" "movecount=${ADJ_DRAW_MOVES}" "score=${ADJ_DRAW_SCORE}" )
fi

echo "=== Smoke test configuration ==="
echo "cutechess-cli: $(command -v cutechess-cli)"
echo "Engines TSV:   $ENGINES_TSV"
echo "Engine A:      $A_NAME_OVERRIDE"
echo "Engine B:      $B_NAME_OVERRIDE"
echo "Openings EPD:  $OPENINGS_FILE   (plies=$BOOK_PLIES, order=random, color-swap)"
echo "Rounds:        $ROUNDS   (effective games = rounds * 2)"
echo "TC:            $TC"
echo "Concurrency:   $CONCURRENCY"
#echo "Seed:          $SEED"
echo "Logs:          $LOGFILE"
echo "PGN:           $PGNFILE"
$ADJUDICATE && echo "Adjudication:  resign ${ADJ_RESIGN_SCORE}/${ADJ_RESIGN_MOVES}, draw ${ADJ_DRAW_MOVENUM}/${ADJ_DRAW_MOVES} score=${ADJ_DRAW_SCORE}"
echo "Debug:         $([[ $DEBUG == true ]] && echo "$DEBUGFILE (enabled, UCI I/O to file)" || echo "off")"

#trap "echo 'Aborting…'; kill 0 || true" INT TERM
#Add the next line to cutechess-cli for adding seed by parameter
#-srand "$SEED" \

# --- Build command ---
CMD=( cutechess-cli
  $ENG_A
  $ENG_B
  -each proto=uci tc="$TC" timemargin=50 ponder=false
  -openings "file=$OPENINGS_FILE" "format=epd" "order=random" "plies=$BOOK_PLIES"
  -repeat
  -games 2
  -rounds "$ROUNDS"
  -concurrency "$CONCURRENCY"
  "${ADJ_FLAGS[@]}"
  -pgnout "$PGNFILE"
)

# Enable cutechess internal debug only if requested
$DEBUG && CMD+=( -debug all )

# --- Execution (with optional UCI I/O capture to .debug) ---
if $DEBUG; then
  {
    echo "=== DEBUG $(date) ==="
    echo "Workdir: $(pwd)"
    echo "nproc: $(nproc)"
    echo "--- Full command ---"
    printf '%q ' "${CMD[@]}"; echo
    echo "--- BEGIN UCI I/O ---"
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
          # cutechess -debug emits lines like: "NNN <engine>(idx): ..." or "NNN >engine(idx): ..."
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

# --- Summary from PGN ---
# Count results from PGN [Result "..."]
if [[ -f "$PGNFILE" ]]; then
  W=$(grep -c '\[Result "1-0"\]' "$PGNFILE" || true)
  L=$(grep -c '\[Result "0-1"\]' "$PGNFILE" || true)
  D=$(grep -c '\[Result "1/2-1/2"\]' "$PGNFILE" || true)
  G=$((W+L+D))
  echo "=== Smoke test result ==="
  echo "Games: $G   W: $W   L: $L   D: $D"
  echo "PGN saved to: $PGNFILE"
  echo "Log saved to: $LOGFILE"
else
  die "cutechess did not produce PGN ($PGNFILE)."
fi
