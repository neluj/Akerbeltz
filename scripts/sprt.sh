#!/usr/bin/env bash
set -euo pipefail

# --- BINARIOS / RUTAS ---
CUTECHESS="cutechess-cli"
ENGINE_A_DIR="/home/julen/code/binaries"
ENGINE_A="Xake-V113"
NAME_A="Xake-NEW"            # candidato
ENGINE_B_DIR="/home/julen/code/binaries"
ENGINE_B="Xake-V119"
NAME_B="Xake-OLD"            # baseline
OPENINGS_FILE="/home/julen/code/binaries/UHO_2024/UHO_2024_+085_+094/UHO_2024_8mvs_+085_+094.epd"

# --- PARÁMETROS BÁSICOS ---
TC="60+0.6"
CONCURRENCY=4

# --- SPRT (hipótesis en Elo) ---
ELO0=0
ELO1=3
ALPHA=0.05
BETA=0.05

# Tope de seguridad (opcional): total = ROUNDS * GPR
MAXGAMES=10000              # usa un número par
GPR=2                      # games por ronda (ida/vuelta)
ROUNDS=$(( MAXGAMES / GPR ))

OPENINGS_FORMAT="epd"
OPENINGS_PLIES=16
OPENINGS_ORDER="random"

OUTDIR="./results"
mkdir -p "$OUTDIR"
STAMP="$(date +%Y%m%d-%H%M%S)"
PGN_OUT="$OUTDIR/sprt-${NAME_A}_vs_${NAME_B}-${STAMP}.pgn"
EVENT_NAME="SPRT ${NAME_A} vs ${NAME_B} (E0=${ELO0}, E1=${ELO1})"

"$CUTECHESS" \
  -concurrency "$CONCURRENCY" \
  -engine cmd="${ENGINE_A_DIR}/${ENGINE_A}" name="$NAME_A" proto=uci \
  -engine cmd="${ENGINE_B_DIR}/${ENGINE_B}" name="$NAME_B" proto=uci \
  -each tc="$TC" timemargin=1000 \
  -openings file="$OPENINGS_FILE" format="$OPENINGS_FORMAT" order="$OPENINGS_ORDER" plies="$OPENINGS_PLIES" \
  -repeat \
  -sprt elo0="$ELO0" elo1="$ELO1" alpha="$ALPHA" beta="$BETA" \
  -rounds "$ROUNDS" -games "$GPR" \
  #-draw movenumber=80 movecount=5 score=10 \
  #-resign movecount=3 score=600 \
  -event "$EVENT_NAME" \
  -pgnout "$PGN_OUT"
