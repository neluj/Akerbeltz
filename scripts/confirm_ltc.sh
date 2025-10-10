#!/usr/bin/env bash
# Long Time Control confirmation with cutechess-cli.
# - Round-robin across TSV engines by default (or --pairwise for A vs B).
# - PGN openings (curated) recommended; color-balanced with -repeat + -games 2.
# - Default TC: 40/300+3 (fallback to 45+0.45 with --tc if preferred).
# - Prints standings and writes JSON summary to logs/ltc_<timestamp>.json.

set -euo pipefail

# --- Defaults ---
TC_DEFAULT="40/300+3"
HASH_DEFAULT=256
THREADS_DEFAULT=1
BOOK_PLIES_DEFAULT=8
ROUNDS_DEFAULT=20
ADJ_RESIGN_SCORE=900
ADJ_RESIGN_MOVES=10
ADJ_DRAW_MOVENUM=80
ADJ_DRAW_MOVES=12
ADJ_DRAW_SCORE=10

PAIRWISE=false
INCLUDE_CSV=""
A_NAME_OVERRIDE=""
B_NAME_OVERRIDE=""

CONCURRENCY_OVERRIDE=""
#GLOBAL_THREADS=""
#GLOBAL_HASH=""
#SEED=""
ENGINES_TSV=""
OPENINGS_FILE=""
TC="$TC_DEFAULT"
ROUNDS="$ROUNDS_DEFAULT"

die(){ echo "ERROR: $*" >&2; exit 1; }
need(){ command -v "$1" >/dev/null 2>&1 || die "'$1' not found in PATH."; }
timestamp(){ date +"%Y%m%d_%H%M%S"; }

usage(){
  cat <<EOF
Usage: $0 --engines engines.tsv --openings file.pgn [options]
Required:
  --engines <path.tsv>
  --openings <path.pgn>
Optional general:
  --tc <tc>                 Default: ${TC_DEFAULT}
  --rounds <N>              Default: ${ROUNDS_DEFAULT} (per pairing)
  --threads <N>             Global default if TSV empty (default ${THREADS_DEFAULT})
  --hash <MB>               Global default if TSV empty (default ${HASH_DEFAULT})
  --concurrency <N>         Default: auto max(1,nproc-1)
  --seed <int>              RNG seed
  --adjudicate              Enable conservative adjudication
Round-robin / selection:
  --pairwise                Use A vs B only
  --a <name>  --b <name>    Pairwise engine selection
  --include name1,name2,..  Round-robin subset by 'name' from TSV
Openings:
  --book-depth <ply>        If using EPD instead of PGN (not typical), default ${BOOK_PLIES_DEFAULT}
EOF
}

# --- Parse CLI ---
BOOK_PLIES="$BOOK_PLIES_DEFAULT"
ADJUDICATE=false
while [[ $# -gt 0 ]]; do
  case "$1" in
    --engines) ENGINES_TSV="${2:-}"; shift 2;;
    --openings) OPENINGS_FILE="${2:-}"; shift 2;;
    --tc) TC="${2:-}"; shift 2;;
    --rounds) ROUNDS="${2:-}"; shift 2;;
    #--threads) GLOBAL_THREADS="${2:-}"; shift 2;;
    #--hash) GLOBAL_HASH="${2:-}"; shift 2;;
    --concurrency) CONCURRENCY_OVERRIDE="${2:-}"; shift 2;;
    #--seed) SEED="${2:-}"; shift 2;;
    --adjudicate) ADJUDICATE=true; shift;;
    --pairwise) PAIRWISE=true; shift;;
    --a) A_NAME_OVERRIDE="${2:-}"; shift 2;;
    --b) B_NAME_OVERRIDE="${2:-}"; shift 2;;
    --include) INCLUDE_CSV="${2:-}"; shift 2;;
    --book-depth) BOOK_PLIES="${2:-}"; shift 2;;
    -h|--help) usage; exit 0;;
    *) die "Unknown option: $1";;
  esac
done

need cutechess-cli; need awk; need sed; need grep; need nproc; need tee; need tr
[[ -n "$ENGINES_TSV" && -f "$ENGINES_TSV" ]] || { usage; die "--engines required"; }
[[ -n "$OPENINGS_FILE" && -f "$OPENINGS_FILE" ]] || { usage; die "--openings required"; }

mkdir -p ./logs ./pgn

# Concurrency (auto)
if [[ -n "$CONCURRENCY_OVERRIDE" ]]; then
  CONCURRENCY="$CONCURRENCY_OVERRIDE"
else
  CPU="$(nproc)"; CONCURRENCY=$(( CPU>1 ? CPU-1 : 1 ))
