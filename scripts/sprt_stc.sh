#!/usr/bin/env bash
set -euo pipefail

# === Config ===
CUTECHESS="${CUTECHESS:-cutechess-cli}"
ENGINE_A="${ENGINE_A:-/ruta/a/engine_A}"
ENGINE_B="${ENGINE_B:-/ruta/a/engine_B}"
NAME_A="${NAME_A:-A}"
NAME_B="${NAME_B:-B}"

# Recomendado: EPD UHO
OPENINGS_FILE="${OPENINGS_FILE:-/ruta/a/UHO_2024_8mvs_+085_+094.epd}"
TC="${TC:-10+0.1}"

# SPRT
ELO0="${ELO0:-0}"
ELO1="${ELO1:-5}"
ALPHA="${ALPHA:-0.05}"
BETA="${BETA:-0.05}"

# Concurrency por defecto: nproc/2 (ajústalo si ves timeouts)
CORES="$(nproc)"
DEF_CONC=$(( CORES / 2 )); (( DEF_CONC < 1 )) && DEF_CONC=1
CONCURRENCY="${CONCURRENCY:-$DEF_CONC}"

OUT_BASENAME="${OUT_BASENAME:-results/stc_sprt}"
PGN_OUT="${PGN_OUT:-${OUT_BASENAME}.pgn}"
LOG_OUT="${LOG_OUT:-${OUT_BASENAME}.log}"
mkdir -p "$(dirname "$PGN_OUT")"

# === Detecta formato por extensión ===
EXT="${OPENINGS_FILE##*.}"
FORMAT="epd"; [[ "${EXT,,}" == "pgn" ]] && FORMAT="pgn"

OPENINGS_ARGS=( -openings file="$OPENINGS_FILE" format="$FORMAT" order=random )
if [[ "$FORMAT" == "pgn" ]]; then
  OPENINGS_ARGS+=( plies=12 start=1 )
fi

"$CUTECHESS" \
  -engine cmd="$ENGINE_A" name="$NAME_A" \
  -engine cmd="$ENGINE_B" name="$NAME_B" \
  -each proto=uci tc="$TC" \
  "${OPENINGS_ARGS[@]}" \
  -repeat \
  -games 2 \
  -sprt "elo0=${ELO0}" "elo1=${ELO1}" "alpha=${ALPHA}" "beta=${BETA}" \
  -concurrency "$CONCURRENCY" \
  -pgnout "$PGN_OUT" | tee "$LOG_OUT"
  #-draw movenumber=80 movecount=5 score=10 \
  #-resign movecount=8 score=600 \
