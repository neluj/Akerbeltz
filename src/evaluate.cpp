#include "evaluate.h"
#include "position.h"

namespace Xake {
namespace Evaluate {

// ---------- Declaraciones ----------
void init();
static inline int flip(int sq) { return sq ^ 56; }
static inline Score mg_eg_blend(Score mg, Score eg, int phase);

static Bitboard ISOLATED_MASKS[SQ64_SIZE];
static Bitboard PASSED_MASKS[COLOR_SIZE][SQ64_SIZE];

static Score MG_PST[PIECE_SIZE][SQ64_SIZE];
static Score EG_PST[PIECE_SIZE][SQ64_SIZE];

// Peso de fase por pieza (similar a V140)
constexpr int PHASE_PIECE_WEIGHT[PIECE_SIZE] = {
/*NO*/ 0,
/*W_P*/ 0, /*W_N*/1, /*W_B*/1, /*W_R*/2, /*W_Q*/4, /*W_K*/0,
/*gap*/ 0, 0,
/*B_P*/ 0, /*B_N*/1, /*B_B*/1, /*B_R*/2, /*B_Q*/4, /*B_K*/0,
/*gap*/ 0
};
constexpr Score MAX_PHASE_PIECE_WEIGHT = 24;

// ---------- Material base MG/EG (escala compacta, segura) ----------
constexpr Score MG_PIECE[PIECETYPE_SIZE] = { 0, 100, 325, 330, 500, 975, 0 };
constexpr Score EG_PIECE[PIECETYPE_SIZE] = { 0, 100, 320, 330, 510, 975, 0 };

// ---------- PSQT MG/EG (64 c/u, base sólida, “tuned-friendly”) ----------
// Notación: tablas para BLANCAS. Para NEGRAS se hace flip(sq).
// Peón
static const Score MG_PAWN[64] = {
   0,  0,  0,  0,  0,  0,  0,  0,
 -10,  2, -6,-10, -8, 12, 16,-12,
 -12, -4, -2, -4,  4,  6, 14, -8,
 -10, -4,  0, 10, 12,  6,  8, -8,
  -6,  8,  6, 12, 14,  8, 10, -8,
  -2,  6, 16, 22, 32, 24, 12, -6,
  10, 18, 10, 22, 16, 26,  8, -2,
   0,  0,  0,  0,  0,  0,  0,  0,
};
static const Score EG_PAWN[64] = {
   0,  0,  0,  0,  0,  0,  0,  0,
   8,  6,  6,  8, 10,  2,  2, -4,
   2,  6, -2,  2,  2, -2,  0, -6,
  10,  8,  0, -2, -2, -2,  4,  0,
  20, 16,  8,  4,  0,  6, 14, 14,
  70, 78, 66, 54, 46, 46, 70, 72,
 150,145,130,115,125,115,140,155,
   0,  0,  0,  0,  0,  0,  0,  0,
};

// Caballo
static const Score MG_KNIGHT[64] = {
 -80,-30,-40,-30,-30,-40,-30,-80,
 -30, -5, -5,  0,  0, -5, -5,-30,
 -25,  0, 10, 12, 12, 10,  0,-20,
 -20,  0, 12, 20, 20, 12,  0,-20,
 -20,  0, 12, 20, 20, 12,  0,-20,
 -25,  0, 10, 12, 12, 10,  0,-20,
 -30, -5, -5,  0,  0, -5, -5,-30,
 -80,-30,-40,-30,-30,-40,-30,-80,
};
static const Score EG_KNIGHT[64] = {
 -60,-40,-35,-30,-30,-35,-40,-60,
 -35,-25,-15, -8, -8,-15,-25,-35,
 -30,-15,  0,  4,  4,  0,-15,-30,
 -28,-10,  4, 10, 10,  4,-10,-28,
 -28,-10,  4, 10, 10,  4,-10,-28,
 -30,-15,  0,  4,  4,  0,-15,-30,
 -35,-25,-15, -8, -8,-15,-25,-35,
 -60,-40,-35,-30,-30,-35,-40,-60,
};

// Alfil
static const Score MG_BISHOP[64] = {
 -30,-10,-15,-12,-12,-15,-10,-30,
  -8,  4,  4,  6,  6,  8,  4, -6,
  -8,  6,  8, 10, 10, 12,  6, -4,
  -6, 10, 10, 16, 18, 12, 10, -4,
  -6,  8, 10, 18, 20, 14,  8, -6,
 -10,  6, 10, 12, 12, 14,  6,-12,
 -14, -8, -4, -2, -2, -4, -8,-14,
 -22,-10,-12,-12,-12,-12,-10,-22,
};
static const Score EG_BISHOP[64] = {
 -22,-12,-10, -8, -8,-10,-12,-22,
 -10,  2,  4,  6,  6,  4,  2, -8,
  -8,  4,  8, 10, 10,  8,  4, -6,
  -6,  8, 10, 12, 12, 10,  8, -6,
  -6,  8, 10, 12, 12, 10,  8, -6,
  -8,  4,  8, 10, 10,  8,  4, -6,
 -10,  2,  4,  6,  6,  4,  2, -8,
 -22,-12,-10, -8, -8,-10,-12,-22,
};

// Torre
static const Score MG_ROOK[64] = {
 -10, -8, -6, -4, -4, -6, -8,-10,
  -6, -2,  0,  2,  2,  0, -2, -6,
  -6,  0,  2,  4,  4,  2,  0, -6,
  -6,  0,  4,  6,  6,  4,  0, -6,
  -6,  0,  4,  6,  6,  4,  0, -6,
  -6,  0,  2,  4,  4,  2,  0, -6,
  -6, -2,  0,  2,  2,  0, -2, -6,
 -10, -8, -6, -4, -4, -6, -8,-10,
};
static const Score EG_ROOK[64] = {
 -10, -8, -6, -4, -4, -6, -8,-10,
  -6, -2,  0,  2,  2,  0, -2, -6,
  -6,  0,  2,  4,  4,  2,  0, -6,
  -6,  0,  4,  6,  6,  4,  0, -6,
  -6,  0,  4,  6,  6,  4,  0, -6,
  -6,  0,  2,  4,  4,  2,  0, -6,
  -6, -2,  0,  2,  2,  0, -2, -6,
 -10, -8, -6, -4, -4, -6, -8,-10,
};

// Dama (PST contenida; el valor viene del material)
static const Score MG_QUEEN[64] = {
 -12,-10, -8, -6, -6, -8,-10,-12,
  -8, -6, -2,  0,  0, -2, -6, -8,
  -8, -2,  2,  4,  4,  2, -2, -8,
  -6,  0,  4,  8,  8,  4,  0, -6,
  -6,  0,  4,  8,  8,  4,  0, -6,
  -8, -2,  2,  4,  4,  2, -2, -8,
  -8, -6, -2,  0,  0, -2, -6, -8,
 -12,-10, -8, -6, -6, -8,-10,-12,
};
static const Score EG_QUEEN[64] = {
 -12,-10, -8, -6, -6, -8,-10,-12,
  -8, -6, -2,  0,  0, -2, -6, -8,
  -8, -2,  2,  4,  4,  2, -2, -8,
  -6,  0,  4,  8,  8,  4,  0, -6,
  -6,  0,  4,  8,  8,  4,  0, -6,
  -8, -2,  2,  4,  4,  2, -2, -8,
  -8, -6, -2,  0,  0, -2, -6, -8,
 -12,-10, -8, -6, -6, -8,-10,-12,
};

// Rey
static const Score MG_KING[64] = {
 -30,-40,-40,-50,-50,-40,-40,-30,
 -30,-40,-45,-55,-55,-45,-40,-30,
 -30,-38,-50,-60,-60,-50,-38,-30,
 -25,-30,-40,-55,-55,-40,-30,-25,
 -20,-20,-30,-40,-40,-30,-20,-20,
 -10,-10,-20,-25,-25,-20,-10,-10,
  10, 10,  0, -5, -5,  0, 10, 10,
  30, 40, 30,  5,  0, 10, 40, 30,
};
static const Score EG_KING[64] = {
 -40,-30,-20,-10,-10,-20,-30,-40,
 -30,-10,  0, 10, 10,  0,-10,-30,
 -20,  0, 15, 25, 25, 15,  0,-20,
 -10, 10, 25, 35, 35, 25, 10,-10,
 -10, 10, 25, 35, 35, 25, 10,-10,
 -20,  0, 15, 25, 25, 15,  0,-20,
 -30,-10,  0, 10, 10,  0,-10,-30,
 -40,-30,-20,-10,-10,-20,-30,-40,
};

// ---------- Estructura de peones ----------
// Penalización peón aislado (MG/EG) — moderada y segura en escala de 100
constexpr Score ISOLATED_PAWN_PENALTY_MG = -10;
constexpr Score ISOLATED_PAWN_PENALTY_EG = -18;

// Bonus peón pasado por rango (1..7; 8=coronación)
// MG: conservador; EG: más fuerte (principal fuerza aquí)
static const Score PASSED_MG[8] = { 0, 0,  4, 10, 20, 36, 60, 0 };
static const Score PASSED_EG[8] = { 0, 0,  8, 18, 34, 60,100, 0 };

// Descuentos contextuales
constexpr int PASSED_BLOCKED_NUM = 1;   // ×1/4 si bloqueado
constexpr int PASSED_BLOCKED_DEN = 4;
constexpr int PASSED_UNSAFE_NUM  = 1;   // ×1/2 si la casilla delante está controlada por el rival
constexpr int PASSED_UNSAFE_DEN  = 2;

// ---------- Helpers de máscaras ----------
static inline Bitboard file_mask(int file) {
    Bitboard m = 0ULL;
    for (int r = 0; r < 8; ++r) m |= (1ULL << (r * 8 + file));
    return m;
}
static inline Bitboard adjacent_files_mask(int file) {
    Bitboard m = 0ULL;
    if (file > 0) m |= file_mask(file - 1);
    if (file < 7) m |= file_mask(file + 1);
    return m;
}

// ---------- INIT: PST + máscaras ----------
void init() {
    // Construye PST completas (material base + offsets PST), con flip para negras
    // Orden de PieceType: {NO, P, N, B, R, Q, K}
    // Orden de Piece: {NO, W_P..W_K, gaps, B_P..B_K, gaps}

    // Primero, limpia
    for (int p = 0; p < PIECE_SIZE; ++p)
        for (int s = 0; s < SQ64_SIZE; ++s)
            MG_PST[p][s] = EG_PST[p][s] = 0;

    // Blancas
    for (int s = 0; s < SQ64_SIZE; ++s) {
        // NO_PIECE (0)
        MG_PST[Piece::NO_PIECE][s] = 0;
        EG_PST[Piece::NO_PIECE][s] = 0;
    }
    // W pieces
    for (int s = 0; s < SQ64_SIZE; ++s) {
        MG_PST[Piece::W_PAWN][s]   = MG_PIECE[PieceType::PAWN]   + MG_PAWN[s];
        MG_PST[Piece::W_KNIGHT][s] = MG_PIECE[PieceType::KNIGHT] + MG_KNIGHT[s];
        MG_PST[Piece::W_BISHOP][s] = MG_PIECE[PieceType::BISHOP] + MG_BISHOP[s];
        MG_PST[Piece::W_ROOK][s]   = MG_PIECE[PieceType::ROOK]   + MG_ROOK[s];
        MG_PST[Piece::W_QUEEN][s]  = MG_PIECE[PieceType::QUEEN]  + MG_QUEEN[s];
        MG_PST[Piece::W_KING][s]   = MG_PIECE[PieceType::KING]   + MG_KING[s];

        EG_PST[Piece::W_PAWN][s]   = EG_PIECE[PieceType::PAWN]   + EG_PAWN[s];
        EG_PST[Piece::W_KNIGHT][s] = EG_PIECE[PieceType::KNIGHT] + EG_KNIGHT[s];
        EG_PST[Piece::W_BISHOP][s] = EG_PIECE[PieceType::BISHOP] + EG_BISHOP[s];
        EG_PST[Piece::W_ROOK][s]   = EG_PIECE[PieceType::ROOK]   + EG_ROOK[s];
        EG_PST[Piece::W_QUEEN][s]  = EG_PIECE[PieceType::QUEEN]  + EG_QUEEN[s];
        EG_PST[Piece::W_KING][s]   = EG_PIECE[PieceType::KING]   + EG_KING[s];
    }
    // Negras: flip tablero
    for (int s = 0; s < SQ64_SIZE; ++s) {
        int fs = flip(s);
        MG_PST[Piece::B_PAWN][s]   = MG_PIECE[PieceType::PAWN]   + MG_PAWN[fs];
        MG_PST[Piece::B_KNIGHT][s] = MG_PIECE[PieceType::KNIGHT] + MG_KNIGHT[fs];
        MG_PST[Piece::B_BISHOP][s] = MG_PIECE[PieceType::BISHOP] + MG_BISHOP[fs];
        MG_PST[Piece::B_ROOK][s]   = MG_PIECE[PieceType::ROOK]   + MG_ROOK[fs];
        MG_PST[Piece::B_QUEEN][s]  = MG_PIECE[PieceType::QUEEN]  + MG_QUEEN[fs];
        MG_PST[Piece::B_KING][s]   = MG_PIECE[PieceType::KING]   + MG_KING[fs];

        EG_PST[Piece::B_PAWN][s]   = EG_PIECE[PieceType::PAWN]   + EG_PAWN[fs];
        EG_PST[Piece::B_KNIGHT][s] = EG_PIECE[PieceType::KNIGHT] + EG_KNIGHT[fs];
        EG_PST[Piece::B_BISHOP][s] = EG_PIECE[PieceType::BISHOP] + EG_BISHOP[fs];
        EG_PST[Piece::B_ROOK][s]   = EG_PIECE[PieceType::ROOK]   + EG_ROOK[fs];
        EG_PST[Piece::B_QUEEN][s]  = EG_PIECE[PieceType::QUEEN]  + EG_QUEEN[fs];
        EG_PST[Piece::B_KING][s]   = EG_PIECE[PieceType::KING]   + EG_KING[fs];
    }

    // Máscaras de peón aislado (columna sin peones propios en adyacentes)
    for (int s = 0; s < SQ64_SIZE; ++s) {
        int file = s & 7; // 0..7
        ISOLATED_MASKS[s] = adjacent_files_mask(file);
    }

    // Máscaras de pasado: para cada color y casilla, columnas adyacentes + delante
    for (int s = 0; s < SQ64_SIZE; ++s) {
        int file = s & 7;
        Bitboard adj = adjacent_files_mask(file) | file_mask(file);

        // Blanco: delante = ranks superiores (mayor índice)
        Bitboard aheadW = 0ULL;
        int rankW = s >> 3;
        for (int r = rankW + 1; r < 8; ++r) aheadW |= (adj & (0xFFULL << (r * 8)));
        PASSED_MASKS[WHITE][s] = aheadW;

        // Negro: delante = ranks inferiores (menor índice)
        Bitboard aheadB = 0ULL;
        int rankB = s >> 3;
        for (int r = 0; r < rankB; ++r) aheadB |= (adj & (0xFFULL << (r * 8)));
        PASSED_MASKS[BLACK][s] = aheadB;
    }
}

// ---------- Eval principal ----------
Score calc_score(const Position& pos) {
    Score mg[COLOR_SIZE] = {0,0};
    Score eg[COLOR_SIZE] = {0,0};
    int phase = 0;

    // --- PST por piezas (sumas MG/EG + fase) ---
    // Peones
    Bitboard wp = pos.get_pieceTypes_bitboard(WHITE, PAWN);
    while (wp) {
        Square64 s{ __builtin_ctzll(wp) };
        mg[WHITE] += MG_PST[Piece::W_PAWN][s];
        eg[WHITE] += EG_PST[Piece::W_PAWN][s];
        phase += PHASE_PIECE_WEIGHT[Piece::W_PAWN];
        wp &= wp - 1;
    }
    Bitboard bp = pos.get_pieceTypes_bitboard(BLACK, PAWN);
    while (bp) {
        Square64 s{ __builtin_ctzll(bp) };
        mg[BLACK] += MG_PST[Piece::B_PAWN][s];
        eg[BLACK] += EG_PST[Piece::B_PAWN][s];
        phase += PHASE_PIECE_WEIGHT[Piece::B_PAWN];
        bp &= bp - 1;
    }

    // Caballos
    Bitboard wn = pos.get_pieceTypes_bitboard(WHITE, KNIGHT);
    while (wn) {
        Square64 s{ __builtin_ctzll(wn) };
        mg[WHITE] += MG_PST[Piece::W_KNIGHT][s];
        eg[WHITE] += EG_PST[Piece::W_KNIGHT][s];
        phase += PHASE_PIECE_WEIGHT[Piece::W_KNIGHT];
        wn &= wn - 1;
    }
    Bitboard bn = pos.get_pieceTypes_bitboard(BLACK, KNIGHT);
    while (bn) {
        Square64 s{ __builtin_ctzll(bn) };
        mg[BLACK] += MG_PST[Piece::B_KNIGHT][s];
        eg[BLACK] += EG_PST[Piece::B_KNIGHT][s];
        phase += PHASE_PIECE_WEIGHT[Piece::B_KNIGHT];
        bn &= bn - 1;
    }

    // Alfiles
    Bitboard wb = pos.get_pieceTypes_bitboard(WHITE, BISHOP);
    while (wb) {
        Square64 s{ __builtin_ctzll(wb) };
        mg[WHITE] += MG_PST[Piece::W_BISHOP][s];
        eg[WHITE] += EG_PST[Piece::W_BISHOP][s];
        phase += PHASE_PIECE_WEIGHT[Piece::W_BISHOP];
        wb &= wb - 1;
    }
    Bitboard bb = pos.get_pieceTypes_bitboard(BLACK, BISHOP);
    while (bb) {
        Square64 s{ __builtin_ctzll(bb) };
        mg[BLACK] += MG_PST[Piece::B_BISHOP][s];
        eg[BLACK] += EG_PST[Piece::B_BISHOP][s];
        phase += PHASE_PIECE_WEIGHT[Piece::B_BISHOP];
        bb &= bb - 1;
    }

    // Torres
    Bitboard wr = pos.get_pieceTypes_bitboard(WHITE, ROOK);
    while (wr) {
        Square64 s{ __builtin_ctzll(wr) };
        mg[WHITE] += MG_PST[Piece::W_ROOK][s];
        eg[WHITE] += EG_PST[Piece::W_ROOK][s];
        phase += PHASE_PIECE_WEIGHT[Piece::W_ROOK];
        wr &= wr - 1;
    }
    Bitboard br = pos.get_pieceTypes_bitboard(BLACK, ROOK);
    while (br) {
        Square64 s{ __builtin_ctzll(br) };
        mg[BLACK] += MG_PST[Piece::B_ROOK][s];
        eg[BLACK] += EG_PST[Piece::B_ROOK][s];
        phase += PHASE_PIECE_WEIGHT[Piece::B_ROOK];
        br &= br - 1;
    }

    // Damas
    Bitboard wq = pos.get_pieceTypes_bitboard(WHITE, QUEEN);
    while (wq) {
        Square64 s{ __builtin_ctzll(wq) };
        mg[WHITE] += MG_PST[Piece::W_QUEEN][s];
        eg[WHITE] += EG_PST[Piece::W_QUEEN][s];
        phase += PHASE_PIECE_WEIGHT[Piece::W_QUEEN];
        wq &= wq - 1;
    }
    Bitboard bq = pos.get_pieceTypes_bitboard(BLACK, QUEEN);
    while (bq) {
        Square64 s{ __builtin_ctzll(bq) };
        mg[BLACK] += MG_PST[Piece::B_QUEEN][s];
        eg[BLACK] += EG_PST[Piece::B_QUEEN][s];
        phase += PHASE_PIECE_WEIGHT[Piece::B_QUEEN];
        bq &= bq - 1;
    }

    // Reyes
    Bitboard wk = pos.get_pieceTypes_bitboard(WHITE, KING);
    while (wk) {
        Square64 s{ __builtin_ctzll(wk) };
        mg[WHITE] += MG_PST[Piece::W_KING][s];
        eg[WHITE] += EG_PST[Piece::W_KING][s];
        wk &= wk - 1;
    }
    Bitboard bk = pos.get_pieceTypes_bitboard(BLACK, KING);
    while (bk) {
        Square64 s{ __builtin_ctzll(bk) };
        mg[BLACK] += MG_PST[Piece::B_KING][s];
        eg[BLACK] += EG_PST[Piece::B_KING][s];
        bk &= bk - 1;
    }

    // --- Estructura de peones: aislados + pasados ---
    // Precalcula bitboards de peones por color
    Bitboard WP = pos.get_pieceTypes_bitboard(WHITE, PAWN);
    Bitboard BP = pos.get_pieceTypes_bitboard(BLACK, PAWN);

    // Aislados (no hay peón propio en columnas adyacentes)
    Bitboard wIso = 0ULL, bIso = 0ULL;
    Bitboard tmp = WP;
    while (tmp) {
        Square64 s{ __builtin_ctzll(tmp) };
        Bitboard sameFileAdj = ISOLATED_MASKS[s] & WP;
        if (!sameFileAdj) {
            mg[WHITE] += ISOLATED_PAWN_PENALTY_MG;
            eg[WHITE] += ISOLATED_PAWN_PENALTY_EG;
            wIso |= (1ULL << s);
        }
        tmp &= tmp - 1;
    }
    tmp = BP;
    while (tmp) {
        Square64 s{ __builtin_ctzll(tmp) };
        Bitboard sameFileAdj = ISOLATED_MASKS[s] & BP;
        if (!sameFileAdj) {
            mg[BLACK] += ISOLATED_PAWN_PENALTY_MG;
            eg[BLACK] += ISOLATED_PAWN_PENALTY_EG;
            bIso |= (1ULL << s);
        }
        tmp &= tmp - 1;
    }

    // Pasados: no hay peones rivales por delante en misma/adyacentes
    tmp = WP;
    Bitboard occ = pos.get_occupied_bitboard(WHITE) | pos.get_occupied_bitboard(BLACK);
    while (tmp) {
        Square64 s{ __builtin_ctzll(tmp) };
        if ((PASSED_MASKS[WHITE][s] & BP) == 0ULL) {
            int rank = (s >> 3) + 1; // 1..8
            Score addMG = PASSED_MG[rank];
            Score addEG = PASSED_EG[rank];

            // Descuento si bloqueado
            int aheadSq = s + 8; // casilla delante
            bool blocked = (aheadSq < 64) && ((occ & (1ULL << aheadSq)) != 0);
            if (blocked) { addMG = addMG * PASSED_BLOCKED_NUM / PASSED_BLOCKED_DEN;
                           addEG = addEG * PASSED_BLOCKED_NUM / PASSED_BLOCKED_DEN; }

            // Descuento si la casilla delante está controlada por rival (si tienes attacks_to)
            // if (pos.square_attacked_by(BLACK, aheadSq)) {
            //     addMG = addMG * PASSED_UNSAFE_NUM / PASSED_UNSAFE_DEN;
            //     addEG = addEG * PASSED_UNSAFE_NUM / PASSED_UNSAFE_DEN;
            // }

            mg[WHITE] += addMG;
            eg[WHITE] += addEG;
        }
        tmp &= tmp - 1;
    }
    tmp = BP;
    while (tmp) {
        Square64 s{ __builtin_ctzll(tmp) };
        if ((PASSED_MASKS[BLACK][s] & WP) == 0ULL) {
            int rankFromWhite = (s >> 3) + 1;           // 1..8
            int rankFromBlack = 9 - rankFromWhite;      // 1..8 relativo a negras
            Score addMG = PASSED_MG[rankFromBlack];
            Score addEG = PASSED_EG[rankFromBlack];

            int aheadSq = s - 8;
            bool blocked = (aheadSq >= 0) && ((occ & (1ULL << aheadSq)) != 0);
            if (blocked) { addMG = addMG * PASSED_BLOCKED_NUM / PASSED_BLOCKED_DEN;
                           addEG = addEG * PASSED_BLOCKED_NUM / PASSED_BLOCKED_DEN; }

            // if (pos.square_attacked_by(WHITE, aheadSq)) {
            //     addMG = addMG * PASSED_UNSAFE_NUM / PASSED_UNSAFE_DEN;
            //     addEG = addEG * PASSED_UNSAFE_NUM / PASSED_UNSAFE_DEN;
            // }

            mg[BLACK] += addMG;
            eg[BLACK] += addEG;
        }
        tmp &= tmp - 1;
    }

    // --- Mezcla MG/EG y devuelve relativo al lado al mover ---
    Color stm = pos.get_side_to_move();
    Score mgDiff = mg[stm] - mg[~stm];
    Score egDiff = eg[stm] - eg[~stm];

    if (phase > MAX_PHASE_PIECE_WEIGHT) phase = MAX_PHASE_PIECE_WEIGHT;
    return mg_eg_blend(mgDiff, egDiff, phase);
}

static inline Score mg_eg_blend(Score mg, Score eg, int phase) {
    // phase = 0 → puro EG; phase = MAX → puro MG
    return (mg * phase + eg * (MAX_PHASE_PIECE_WEIGHT - phase)) / MAX_PHASE_PIECE_WEIGHT;
}

bool material_draw(const Position& pos) {
    // Fast exit: if there are any pawns on the board, it's never a theoretical material draw.
    const Bitboard wP = pos.get_pieceTypes_bitboard(WHITE, PAWN);
    const Bitboard bP = pos.get_pieceTypes_bitboard(BLACK, PAWN);
    if (wP || bP) return false;

    const Bitboard wQ = pos.get_pieceTypes_bitboard(WHITE, QUEEN);
    const Bitboard bQ = pos.get_pieceTypes_bitboard(BLACK, QUEEN);
    const Bitboard wR = pos.get_pieceTypes_bitboard(WHITE, ROOK);
    const Bitboard bR = pos.get_pieceTypes_bitboard(BLACK, ROOK);

    // No rooks and no queens on the board
    if (!wR && !bR && !wQ && !bQ) {
        const Bitboard wB = pos.get_pieceTypes_bitboard(WHITE, BISHOP);
        const Bitboard bB = pos.get_pieceTypes_bitboard(BLACK, BISHOP);
        const Bitboard wN = pos.get_pieceTypes_bitboard(WHITE, KNIGHT);
        const Bitboard bN = pos.get_pieceTypes_bitboard(BLACK, KNIGHT);

        // Knights only (no bishops at all)
        if (!wB && !bB) {
            const int wn = Bitboards::cpop(wN);
            const int bn = Bitboards::cpop(bN);
            // K+N or K+NN vs K is a draw; K+NNN vs K can mate in theory (but is practically impossible),
            // so treat draws when the stronger side has < 3 knights and the weaker side has none.
            return (wn < 3 && bn == 0) || (bn < 3 && wn == 0);
        }
        // Bishops only (no knights at all)
        else if (!wN && !bN) {
            const int wb = Bitboards::cpop(wB);
            const int bb = Bitboards::cpop(bB);
            const int totalB = wb + bb;

            // KB vs K is a draw
            if (totalB == 1) return true;

            if (totalB == 2) {
                // Draw only when it's KB vs KB and both bishops are on the same color complex.
                // IMPORTANT: require exactly one bishop per side; otherwise KBB vs K would be wrongly flagged as a draw.
                if (wb == 1 && bb == 1) {
                    constexpr Bitboard DARK_SQUARES = 0xAA55AA55AA55AA55ULL;
                    const bool wDark = (wB & DARK_SQUARES) != 0;
                    const bool bDark = (bB & DARK_SQUARES) != 0;
                    return wDark == bDark; // same-color bishops → theoretical draw
                }
                // KBB vs K (or K vs KBB): NOT a theoretical draw.
                return false;
            }
            // 3+ bishops in total (promotions, etc.) → not a theoretical draw.
        }
    }
    // No queens; check "minor-only vs bare king" cases while also excluding rooks.
    else if (!wQ && !bQ) {
        const Bitboard wB = pos.get_pieceTypes_bitboard(WHITE, BISHOP);
        const Bitboard bB = pos.get_pieceTypes_bitboard(BLACK, BISHOP);
        const Bitboard wN = pos.get_pieceTypes_bitboard(WHITE, KNIGHT);
        const Bitboard bN = pos.get_pieceTypes_bitboard(BLACK, KNIGHT);

        // White has no rook/bishop/knight: black's remaining force < 2 minor pieces → draw.
        if (!wR && !wB && !wN) {
            const int bn = Bitboards::cpop(bN);
            const int bb = Bitboards::cpop(bB);
            return !bR && (bn + bb) < 2;
        }
        // Black has no rook/bishop/knight: white's remaining force < 2 minor pieces → draw.
        else if (!bR && !bB && !bN) {
            const int wn = Bitboards::cpop(wN);
            const int wb = Bitboards::cpop(wB);
            return !wR && (wn + wb) < 2;
        }
    }

    // Default: not a theoretical material draw.
    return false;
}


} // namespace Evaluate
} // namespace Xake