fi

# Seed
#if [[ -z "$SEED" ]]; then SEED="$(( (RANDOM<<16) ^ RANDOM ))"; fi

# TSV lines
mapfile -t LINES < <(awk -F'\t' 'NR==1{next} NF && $0!~/^[ \t]*#/{print}' "$ENGINES_TSV")
[[ ${#LINES[@]} -ge 2 ]] || die "Need at least two engines in TSV."

declare -A ENGINE_BY_NAME
declare -a NAMES
for ln in "${LINES[@]}"; do
  n="$(echo -e "$ln"|awk -F'\t' '{print $1}')"; ENGINE_BY_NAME["$n"]="$ln"; NAMES+=("$n")
done

# Selection of engines
SELECTED=()
if $PAIRWISE; then
  [[ -n "$A_NAME_OVERRIDE" && -n "$B_NAME_OVERRIDE" ]] || {
    A_NAME_OVERRIDE="${NAMES[0]}"; B_NAME_OVERRIDE="${NAMES[1]:-}"; [[ -n "$B_NAME_OVERRIDE" ]] || die "Need two engines."
  }
  SELECTED+=("$A_NAME_OVERRIDE" "$B_NAME_OVERRIDE")
else
  if [[ -n "$INCLUDE_CSV" ]]; then
    IFS=',' read -ra inc <<<"$INCLUDE_CSV"
    for name in "${inc[@]}"; do
      [[ -n "${ENGINE_BY_NAME[$name]:-}" ]] || die "Engine '$name' not in TSV"
      SELECTED+=("$name")
    done
  else
    SELECTED=("${NAMES[@]}")
  fi
fi

[[ ${#SELECTED[@]} -ge 2 ]] || die "Need at least two engines selected."

# Build engines and compute effective per-engine threads to avoid oversubscription
TOTAL_CPU="$(nproc)"
if [[ -n "${GLOBAL_THREADS:-}" ]]; then base_thr="$GLOBAL_THREADS"; else base_thr="$THREADS_DEFAULT"; fi
# If threads*CONCURRENCY > TOTAL_CPU, reduce threads
eff_thr="$base_thr"
if (( base_thr * CONCURRENCY > TOTAL_CPU )); then
  eff_thr=$(( TOTAL_CPU / CONCURRENCY ))
  (( eff_thr < 1 )) && eff_thr=1
fi

build_engine(){
  local name="$1"; local line="${ENGINE_BY_NAME[$name]}"
  IFS=$'\t' read -r _name cmd args thr hash uci <<<"$line"
  [[ -x "$cmd" ]] || die "Engine binary not executable: $cmd"
  #local threads="${thr:-$eff_thr}"
  #local hashmb="${hash:-${GLOBAL_HASH:-$HASH_DEFAULT}}"
  #local optstr=" option.Hash=${hashmb} option.Threads=${threads}"
  local optstr=""   #Remove if some param is used
  if [[ -n "${uci:-}" ]]; then
    IFS=';' read -ra opts <<<"$uci"
    for kv in "${opts[@]}"; do [[ -z "$kv" ]]&&continue; key="${kv%%=*}"; val="${kv#*=}"; optstr+=" option.${key}=${val}"; done
  fi
  local argstr=""; [[ -n "${args:-}" ]] && argstr=" args=${args}"
  echo "-engine name=${name} cmd=${cmd}${argstr} proto=uci${optstr}"
}

ENGINE_CLAUSES=()
for n in "${SELECTED[@]}"; do ENGINE_CLAUSES+=( "$(build_engine "$n")" ); done

# Openings: prefer PGN; support EPD as fallback
ext="${OPENINGS_FILE##*.}"; lower_ext="$(echo "$ext"|tr '[:upper:]' '[:lower:]')"
OPEN_FLAGS=()
case "$lower_ext" in
  pgn) OPEN_FLAGS=( -openings "file=${OPENINGS_FILE}" "format=pgn" "policy=round" );;
  epd) OPEN_FLAGS=( -openings "file=${OPENINGS_FILE}" "format=epd" "order=random" "plies=${BOOK_PLIES}" );;
  *) die "Openings must be .pgn (preferred) or .epd";;
esac

ADJ_FLAGS=()
$ADJUDICATE && {
  ADJ_FLAGS+=( -resign "movecount=${ADJ_RESIGN_MOVES}" "score=${ADJ_RESIGN_SCORE}" )
  ADJ_FLAGS+=( -draw "movenumber=${ADJ_DRAW_MOVENUM}" "movecount=${ADJ_DRAW_MOVES}" "score=${ADJ_DRAW_SCORE}" )
}

ts="$(timestamp)"
TAG=$([[ $PAIRWISE == true ]] && echo "ltc_pairwise" || echo "ltc_rr")
LOGFILE="./logs/${TAG}_${ts}.log"
PGNFILE="./pgn/${TAG}_${ts}.pgn"
JSONFILE="./logs/ltc_${ts}.json"

echo "=== LTC configuration ==="
echo "Mode:          $([[ $PAIRWISE == true ]] && echo "pairwise" || echo "round-robin")"
echo "Engines TSV:   $ENGINES_TSV"
echo "Selected:      ${SELECTED[*]}"
echo "Openings:      $OPENINGS_FILE (format: $lower_ext) with color-swap"
echo "Rounds/Pair:   $ROUNDS   (games/round=2, repeat openings)"
echo "TC:            $TC"
echo "Concurrency:   $CONCURRENCY   (effective per-engine threads ~${eff_thr})"
#echo "Seed:          $SEED"
echo "Logs:          $LOGFILE"
echo "PGN:           $PGNFILE"
$ADJUDICATE && echo "Adjudication:  resign ${ADJ_RESIGN_SCORE}/${ADJ_RESIGN_MOVES}, draw ${ADJ_DRAW_MOVENUM}/${ADJ_DRAW_MOVES} score=${ADJ_DRAW_SCORE}"

#trap "echo 'Aborting…'; kill 0 || true" INT TERM

CMD=( cutechess-cli
  ${ENGINE_CLAUSES[@]}
  -each proto=uci tc="$TC" timemargin=50 ponder=false
  "${OPEN_FLAGS[@]}"
  -repeat
  -games 2
  -rounds "$ROUNDS"
  -concurrency "$CONCURRENCY"
  #-srand "$SEED"
  "${ADJ_FLAGS[@]}"
  -pgnout "$PGNFILE"
)

# Tournament mode: round-robin unless explicitly pairwise with two engines
if $PAIRWISE; then
  # cutechess default tournament is round-robin; with only two engines it's effectively head-to-head.
  :
else
  CMD+=( -tournament round-robin )
fi

set -x
"${CMD[@]}" 2>&1 | tee "$LOGFILE"
set +x

# --- Produce standings + JSON from PGN ---
# Parse PGN -> standings (console) + JSON without sed hacks (no trailing commas).
awk -v json="$JSONFILE" -v pgn="$PGNFILE" '
  BEGIN{
    FS="\""; OFS="";
  }
  /^\[White "/ { white=$2 }
  /^\[Black "/ { black=$2 }
  /^\[Result "/{
    res=$2
    if(res=="1-0"){ wpts=1; bpts=0; wwin=1; bwin=0; draw=0 }
    else if(res=="0-1"){ wpts=0; bpts=1; wwin=0; bwin=1; draw=0 }
    else { wpts=0.5; bpts=0.5; wwin=0; bwin=0; draw=1 }
    if(!(seen[white]++)) order[++ord]=white
    if(!(seen[black]++)) order[++ord]=black
    pts[white]+=wpts; pts[black]+=bpts
    wins[white]+=wwin; wins[black]+=bwin
    draws[white]+=draw; draws[black]+=draw
    losses[white]+= (res=="0-1") ? 1 : 0
    losses[black]+= (res=="1-0") ? 1 : 0
    games[white]++; games[black]++
  }
  END{
    # Console standings
    for(i=1;i<=ord;i++){
      n=order[i]
      p=(pts[n]+0)
      w=(wins[n]+0); d=(draws[n]+0); l=(losses[n]+0); g=(games[n]+0)
      sc=(g>0)?(100.0*p/g):0
      printf "%-24s  Pts: %6.1f  W:%4d  D:%4d  L:%4d  G:%4d  Score: %6.1f%%\n", n, p, w, d, l, g, sc
    }

    # Proper JSON output
    print "{" > json
    print "  \"meta\": {\"pgn\": \"" pgn "\"}," >> json
    print "  \"standings\": [" >> json
    for(i=1;i<=ord;i++){
      n=order[i]
      p=(pts[n]+0)
      w=(wins[n]+0); d=(draws[n]+0); l=(losses[n]+0); g=(games[n]+0)
      sc=(g>0)?(100.0*p/g):0
      printf "    {\"engine\":\"%s\",\"points\":%.1f,\"wins\":%d,\"draws\":%d,\"losses\":%d,\"games\":%d,\"score_pct\":%.2f}", n, p, w, d, l, g, sc >> json
      if(i<ord) printf ",\n" >> json; else printf "\n" >> json
    }
    print "  ]" >> json
    print "}" >> json
  }
' "$PGNFILE"

echo "JSON standings written to: $JSONFILE"