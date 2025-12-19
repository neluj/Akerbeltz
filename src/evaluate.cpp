//#include "evaluate.h"
#include "position.h"
#include <algorithm>
#include <cmath>

namespace Xake {

using namespace Bitboards;
namespace Evaluate {

// ---------- Declaration----------
void init();
static inline int flip(int sq) { return sq ^ 56; }
static inline Score mg_eg_blend(double mg, double eg, int mgWeight, int egWeight);

// ---------- MG/EG Piece Values----------
constexpr double MG_PIECE_SCORES[PieceType::PIECETYPE_SIZE] = {0.0, 82.5, 337.5, 365.0, 477.5, 1025.0, 20000.0};
constexpr double EG_PIECE_SCORES[PieceType::PIECETYPE_SIZE] = {0.0, 94.0, 281.5, 297.5, 512.0, 936.5, 20000.0};

// Middle Game PSQT
static constexpr Score MG_PAWN_TABLE[SQ64_SIZE] = {
      0,   0,   0,   0,   0,   0,  0,   0,
    -35,  -1, -20, -23, -15,  24, 38, -22,
    -26,  -4,  -4, -10,   3,   3, 33, -12,
    -27,  -2,  -5,  12,  17,   6, 10, -25,
    -14,  13,   6,  21,  23,  12, 17, -23,
     -6,   7,  26,  31,  65,  56, 25, -20,
     98, 134,  61,  95,  68, 126, 34, -11,
      0,   0,   0,   0,   0,   0,  0,   0,
};

static constexpr Score MG_KNIGHT_TABLE[SQ64_SIZE] = {
    -105, -21, -58, -33, -17, -28, -19,  -23,
     -29, -53, -12,  -3,  -1,  18, -14,  -19,
     -23,  -9,  12,  10,  19,  17,  25,  -16,
     -13,   4,  16,  13,  28,  19,  21,   -8,
      -9,  17,  19,  53,  37,  69,  18,   22,
     -47,  60,  37,  65,  84, 129,  73,   44,
     -73, -41,  72,  36,  23,  62,   7,  -17,
    -167, -89, -34, -49,  61, -97, -15, -107,
};

static constexpr Score MG_BISHOP_TABLE[SQ64_SIZE] = {
    -33,  -3, -14, -21, -13, -12, -39, -21,
      4,  15,  16,   0,   7,  21,  33,   1,
      0,  15,  15,  15,  14,  27,  18,  10,
     -6,  13,  13,  26,  34,  12,  10,   4,
     -4,   5,  19,  50,  37,  37,   7,  -2,
    -16,  37,  43,  40,  35,  50,  37,  -2,
    -26,  16, -18, -13,  30,  59,  18, -47,
    -29,   4, -82, -37, -25, -42,   7,  -8,
};

static constexpr Score MG_ROOK_TABLE[SQ64_SIZE] = {
    -19, -13,   1,  17,  16,   7, -37, -26,
    -44, -16, -20,  -9,  -1,  11,  -6, -71,
    -45, -25, -16, -17,   3,   0,  -5, -33,
    -36, -26, -12,  -1,   9,  -7,   6, -23,
    -24, -11,   7,  26,  24,  35,  -8, -20,
     -5,  19,  26,  36,  17,  45,  61,  16,
     27,  32,  58,  62,  80,  67,  26,  44,
     32,  42,  32,  51,  63,   9,  31,  43,
};

static constexpr Score MG_QUEEN_TABLE[SQ64_SIZE] = {
     -1, -18,  -9,  10, -15, -25, -31, -50,
    -35,  -8,  11,   2,   8,  15,  -3,   1,
    -14,   2, -11,  -2,  -5,   2,  14,   5,
     -9, -26,  -9, -10,  -2,  -4,   3,  -3,
    -27, -27, -16, -16,  -1,  17,  -2,   1,
    -13, -17,   7,   8,  29,  56,  47,  57,
    -24, -39,  -5,   1, -16,  57,  28,  54,
    -28,   0,  29,  12,  59,  44,  43,  45,
};

static constexpr Score MG_KING_TABLE[SQ64_SIZE] = {
    -15,  36,  12, -54,   8, -28,  24,  14,
      1,   7,  -8, -64, -43, -16,   9,   8,
    -14, -14, -22, -46, -44, -30, -15, -27,
    -49,  -1, -27, -39, -46, -44, -33, -51,
    -17, -20, -12, -27, -30, -25, -14, -36,
     -9,  24,   2, -16, -20,   6,  22, -22,
     29,  -1, -20,  -7,  -8,  -4, -38, -29,
    -65,  23,  16, -15, -56, -34,   2,  13,
};

// End Game PSQT 
static constexpr Score EG_PAWN_TABLE[SQ64_SIZE] = {
      0,   0,   0,   0,   0,   0,   0,   0,
     13,   8,   8,  10,  13,   0,   2,  -7,
      4,   7,  -6,   1,   0,  -5,  -1,  -8,
     13,   9,  -3,  -7,  -7,  -8,   3,  -1,
     32,  24,  13,   5,  -2,   4,  17,  17,
     94, 100,  85,  67,  56,  53,  82,  84,
    178, 173, 158, 134, 147, 132, 165, 187,
      0,   0,   0,   0,   0,   0,   0,   0,
};

static constexpr Score EG_KNIGHT_TABLE[SQ64_SIZE] = {
    -29, -51, -23, -15, -22, -18, -50, -64,
    -42, -20, -10,  -5,  -2, -20, -23, -44,
    -23,  -3,  -1,  15,  10,  -3, -20, -22,
    -18,  -6,  16,  25,  16,  17,   4, -18,
    -17,   3,  22,  22,  22,  11,   8, -18,
    -24, -20,  10,   9,  -1,  -9, -19, -41,
    -25,  -8, -25,  -2,  -9, -25, -24, -52,
    -58, -38, -13, -28, -31, -27, -63, -99,
};

static constexpr Score EG_BISHOP_TABLE[SQ64_SIZE] = {
    -23,  -9, -23,  -5,  -9, -16,  -5, -17,
    -14, -18,  -7,  -1,   4,  -9, -15, -27,
    -12,  -3,   8,  10,  13,   3,  -7, -15,
     -6,   3,  13,  19,   7,  10,  -3,  -9,
     -3,   9,  12,   9,  14,  10,   3,   2,
      2,  -8,   0,  -1,  -2,   6,   0,   4,
     -8,  -4,   7, -12,  -3, -13,  -4, -14,
    -14, -21, -11,  -8,  -7,  -9, -17, -24,
};

static constexpr Score EG_ROOK_TABLE[SQ64_SIZE] = {
     -9,   2,   3,  -1,  -5, -13,   4, -20,
     -6,  -6,   0,   2,  -9,  -9, -11,  -3,
     -4,   0,  -5,  -1,  -7, -12,  -8, -16,
      3,   5,   8,   4,  -5,  -6,  -8, -11,
      4,   3,  13,   1,   2,   1,  -1,   2,
      7,   7,   7,   5,   4,  -3,  -5,  -3,
     11,  13,  13,  11,  -3,   3,   8,   3,
     13,  10,  18,  15,  12,  12,   8,   5,
};

static constexpr Score EG_QUEEN_TABLE[SQ64_SIZE] = {
    -33, -28, -22, -43,  -5, -32, -20, -41,
    -22, -23, -30, -16, -16, -23, -36, -32,
    -16, -27,  15,   6,   9,  17,  10,   5,
    -18,  28,  19,  47,  31,  34,  39,  23,
      3,  22,  24,  45,  57,  40,  57,  36,
    -20,   6,   9,  49,  47,  35,  19,   9,
    -17,  20,  32,  41,  58,  25,  30,   0,
     -9,  22,  22,  27,  27,  19,  10,  20,
};

static constexpr Score EG_KING_TABLE[SQ64_SIZE] = {
    -53, -34, -21, -11, -28, -14, -24, -43,
    -27, -11,   4,  13,  14,   4,  -5, -17,
    -19,  -3,  11,  21,  23,  16,   7,  -9,
    -18,  -4,  21,  24,  27,  23,   9, -11,
     -8,  22,  24,  27,  26,  33,  26,   3,
     10,  17,  23,  15,  20,  45,  44,  13,
    -12,  17,  14,  17,  17,  38,  23,  11,
    -74, -35, -18, -18, -11,  15,   4, -17,
};

constexpr Score MG_TEMPO_BONUS = 8;
constexpr Score EG_TEMPO_BONUS = 4;

// ---------- Internal State ----------
static Score PST_MG[PIECE_SIZE][SQ64_SIZE];
static Score PST_EG[PIECE_SIZE][SQ64_SIZE];

// ---------- INIT ----------
void init() {
    // Clean PST
    for (int p = 0; p < PIECE_SIZE; ++p) {
        for (int s = 0; s < SQ64_SIZE; ++s) {
            PST_MG[p][s] = PST_EG[p][s] = 0;
        }
    }

    // White
    for (int s = 0; s < SQ64_SIZE; ++s) {
        PST_MG[Piece::NO_PIECE][s] = 0;
        PST_EG[Piece::NO_PIECE][s] = 0;
        PST_MG[Piece::W_PAWN][s]   = MG_PAWN_TABLE[s];
        PST_EG[Piece::W_PAWN][s]   = EG_PAWN_TABLE[s];
        PST_MG[Piece::W_KNIGHT][s] = MG_KNIGHT_TABLE[s];
        PST_EG[Piece::W_KNIGHT][s] = EG_KNIGHT_TABLE[s];
        PST_MG[Piece::W_BISHOP][s] = MG_BISHOP_TABLE[s];
        PST_EG[Piece::W_BISHOP][s] = EG_BISHOP_TABLE[s];
        PST_MG[Piece::W_ROOK][s]   = MG_ROOK_TABLE[s];
        PST_EG[Piece::W_ROOK][s]   = EG_ROOK_TABLE[s];
        PST_MG[Piece::W_QUEEN][s]  = MG_QUEEN_TABLE[s];
        PST_EG[Piece::W_QUEEN][s]  = EG_QUEEN_TABLE[s];
        PST_MG[Piece::W_KING][s]   = MG_KING_TABLE[s];
        PST_EG[Piece::W_KING][s]   = EG_KING_TABLE[s];
    }

    // Black
    for (int s = 0; s < SQ64_SIZE; ++s) {
        const int fs = flip(s);
        PST_MG[Piece::B_PAWN][s]   = MG_PAWN_TABLE[fs];
        PST_EG[Piece::B_PAWN][s]   = EG_PAWN_TABLE[fs];
        PST_MG[Piece::B_KNIGHT][s] = MG_KNIGHT_TABLE[fs];
        PST_EG[Piece::B_KNIGHT][s] = EG_KNIGHT_TABLE[fs];
        PST_MG[Piece::B_BISHOP][s] = MG_BISHOP_TABLE[fs];
        PST_EG[Piece::B_BISHOP][s] = EG_BISHOP_TABLE[fs];
        PST_MG[Piece::B_ROOK][s]   = MG_ROOK_TABLE[fs];
        PST_EG[Piece::B_ROOK][s]   = EG_ROOK_TABLE[fs];
        PST_MG[Piece::B_QUEEN][s]  = MG_QUEEN_TABLE[fs];
        PST_EG[Piece::B_QUEEN][s]  = EG_QUEEN_TABLE[fs];
        PST_MG[Piece::B_KING][s]   = MG_KING_TABLE[fs];
        PST_EG[Piece::B_KING][s]   = EG_KING_TABLE[fs];
    }

}

// ---------- Main Eval ----------
Score calc_score(const Position &position) {
    double mg[COLOR_SIZE] = {0.0, 0.0};
    double eg[COLOR_SIZE] = {0.0, 0.0};
    GamePhaseWeight phase = 0;

    // Pawn
    Bitboard wp = position.get_pieceTypes_bitboard(WHITE, PAWN);
    while (wp) {
        const Square64 s{ Bitboards::ctz(wp) };
        mg[WHITE] += MG_PIECE_SCORES[PieceType::PAWN] + PST_MG[Piece::W_PAWN][s];
        eg[WHITE] += EG_PIECE_SCORES[PieceType::PAWN] + PST_EG[Piece::W_PAWN][s];
        phase += PHASE_PIECE_WEIGHT[Piece::W_PAWN];
        wp &= wp - 1;
    }

    Bitboard bp = position.get_pieceTypes_bitboard(BLACK, PAWN);
    while (bp) {
        const Square64 s{ Bitboards::ctz(bp) };
        mg[BLACK] += MG_PIECE_SCORES[PieceType::PAWN] + PST_MG[Piece::B_PAWN][s];
        eg[BLACK] += EG_PIECE_SCORES[PieceType::PAWN] + PST_EG[Piece::B_PAWN][s];
        phase += PHASE_PIECE_WEIGHT[Piece::B_PAWN];
        bp &= bp - 1;
    }

    // Knight
    Bitboard wn = position.get_pieceTypes_bitboard(WHITE, KNIGHT);
    while (wn) {
        const Square64 s{ Bitboards::ctz(wn) };
        mg[WHITE] += MG_PIECE_SCORES[PieceType::KNIGHT] + PST_MG[Piece::W_KNIGHT][s];
        eg[WHITE] += EG_PIECE_SCORES[PieceType::KNIGHT] + PST_EG[Piece::W_KNIGHT][s];
        phase += PHASE_PIECE_WEIGHT[Piece::W_KNIGHT];
        wn &= wn - 1;
    }

    Bitboard bn = position.get_pieceTypes_bitboard(BLACK, KNIGHT);
    while (bn) {
        const Square64 s{ Bitboards::ctz(bn) };
        mg[BLACK] += MG_PIECE_SCORES[PieceType::KNIGHT] + PST_MG[Piece::B_KNIGHT][s];
        eg[BLACK] += EG_PIECE_SCORES[PieceType::KNIGHT] + PST_EG[Piece::B_KNIGHT][s];
        phase += PHASE_PIECE_WEIGHT[Piece::B_KNIGHT];
        bn &= bn - 1;
    }

    // Bishop
    Bitboard wb = position.get_pieceTypes_bitboard(WHITE, BISHOP);
    while (wb) {
        const Square64 s{ Bitboards::ctz(wb) };
        mg[WHITE] += MG_PIECE_SCORES[PieceType::BISHOP] + PST_MG[Piece::W_BISHOP][s];
        eg[WHITE] += EG_PIECE_SCORES[PieceType::BISHOP] + PST_EG[Piece::W_BISHOP][s];
        phase += PHASE_PIECE_WEIGHT[Piece::W_BISHOP];
        wb &= wb - 1;
    }

    Bitboard bb = position.get_pieceTypes_bitboard(BLACK, BISHOP);
    while (bb) {
        const Square64 s{ Bitboards::ctz(bb) };
        mg[BLACK] += MG_PIECE_SCORES[PieceType::BISHOP] + PST_MG[Piece::B_BISHOP][s];
        eg[BLACK] += EG_PIECE_SCORES[PieceType::BISHOP] + PST_EG[Piece::B_BISHOP][s];
        phase += PHASE_PIECE_WEIGHT[Piece::B_BISHOP];
        bb &= bb - 1;
    }

    // Rook
    Bitboard wr = position.get_pieceTypes_bitboard(WHITE, ROOK);
    while (wr) {
        const Square64 s{ Bitboards::ctz(wr) };
        mg[WHITE] += MG_PIECE_SCORES[PieceType::ROOK] + PST_MG[Piece::W_ROOK][s];
        eg[WHITE] += EG_PIECE_SCORES[PieceType::ROOK] + PST_EG[Piece::W_ROOK][s];
        phase += PHASE_PIECE_WEIGHT[Piece::W_ROOK];
        wr &= wr - 1;
    }

    Bitboard br = position.get_pieceTypes_bitboard(BLACK, ROOK);
    while (br) {
        const Square64 s{ Bitboards::ctz(br) };
        mg[BLACK] += MG_PIECE_SCORES[PieceType::ROOK] + PST_MG[Piece::B_ROOK][s];
        eg[BLACK] += EG_PIECE_SCORES[PieceType::ROOK] + PST_EG[Piece::B_ROOK][s];
        phase += PHASE_PIECE_WEIGHT[Piece::B_ROOK];
        br &= br - 1;
    }

    // Queen
    Bitboard wq = position.get_pieceTypes_bitboard(WHITE, QUEEN);
    while (wq) {
        const Square64 s{ Bitboards::ctz(wq) };
        mg[WHITE] += MG_PIECE_SCORES[PieceType::QUEEN] + PST_MG[Piece::W_QUEEN][s];
        eg[WHITE] += EG_PIECE_SCORES[PieceType::QUEEN] + PST_EG[Piece::W_QUEEN][s];
        phase += PHASE_PIECE_WEIGHT[Piece::W_QUEEN];
        wq &= wq - 1;
    }

    Bitboard bq = position.get_pieceTypes_bitboard(BLACK, QUEEN);
    while (bq) {
        const Square64 s{ Bitboards::ctz(bq) };
        mg[BLACK] += MG_PIECE_SCORES[PieceType::QUEEN] + PST_MG[Piece::B_QUEEN][s];
        eg[BLACK] += EG_PIECE_SCORES[PieceType::QUEEN] + PST_EG[Piece::B_QUEEN][s];
        phase += PHASE_PIECE_WEIGHT[Piece::B_QUEEN];
        bq &= bq - 1;
    }

    // Kings
    Bitboard wk = position.get_pieceTypes_bitboard(WHITE, KING);
    while (wk) {
        const Square64 s{ Bitboards::ctz(wk) };
        mg[WHITE] += MG_PIECE_SCORES[PieceType::KING] + PST_MG[Piece::W_KING][s];
        eg[WHITE] += EG_PIECE_SCORES[PieceType::KING] + PST_EG[Piece::W_KING][s];
        wk &= wk - 1;
    }

    Bitboard bk = position.get_pieceTypes_bitboard(BLACK, KING);
    while (bk) {
        const Square64 s{ Bitboards::ctz(bk) };
        mg[BLACK] += MG_PIECE_SCORES[PieceType::KING] + PST_MG[Piece::B_KING][s];
        eg[BLACK] += EG_PIECE_SCORES[PieceType::KING] + PST_EG[Piece::B_KING][s];
        bk &= bk - 1;
    }

    // Tempo
    const Color stm = position.get_side_to_move();
    mg[stm] += MG_TEMPO_BONUS;
    eg[stm] += EG_TEMPO_BONUS;

    const GamePhaseWeight mgWeight = std::min<GamePhaseWeight>(phase, MAX_PHASE_PIECE_WEIGHT);
    const GamePhaseWeight egWeight = MAX_PHASE_PIECE_WEIGHT - mgWeight;

    const double mgScore = mg[stm] - mg[~stm];
    const double egScore = eg[stm] - eg[~stm];

    return mg_eg_blend(mgScore, egScore, mgWeight, egWeight);
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

static inline Score mg_eg_blend(double mg, double eg, int mgWeight, int egWeight) {
    const double blended = (mg * mgWeight + eg * egWeight) / MAX_PHASE_PIECE_WEIGHT;
    return static_cast<Score>(std::lround(blended));
}

} // namespace Evaluate
} // namespace Xake
