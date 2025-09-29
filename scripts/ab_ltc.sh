#!/usr/bin/env bash
set -euo pipefail

# === Config ===
CUTECHESS="${CUTECHESS:-cutechess-cli}"
ENGINE_A="${ENGINE_A:-/ruta/a/engine_A}"
ENGINE_B="${ENGINE_B:-/ruta/a/engine_B}"
NAME_A="${NAME_A:-A}"
NAME_B="${NAME_B:-B}"

# Preferido: PGN en parejas espejo; si apuntas a .epd, el script lo respeta
OPENINGS_FILE="${OPENINGS_FILE:-/ruta/a/libro_parejas_espejo.pgn}"
TC="${TC:-60+0.6}"          # Long Time Control
GAMES="${GAMES:-6000}"

# Concurrency por defecto: nproc/2 (mínimo 1)
CORES="$(nproc)"
DEF_CONC=$(( CORES / 2 ))
(( DEF_CONC < 1 )) && DEF_CONC=1
CONCURRENCY="${CONCURRENCY:-$DEF_CONC}"

OUT_BASENAME="${OUT_BASENAME:-results/ltc_confirm}"
PGN_OUT="${PGN_OUT:-${OUT_BASENAME}.pgn}"
LOG_OUT="${LOG_OUT:-${OUT_BASENAME}.log}"
mkdir -p "$(dirname "$PGN_OUT")"

# === Detecta formato por extensión ===
EXT="${OPENINGS_FILE##*.}"
FORMAT="pgn"; [[ "${EXT,,}" == "epd" ]] && FORMAT="epd"

OPENINGS_ARGS=( -openings file="$OPENINGS_FILE" format="$FORMAT" order=random )
# En LTC, con PGN salimos tras 12 plies para entrar en mediojuegos variados
if [[ "$FORMAT" == "pgn" ]]; then
  OPENINGS_ARGS+=( plies=12 start=1 )
fi

"$CUTECHESS" \
  -engine cmd="$ENGINE_A" name="$NAME_A" \
  -engine cmd="$ENGINE_B" name="$NAME_B" \
  -each proto=uci tc="$TC" \
  "${OPENINGS_ARGS[@]}" \
  -repeat \
  -games "$GAMES" \
  -concurrency "$CONCURRENCY" \
  -pgnout "$PGN_OUT" | tee "$LOG_OUT"
  #-draw movenumber=100 movecount=5 score=10 \
  #-resign movecount=8 score=600 \