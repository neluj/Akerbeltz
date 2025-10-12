#include <algorithm>
#include <array>
#include <cctype>
#include <cstdlib>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

#include "evaluate.h"
#include "position.h"

using namespace Xake;

namespace Xake::Evaluate {
    Score eval_open_files(const Position& pos);
}

namespace {
constexpr Evaluate::GamePhaseWeight MAX_PHASE_PIECE_WEIGHT = 24;
constexpr Evaluate::Score MG_PIECE_SCORES[PieceType::PIECETYPE_SIZE] = {0, 82, 337, 365, 477, 1025, 20000};
constexpr Evaluate::Score EG_PIECE_SCORES[PieceType::PIECETYPE_SIZE] = {0, 94, 281, 297, 512, 936, 20000};
constexpr Evaluate::Score MG_ISOLATED_PAWN_PENALTY = -8;
constexpr Evaluate::Score EG_ISOLATED_PAWN_PENALTY = -18;
constexpr Evaluate::Score MG_PASSED_PAWN_BONUS_BY_RANK[RANK_SIZE] = {
    0, 0, 5, 12, 25, 45, 85, 0
};
constexpr Evaluate::Score EG_PASSED_PAWN_BONUS_BY_RANK[RANK_SIZE] = {
    0, 0, 10, 25, 50, 90, 150, 0
};

constexpr Evaluate::GamePhaseWeight PHASE_PIECE_WEIGHT[Piece::PIECE_SIZE] = {
    0, // NO_PIECE
    0, // W_PAWN
    1, // W_KNIGHT
    1, // W_BISHOP
    2, // W_ROOK
    4, // W_QUEEN
    0, // W_KING
    0,
    0,
    0, // B_PAWN
    1, // B_KNIGHT
    1, // B_BISHOP
    2, // B_ROOK
    4, // B_QUEEN
    0, // B_KING
    0
};

constexpr Evaluate::Score MG_PAWN_TABLE[SQ64_SIZE] = {
      0,   0,   0,   0,   0,   0,  0,   0,
    -35,  -1, -20, -23, -15,  24, 38, -22,
    -26,  -4,  -4, -10,   3,   3, 33, -12,
    -27,  -2,  -5,  12,  17,   6, 10, -25,
    -14,  13,   6,  21,  23,  12, 17, -23,
     -6,   7,  26,  31,  65,  56, 25, -20,
     98, 134,  61,  95,  68, 126, 34, -11,
      0,   0,   0,   0,   0,   0,  0,   0,
};

constexpr Evaluate::Score EG_PAWN_TABLE[SQ64_SIZE] = {
      0,   0,   0,   0,   0,   0,   0,   0,
     13,   8,   8,  10,  13,   0,   2,  -7,
      4,   7,  -6,   1,   0,  -5,  -1,  -8,
     13,   9,  -3,  -7,  -7,  -8,   3,  -1,
     32,  24,  13,   5,  -2,   4,  17,  17,
     94, 100,  85,  67,  56,  53,  82,  84,
    178, 173, 158, 134, 147, 132, 165, 187,
      0,   0,   0,   0,   0,   0,   0,   0,
};

constexpr Evaluate::Score MG_KNIGHT_TABLE[SQ64_SIZE] = {
    -105, -21, -58, -33, -17, -28, -19,  -23,
     -29, -53, -12,  -3,  -1,  18, -14,  -19,
     -23,  -9,  12,  10,  19,  17,  25,  -16,
     -13,   4,  16,  13,  28,  19,  21,   -8,
      -9,  17,  19,  53,  37,  69,  18,   22,
     -47,  60,  37,  65,  84, 129,  73,   44,
     -73, -41,  72,  36,  23,  62,   7,  -17,
    -167, -89, -34, -49,  61, -97, -15, -107,
};

constexpr Evaluate::Score EG_KNIGHT_TABLE[SQ64_SIZE] = {
    -29, -51, -23, -15, -22, -18, -50, -64,
    -42, -20, -10,  -5,  -2, -20, -23, -44,
    -23,  -3,  -1,  15,  10,  -3, -20, -22,
    -18,  -6,  16,  25,  16,  17,   4, -18,
    -17,   3,  22,  22,  22,  11,   8, -18,
    -24, -20,  10,   9,  -1,  -9, -19, -41,
    -25,  -8, -25,  -2,  -9, -25, -24, -52,
    -58, -38, -13, -28, -31, -27, -63, -99,
};

constexpr Evaluate::Score MG_BISHOP_TABLE[SQ64_SIZE] = {
    -33,  -3, -14, -21, -13, -12, -39, -21,
      4,  15,  16,   0,   7,  21,  33,   1,
      0,  15,  15,  15,  14,  27,  18,  10,
     -6,  13,  13,  26,  34,  12,  10,   4,
     -4,   5,  19,  50,  37,  37,   7,  -2,
    -16,  37,  43,  40,  35,  50,  37,  -2,
    -26,  16, -18, -13,  30,  59,  18, -47,
    -29,   4, -82, -37, -25, -42,   7,  -8,
};

constexpr Evaluate::Score EG_BISHOP_TABLE[SQ64_SIZE] = {
    -23,  -9, -23,  -5,  -9, -16,  -5, -17,
    -14, -18,  -7,  -1,   4,  -9, -15, -27,
    -12,  -3,   8,  10,  13,   3,  -7, -15,
     -6,   3,  13,  19,   7,  10,  -3,  -9,
     -3,   9,  12,   9,  14,  10,   3,   2,
      2,  -8,   0,  -1,  -2,   6,   0,   4,
     -8,  -4,   7, -12,  -3, -13,  -4, -14,
    -14, -21, -11,  -8,  -7,  -9, -17, -24,
};

constexpr Evaluate::Score MG_ROOK_TABLE[SQ64_SIZE] = {
    -19, -13,   1,  17,  16,   7, -37, -26,
    -44, -16, -20,  -9,  -1,  11,  -6, -71,
    -45, -25, -16, -17,   3,   0,  -5, -33,
    -36, -26, -12,  -1,   9,  -7,   6, -23,
    -24, -11,   7,  26,  24,  35,  -8, -20,
     -5,  19,  26,  36,  17,  45,  61,  16,
     27,  32,  58,  62,  80,  67,  26,  44,
     32,  42,  32,  51,  63,   9,  31,  43,
};

constexpr Evaluate::Score EG_ROOK_TABLE[SQ64_SIZE] = {
     -9,   2,   3,  -1,  -5, -13,   4, -20,
     -6,  -6,   0,   2,  -9,  -9, -11,  -3,
     -4,   0,  -5,  -1,  -7, -12,  -8, -16,
      3,   5,   8,   4,  -5,  -6,  -8, -11,
      4,   3,  13,   1,   2,   1,  -1,   2,
      7,   7,   7,   5,   4,  -3,  -5,  -3,
     11,  13,  13,  11,  -3,   3,   8,   3,
     13,  10,  18,  15,  12,  12,   8,   5,
};

constexpr Evaluate::Score MG_QUEEN_TABLE[SQ64_SIZE] = {
     -1, -18,  -9,  10, -15, -25, -31, -50,
    -35,  -8,  11,   2,   8,  15,  -3,   1,
    -14,   2, -11,  -2,  -5,   2,  14,   5,
     -9, -26,  -9, -10,  -2,  -4,   3,  -3,
    -27, -27, -16, -16,  -1,  17,  -2,   1,
    -13, -17,   7,   8,  29,  56,  47,  57,
    -24, -39,  -5,   1, -16,  57,  28,  54,
    -28,   0,  29,  12,  59,  44,  43,  45,
};

constexpr Evaluate::Score EG_QUEEN_TABLE[SQ64_SIZE] = {
    -33, -28, -22, -43,  -5, -32, -20, -41,
    -22, -23, -30, -16, -16, -23, -36, -32,
    -16, -27,  15,   6,   9,  17,  10,   5,
    -18,  28,  19,  47,  31,  34,  39,  23,
      3,  22,  24,  45,  57,  40,  57,  36,
    -20,   6,   9,  49,  47,  35,  19,   9,
    -17,  20,  32,  41,  58,  25,  30,   0,
     -9,  22,  22,  27,  27,  19,  10,  20,
};

constexpr Evaluate::Score MG_KING_TABLE[SQ64_SIZE] = {
    -15,  36,  12, -54,   8, -28,  24,  14,
      1,   7,  -8, -64, -43, -16,   9,   8,
    -14, -14, -22, -46, -44, -30, -15, -27,
    -49,  -1, -27, -39, -46, -44, -33, -51,
    -17, -20, -12, -27, -30, -25, -14, -36,
     -9,  24,   2, -16, -20,   6,  22, -22,
     29,  -1, -20,  -7,  -8,  -4, -38, -29,
    -65,  23,  16, -15, -56, -34,   2,  13,
};

constexpr Evaluate::Score EG_KING_TABLE[SQ64_SIZE] = {
    -53, -34, -21, -11, -28, -14, -24, -43,
    -27, -11,   4,  13,  14,   4,  -5, -17,
    -19,  -3,  11,  21,  23,  16,   7,  -9,
    -18,  -4,  21,  24,  27,  23,   9, -11,
     -8,  22,  24,  27,  26,  33,  26,   3,
     10,  17,  23,  15,  20,  45,  44,  13,
    -12,  17,  14,  17,  17,  38,  23,  11,
    -74, -35, -18, -18, -11,  15,   4, -17,
};

constexpr int flipIndex(int squareIndex) {
    return squareIndex ^ 56;
}

constexpr Bitboard FILE_A_MASK = 0x0101010101010101ULL;
constexpr Bitboard FULL_BITBOARD = ~Bitboard{0};

constexpr Bitboard isolatedMask(Square64 square) {
    const int file = static_cast<int>(square) & 7;
    Bitboard mask = 0;
    if (file > 0) {
        mask |= FILE_A_MASK << (file - 1);
    }
    if (file < 7) {
        mask |= FILE_A_MASK << (file + 1);
    }
    return mask;
}

constexpr Bitboard passedMask(Color color, Square64 square) {
    const int file = static_cast<int>(square) & 7;
    Bitboard files = FILE_A_MASK << file;
    if (file > 0) {
        files |= FILE_A_MASK << (file - 1);
    }
    if (file < 7) {
        files |= FILE_A_MASK << (file + 1);
    }

    const int rank = static_cast<int>(square) >> 3;
    if (color == Color::WHITE) {
        if (rank == 7) {
            return 0;
        }
        return files & (FULL_BITBOARD << ((rank + 1) * 8));
    }

    if (rank == 0) {
        return 0;
    }
    const int shift = rank * 8;
    return files & ((Bitboard{1} << shift) - 1);
}

static_assert(passedMask(Color::WHITE, Square64::SQ64_C4) != 0,
              "Expected passed mask for white pawn on c4 to be non-zero");

Evaluate::Score mgPieceValue(Piece piece, Square64 square) {
    const int sq = static_cast<int>(square);
    switch (piece) {
    case Piece::W_PAWN:
        return MG_PIECE_SCORES[PieceType::PAWN] + MG_PAWN_TABLE[sq];
    case Piece::B_PAWN:
        return MG_PIECE_SCORES[PieceType::PAWN] + MG_PAWN_TABLE[flipIndex(sq)];
    case Piece::W_BISHOP:
        return MG_PIECE_SCORES[PieceType::BISHOP] + MG_BISHOP_TABLE[sq];
    case Piece::B_BISHOP:
        return MG_PIECE_SCORES[PieceType::BISHOP] + MG_BISHOP_TABLE[flipIndex(sq)];
    case Piece::W_KNIGHT:
        return MG_PIECE_SCORES[PieceType::KNIGHT] + MG_KNIGHT_TABLE[sq];
    case Piece::B_KNIGHT:
        return MG_PIECE_SCORES[PieceType::KNIGHT] + MG_KNIGHT_TABLE[flipIndex(sq)];
    case Piece::W_ROOK:
        return MG_PIECE_SCORES[PieceType::ROOK] + MG_ROOK_TABLE[sq];
    case Piece::B_ROOK:
        return MG_PIECE_SCORES[PieceType::ROOK] + MG_ROOK_TABLE[flipIndex(sq)];
    case Piece::W_QUEEN:
        return MG_PIECE_SCORES[PieceType::QUEEN] + MG_QUEEN_TABLE[sq];
    case Piece::B_QUEEN:
        return MG_PIECE_SCORES[PieceType::QUEEN] + MG_QUEEN_TABLE[flipIndex(sq)];
    case Piece::W_KING:
        return MG_PIECE_SCORES[PieceType::KING] + MG_KING_TABLE[sq];
    case Piece::B_KING:
        return MG_PIECE_SCORES[PieceType::KING] + MG_KING_TABLE[flipIndex(sq)];
    default:
        return 0;
    }
}

Evaluate::Score egPieceValue(Piece piece, Square64 square) {
    const int sq = static_cast<int>(square);
    switch (piece) {
    case Piece::W_PAWN:
        return EG_PIECE_SCORES[PieceType::PAWN] + EG_PAWN_TABLE[sq];
    case Piece::B_PAWN:
        return EG_PIECE_SCORES[PieceType::PAWN] + EG_PAWN_TABLE[flipIndex(sq)];
    case Piece::W_BISHOP:
        return EG_PIECE_SCORES[PieceType::BISHOP] + EG_BISHOP_TABLE[sq];
    case Piece::B_BISHOP:
        return EG_PIECE_SCORES[PieceType::BISHOP] + EG_BISHOP_TABLE[flipIndex(sq)];
    case Piece::W_KNIGHT:
        return EG_PIECE_SCORES[PieceType::KNIGHT] + EG_KNIGHT_TABLE[sq];
    case Piece::B_KNIGHT:
        return EG_PIECE_SCORES[PieceType::KNIGHT] + EG_KNIGHT_TABLE[flipIndex(sq)];
    case Piece::W_ROOK:
        return EG_PIECE_SCORES[PieceType::ROOK] + EG_ROOK_TABLE[sq];
    case Piece::B_ROOK:
        return EG_PIECE_SCORES[PieceType::ROOK] + EG_ROOK_TABLE[flipIndex(sq)];
    case Piece::W_QUEEN:
        return EG_PIECE_SCORES[PieceType::QUEEN] + EG_QUEEN_TABLE[sq];
    case Piece::B_QUEEN:
        return EG_PIECE_SCORES[PieceType::QUEEN] + EG_QUEEN_TABLE[flipIndex(sq)];
    case Piece::W_KING:
        return EG_PIECE_SCORES[PieceType::KING] + EG_KING_TABLE[sq];
    case Piece::B_KING:
        return EG_PIECE_SCORES[PieceType::KING] + EG_KING_TABLE[flipIndex(sq)];
    default:
        return 0;
    }
}

Evaluate::GamePhaseWeight manualGamePhase(const Position& pos) {
    Evaluate::GamePhaseWeight accumulated = 0;
    for (Color color : {Color::WHITE, Color::BLACK}) {
        for (int pt = PieceType::PAWN; pt <= PieceType::KING; ++pt) {
            const auto pieceType = static_cast<PieceType>(pt);
            const Bitboard bitboard = pos.get_pieceTypes_bitboard(color, pieceType);
            const Evaluate::GamePhaseWeight count = static_cast<Evaluate::GamePhaseWeight>(Bitboards::cpop(bitboard));
            const Piece piece = make_piece(color, pieceType);
            accumulated += count * PHASE_PIECE_WEIGHT[piece];
        }
    }
    return accumulated;
}

struct ManualScoreBreakdown {
    Evaluate::Score mgTotals[COLOR_SIZE];
    Evaluate::Score egTotals[COLOR_SIZE];
    Evaluate::Score mgScore;
    Evaluate::Score egScore;
    Evaluate::GamePhaseWeight phase;
    Evaluate::GamePhaseWeight mgWeight;
    Evaluate::GamePhaseWeight egWeight;
    Evaluate::Score blended;
};

ManualScoreBreakdown manualScoreBreakdown(const Position& pos) {
    ManualScoreBreakdown breakdown{};
    Evaluate::Score mgTotals[COLOR_SIZE] = {0};
    Evaluate::Score egTotals[COLOR_SIZE] = {0};

    const auto accumulatePawns = [&](Color color, Bitboard pawns, Bitboard opponents) {
        Bitboard it = pawns;
        while (it) {
            const Square64 square = Square64(Bitboards::ctz(it));
            const Piece piece = make_piece(color, PieceType::PAWN);
            mgTotals[color] += mgPieceValue(piece, square);
            egTotals[color] += egPieceValue(piece, square);
            if ((pawns & isolatedMask(square)) == 0) {
                mgTotals[color] += MG_ISOLATED_PAWN_PENALTY;
                egTotals[color] += EG_ISOLATED_PAWN_PENALTY;
            }
            if ((passedMask(color, square) & opponents) == 0) {
                const int rank = static_cast<int>(square_rank(square));
                if (color == Color::WHITE) {
                    mgTotals[color] += MG_PASSED_PAWN_BONUS_BY_RANK[rank];
                    egTotals[color] += EG_PASSED_PAWN_BONUS_BY_RANK[rank];
                } else {
                    mgTotals[color] += MG_PASSED_PAWN_BONUS_BY_RANK[7 - rank];
                    egTotals[color] += EG_PASSED_PAWN_BONUS_BY_RANK[7 - rank];
                }
            }
            it &= it - 1;
        }
    };

    const auto accumulate = [&](Color color, PieceType pieceType) {
        Bitboard bitboard = pos.get_pieceTypes_bitboard(color, pieceType);
        while (bitboard) {
            const Square64 square = Square64(Bitboards::ctz(bitboard));
            const Piece piece = make_piece(color, pieceType);
            mgTotals[color] += mgPieceValue(piece, square);
            egTotals[color] += egPieceValue(piece, square);
            bitboard &= bitboard - 1;
        }
    };

    const Bitboard whitePawns = pos.get_pieceTypes_bitboard(Color::WHITE, PieceType::PAWN);
    const Bitboard blackPawns = pos.get_pieceTypes_bitboard(Color::BLACK, PieceType::PAWN);

    accumulatePawns(Color::WHITE, whitePawns, blackPawns);
    accumulatePawns(Color::BLACK, blackPawns, whitePawns);

    for (PieceType pieceType : {PieceType::KING, PieceType::KNIGHT, PieceType::BISHOP,
                                 PieceType::ROOK, PieceType::QUEEN}) {
        accumulate(Color::WHITE, pieceType);
        accumulate(Color::BLACK, pieceType);
    }

    for (Color color : {Color::WHITE, Color::BLACK}) {
        breakdown.mgTotals[color] = mgTotals[color];
        breakdown.egTotals[color] = egTotals[color];
    }

    breakdown.phase = manualGamePhase(pos);
    breakdown.mgWeight = std::min(breakdown.phase, MAX_PHASE_PIECE_WEIGHT);
    breakdown.egWeight = MAX_PHASE_PIECE_WEIGHT - breakdown.mgWeight;

    const Color sideToMove = pos.get_side_to_move();
    breakdown.mgScore = mgTotals[sideToMove] - mgTotals[~sideToMove];
    breakdown.egScore = egTotals[sideToMove] - egTotals[~sideToMove];

    breakdown.blended = ((breakdown.mgScore * breakdown.mgWeight) +
                         (breakdown.egScore * breakdown.egWeight)) /
                        MAX_PHASE_PIECE_WEIGHT;

    return breakdown;
}

Evaluate::Score manualCalcScore(const Position& pos) {
    return manualScoreBreakdown(pos).blended;
}

Evaluate::Score accumulateMgBase(const Position& pos, Color color) {
    Evaluate::Score total = 0;
    for (int pt = PieceType::PAWN; pt <= PieceType::KING; ++pt) {
        const auto pieceType = static_cast<PieceType>(pt);
        Bitboard bitboard = pos.get_pieceTypes_bitboard(color, pieceType);
        while (bitboard) {
            const Square64 square = Square64(Bitboards::ctz(bitboard));
            total += mgPieceValue(make_piece(color, pieceType), square);
            bitboard &= bitboard - 1;
        }
    }
    return total;
}

Evaluate::Score accumulateEgBase(const Position& pos, Color color) {
    Evaluate::Score total = 0;
    for (int pt = PieceType::PAWN; pt <= PieceType::KING; ++pt) {
        const auto pieceType = static_cast<PieceType>(pt);
        Bitboard bitboard = pos.get_pieceTypes_bitboard(color, pieceType);
        while (bitboard) {
            const Square64 square = Square64(Bitboards::ctz(bitboard));
            total += egPieceValue(make_piece(color, pieceType), square);
            bitboard &= bitboard - 1;
        }
    }
    return total;
}


constexpr const char* START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
constexpr const char* WHITE_ADVANTAGE_WHITE_TO_MOVE = "4k3/8/8/8/4Q3/8/8/4K3 w - - 0 1";
constexpr const char* WHITE_ADVANTAGE_BLACK_TO_MOVE = "4k3/8/8/8/4Q3/8/8/4K3 b - - 0 1";
constexpr const char* ISOLATED_PAWNS_FEN = "4k3/8/8/8/2P5/8/8/P3K3 w - - 0 1";
constexpr const char* SUPPORTED_PAWNS_FEN = "4k3/8/8/8/2P5/8/8/1P2K3 w - - 0 1";
constexpr const char* BARE_KINGS_FEN = "4k3/8/8/8/8/8/8/4K3 w - - 0 1";
constexpr const char* KBK_FEN = "4k3/8/8/8/8/8/8/3BK3 w - - 0 1";
constexpr const char* KNK_FEN = "4k3/8/8/8/8/8/8/3NK3 w - - 0 1";
constexpr const char* KNNK_FEN = "4k3/8/8/8/8/8/8/2NNK3 w - - 0 1";
constexpr const char* KBKB_SAME_COLOR_FEN = "4kb2/8/8/8/8/8/8/2B1K3 w - - 0 1";
constexpr const char* KBNK_FEN = "4k3/8/8/8/8/8/8/2B1K1N1 w - - 0 1";
constexpr const char* KRK_FEN = "4k3/8/8/8/8/8/8/3RK3 w - - 0 1";
constexpr const char* PASSED_PAWN_MG_FEN = "rnbqkbnr/p4ppp/8/3P4/8/8/PPP2PPP/RNBQKBNR w KQkq - 0 1";
constexpr const char* BLOCKED_PAWN_MG_FEN = "rnbqkbnr/5ppp/4p3/3P4/8/8/PPP2PPP/RNBQKBNR w KQkq - 0 1";
constexpr const char* PASSED_PAWN_EG_FEN = "4k3/p5p1/8/3P4/8/8/P6P/4K3 w - - 0 1";
constexpr const char* BLOCKED_PAWN_EG_FEN = "4k3/6p1/4p3/3P4/8/8/P6P/4K3 w - - 0 1";
constexpr const char* BISHOP_CENTER_MG_FEN = "rnbqkbnr/pppppppp/8/8/4B3/8/PPPPPPPP/RN1QKBNR w KQkq - 0 1";
constexpr const char* BISHOP_CORNER_MG_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/BNRQKBNR w KQkq - 0 1";
constexpr const char* BISHOP_CENTER_EG_FEN = "4k3/8/8/8/4B3/8/8/4K3 w - - 0 1";
constexpr const char* BISHOP_CORNER_EG_FEN = "4k3/8/8/8/8/8/8/B3K3 w - - 0 1";
constexpr const char* ROOK_CENTER_MG_FEN = "rnbqkbnr/pppppppp/8/8/3R4/8/PPPPPPPP/RNBQKBN1 w KQkq - 0 1";
constexpr const char* ROOK_CORNER_MG_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
constexpr const char* ROOK_CENTER_EG_FEN = "4k3/8/8/8/3R4/8/8/4K3 w - - 0 1";
constexpr const char* ROOK_CORNER_EG_FEN = "4k3/8/8/8/8/8/8/R3K3 w - - 0 1";
constexpr const char* QUEEN_CENTER_MG_FEN = "rnbqkbnr/pppppppp/5Q2/8/8/8/PPPPPPPP/1RNBKBNR w KQkq - 0 1";
constexpr const char* QUEEN_CORNER_MG_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/QRNBKBNR w KQkq - 0 1";
constexpr const char* QUEEN_CENTER_EG_FEN = "4k3/8/5Q2/8/8/8/8/4K3 w - - 0 1";
constexpr const char* QUEEN_CORNER_EG_FEN = "4k3/8/8/8/8/8/8/Q3K3 w - - 0 1";
constexpr const char* ISOLATED_FILE_A_FEN = "4k3/8/8/8/8/8/P7/4K3 w - - 0 1";
constexpr const char* SUPPORTED_FILE_A_FEN = "4k3/8/8/8/8/8/PP6/4K3 w - - 0 1";
constexpr const char* WHITE_PASSED_ON_SEVENTH_FEN = "4k3/P7/8/8/8/8/8/4K3 w - - 0 1";
constexpr const char* WHITE_BLOCKED_ON_SEVENTH_FEN = "1p2k3/P7/8/8/8/8/8/4K3 w - - 0 1";
constexpr const char* BLACK_PASSED_ON_SECOND_FEN = "4k3/8/8/8/8/8/p7/4K3 b - - 0 1";
constexpr const char* BLACK_BLOCKED_ON_SECOND_FEN = "4k3/8/8/8/8/8/p7/1P2K3 b - - 0 1";
constexpr const char* INTERMEDIATE_PHASE_FEN = "4k2r/2n2ppp/8/2pP4/3BPN2/8/PPP3PP/4K2R w K - 0 1";
constexpr const char* MIRROR_BASE_FEN = "4k3/8/8/3r4/4Q3/8/8/4K3 w - - 0 1";
constexpr const char* PHASE_BASE_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
constexpr const char* PHASE_OVERFLOW_FEN = "rnbqkbnr/pppppppp/3Q4/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}

std::string mirrorFen(std::string_view fen) {
    const std::string input(fen);
    const std::size_t spacePos = input.find(' ');
    const std::string boardPart = input.substr(0, spacePos);
    const char active = (spacePos != std::string::npos && spacePos + 1 < input.size())
                            ? input[spacePos + 1]
                            : 'w';

    std::array<std::array<char, 8>, 8> grid{};
    for (auto& row : grid) {
        row.fill('.');
    }

    std::size_t rank = 0;
    std::size_t file = 0;
    for (char ch : boardPart) {
        if (ch == '/') {
            ++rank;
            file = 0;
            continue;
        }
        if (std::isdigit(static_cast<unsigned char>(ch))) {
            file += static_cast<std::size_t>(ch - '0');
            continue;
        }
        if (rank < 8 && file < 8) {
            grid[rank][file] = ch;
        }
        ++file;
    }

    std::array<std::array<char, 8>, 8> mirrored{};
    for (auto& row : mirrored) {
        row.fill('.');
    }

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            const char piece = grid[r][c];
            if (piece == '.') {
                continue;
            }
            const unsigned char u = static_cast<unsigned char>(piece);
            const char mirroredPiece = std::islower(u)
                                            ? static_cast<char>(std::toupper(u))
                                            : static_cast<char>(std::tolower(u));
            mirrored[7 - r][c] = mirroredPiece;
        }
    }

    const auto compressRank = [](const std::array<char, 8>& row) {
        std::string result;
        int empty = 0;
        for (char cell : row) {
            if (cell == '.') {
                ++empty;
                continue;
            }
            if (empty > 0) {
                result.push_back(static_cast<char>('0' + empty));
                empty = 0;
            }
            result.push_back(cell);
        }
        if (empty > 0) {
            result.push_back(static_cast<char>('0' + empty));
        }
        return result;
    };

    std::string mirroredBoard;
    for (int r = 0; r < 8; ++r) {
        if (r > 0) {
            mirroredBoard.push_back('/');
        }
        mirroredBoard += compressRank(mirrored[r]);
    }

    const char mirroredColor = (active == 'w') ? 'b' : 'w';
    return mirroredBoard + ' ' + std::string(1, mirroredColor) + " - - 0 1";
}

class EvaluateTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        Position::init();
        Evaluate::init();
    }

    Position position_;
};

TEST_F(EvaluateTest, CalcScoreStartPositionIsNeutral) {
    position_.set_FEN(START_FEN);
    EXPECT_EQ(Evaluate::calc_score(position_), 0);
}

TEST_F(EvaluateTest, CalcScoreDependsOnSideToMove) {
    position_.set_FEN(WHITE_ADVANTAGE_WHITE_TO_MOVE);
    const Evaluate::Score whiteToMove = Evaluate::calc_score(position_);

    position_.set_FEN(WHITE_ADVANTAGE_BLACK_TO_MOVE);
    const Evaluate::Score blackToMove = Evaluate::calc_score(position_);

    EXPECT_GT(whiteToMove, 0);
    EXPECT_LT(blackToMove, 0);
    EXPECT_EQ(whiteToMove, -blackToMove);
}

TEST_F(EvaluateTest, CalcScorePenalizesIsolatedPawns) {
    Position isolated;
    Position supported;
    isolated.set_FEN(ISOLATED_PAWNS_FEN);
    supported.set_FEN(SUPPORTED_PAWNS_FEN);

    const Evaluate::Score isolatedExpected = manualCalcScore(isolated);
    const Evaluate::Score supportedExpected = manualCalcScore(supported);

    EXPECT_EQ(Evaluate::calc_score(isolated), isolatedExpected);
    EXPECT_EQ(Evaluate::calc_score(supported), supportedExpected);
    EXPECT_GT(supportedExpected, isolatedExpected);
    EXPECT_EQ(Evaluate::calc_score(supported) - Evaluate::calc_score(isolated),
              supportedExpected - isolatedExpected);
}

TEST_F(EvaluateTest, CalcScoreRewardsPassedPawnInMiddleGame) {
    Position passed;
    Position blocked;
    passed.set_FEN(PASSED_PAWN_MG_FEN);
    blocked.set_FEN(BLOCKED_PAWN_MG_FEN);

    const Evaluate::Score passedExpected = manualCalcScore(passed);
    const Evaluate::Score blockedExpected = manualCalcScore(blocked);

    EXPECT_EQ(Evaluate::calc_score(passed), passedExpected);
    EXPECT_EQ(Evaluate::calc_score(blocked), blockedExpected);

    EXPECT_EQ(manualGamePhase(passed), MAX_PHASE_PIECE_WEIGHT);
    EXPECT_EQ(manualGamePhase(blocked), MAX_PHASE_PIECE_WEIGHT);
    EXPECT_GT(passedExpected, blockedExpected);
}

TEST_F(EvaluateTest, CalcScoreRewardsPassedPawnInEndgame) {
    Position passed;
    Position blocked;
    passed.set_FEN(PASSED_PAWN_EG_FEN);
    blocked.set_FEN(BLOCKED_PAWN_EG_FEN);

    const Evaluate::Score passedExpected = manualCalcScore(passed);
    const Evaluate::Score blockedExpected = manualCalcScore(blocked);

    EXPECT_EQ(Evaluate::calc_score(passed), passedExpected);
    EXPECT_EQ(Evaluate::calc_score(blocked), blockedExpected);

    EXPECT_EQ(manualGamePhase(passed), 0);
    EXPECT_EQ(manualGamePhase(blocked), 0);
    EXPECT_GT(passedExpected, blockedExpected);
}

TEST_F(EvaluateTest, MaterialDrawDetectionMatchesCommonCases) {
    Position pos;

    pos.set_FEN(BARE_KINGS_FEN);
    EXPECT_TRUE(Evaluate::material_draw(pos));

    pos.set_FEN(KBK_FEN);
    EXPECT_TRUE(Evaluate::material_draw(pos));

    pos.set_FEN(KNK_FEN);
    EXPECT_TRUE(Evaluate::material_draw(pos));

    pos.set_FEN(KNNK_FEN);
    EXPECT_TRUE(Evaluate::material_draw(pos));

    pos.set_FEN(KBKB_SAME_COLOR_FEN);
    EXPECT_TRUE(Evaluate::material_draw(pos));

    pos.set_FEN(KBNK_FEN);
    EXPECT_FALSE(Evaluate::material_draw(pos));

    pos.set_FEN(KRK_FEN);
    EXPECT_FALSE(Evaluate::material_draw(pos));
}


TEST_F(EvaluateTest, CalcScoreSingleKnightMatchesManualBlend) {
    position_.set_FEN("4k3/8/8/8/8/2N5/8/4K3 w - - 0 1");

    const Evaluate::Score expected = manualCalcScore(position_);
    EXPECT_EQ(Evaluate::calc_score(position_), expected);
}

TEST_F(EvaluateTest, CalcScoreWeightsIncreaseWithSupportingPieces) {
    position_.set_FEN("4k3/8/8/8/8/2N5/8/4K3 w - - 0 1");
    const Evaluate::Score base_expected = manualCalcScore(position_);
    const Evaluate::Score base_score = Evaluate::calc_score(position_);
    EXPECT_EQ(base_score, base_expected);

    Position richer;
    richer.set_FEN("r1bqkb1r/8/8/8/8/2N5/8/R1BQKB1R w - - 0 1");

    const Evaluate::Score richer_expected = manualCalcScore(richer);
    const Evaluate::Score richer_score = Evaluate::calc_score(richer);

    EXPECT_EQ(richer_score, richer_expected);
    EXPECT_GT(richer_score, base_score);
    EXPECT_EQ(manualGamePhase(richer), 21);
}

TEST_F(EvaluateTest, CalcScoreRespondsToKnightSquareTables) {
    Position central;
    central.set_FEN("4k3/8/8/8/8/2N5/8/4K3 w - - 0 1");

    Position corner;
    corner.set_FEN("4k3/8/8/8/8/8/8/N3K3 w - - 0 1");

    const Evaluate::Score centralExpected = manualCalcScore(central);
    const Evaluate::Score cornerExpected = manualCalcScore(corner);

    EXPECT_EQ(Evaluate::calc_score(central), centralExpected);
    EXPECT_EQ(Evaluate::calc_score(corner), cornerExpected);
    EXPECT_GT(centralExpected, cornerExpected);
}

TEST_F(EvaluateTest, PST_Bishop_CenterBeatsCorner_MGandEG) {
    // Bishop in the centre should earn better PST scores than in the corner.
    Position centerMg;
    centerMg.set_FEN(BISHOP_CENTER_MG_FEN);
    const auto centerMgBreakdown = manualScoreBreakdown(centerMg);
    EXPECT_EQ(Evaluate::calc_score(centerMg), centerMgBreakdown.blended);
    EXPECT_EQ(centerMgBreakdown.mgWeight, MAX_PHASE_PIECE_WEIGHT);

    Position cornerMg;
    cornerMg.set_FEN(BISHOP_CORNER_MG_FEN);
    const auto cornerMgBreakdown = manualScoreBreakdown(cornerMg);
    EXPECT_EQ(Evaluate::calc_score(cornerMg), cornerMgBreakdown.blended);
    EXPECT_EQ(cornerMgBreakdown.mgWeight, MAX_PHASE_PIECE_WEIGHT);
    EXPECT_GT(centerMgBreakdown.mgScore, cornerMgBreakdown.mgScore);

    Position centerEg;
    centerEg.set_FEN(BISHOP_CENTER_EG_FEN);
    const auto centerEgBreakdown = manualScoreBreakdown(centerEg);
    EXPECT_EQ(Evaluate::calc_score(centerEg), centerEgBreakdown.blended);

    Position cornerEg;
    cornerEg.set_FEN(BISHOP_CORNER_EG_FEN);
    const auto cornerEgBreakdown = manualScoreBreakdown(cornerEg);
    EXPECT_EQ(Evaluate::calc_score(cornerEg), cornerEgBreakdown.blended);
    EXPECT_GT(centerEgBreakdown.egScore, cornerEgBreakdown.egScore);
}

TEST_F(EvaluateTest, PST_Rook_CenterFileBeatsCorner_MGandEG) {
    // Central rook activity must outperform a corner rook in both phases.
    Position centerMg;
    centerMg.set_FEN(ROOK_CENTER_MG_FEN);
    const auto centerMgBreakdown = manualScoreBreakdown(centerMg);
    EXPECT_EQ(Evaluate::calc_score(centerMg), centerMgBreakdown.blended);
    EXPECT_EQ(centerMgBreakdown.mgWeight, MAX_PHASE_PIECE_WEIGHT);

    Position cornerMg;
    cornerMg.set_FEN(ROOK_CORNER_MG_FEN);
    const auto cornerMgBreakdown = manualScoreBreakdown(cornerMg);
    EXPECT_EQ(Evaluate::calc_score(cornerMg), cornerMgBreakdown.blended);
    EXPECT_EQ(cornerMgBreakdown.mgWeight, MAX_PHASE_PIECE_WEIGHT);
    EXPECT_GT(centerMgBreakdown.mgScore, cornerMgBreakdown.mgScore);

    Position centerEg;
    centerEg.set_FEN(ROOK_CENTER_EG_FEN);
    const auto centerEgBreakdown = manualScoreBreakdown(centerEg);
    EXPECT_EQ(Evaluate::calc_score(centerEg), centerEgBreakdown.blended);

    Position cornerEg;
    cornerEg.set_FEN(ROOK_CORNER_EG_FEN);
    const auto cornerEgBreakdown = manualScoreBreakdown(cornerEg);
    EXPECT_EQ(Evaluate::calc_score(cornerEg), cornerEgBreakdown.blended);
    EXPECT_GT(centerEgBreakdown.egScore, cornerEgBreakdown.egScore);
}

TEST_F(EvaluateTest, PST_Queen_CenterBeatsCorner_MGandEG) {
    // Queen mobility in the centre should deliver the highest PST gain.
    Position centerMg;
    centerMg.set_FEN(QUEEN_CENTER_MG_FEN);
    const auto centerMgBreakdown = manualScoreBreakdown(centerMg);
    EXPECT_EQ(Evaluate::calc_score(centerMg), centerMgBreakdown.blended);
    EXPECT_EQ(centerMgBreakdown.mgWeight, MAX_PHASE_PIECE_WEIGHT);

    Position cornerMg;
    cornerMg.set_FEN(QUEEN_CORNER_MG_FEN);
    const auto cornerMgBreakdown = manualScoreBreakdown(cornerMg);
    EXPECT_EQ(Evaluate::calc_score(cornerMg), cornerMgBreakdown.blended);
    EXPECT_EQ(cornerMgBreakdown.mgWeight, MAX_PHASE_PIECE_WEIGHT);
    EXPECT_GT(centerMgBreakdown.mgScore, cornerMgBreakdown.mgScore);

    Position centerEg;
    centerEg.set_FEN(QUEEN_CENTER_EG_FEN);
    const auto centerEgBreakdown = manualScoreBreakdown(centerEg);
    EXPECT_EQ(Evaluate::calc_score(centerEg), centerEgBreakdown.blended);

    Position cornerEg;
    cornerEg.set_FEN(QUEEN_CORNER_EG_FEN);
    const auto cornerEgBreakdown = manualScoreBreakdown(cornerEg);
    EXPECT_EQ(Evaluate::calc_score(cornerEg), cornerEgBreakdown.blended);
    EXPECT_GT(centerEgBreakdown.egScore, cornerEgBreakdown.egScore);
}

TEST_F(EvaluateTest, PST_Flip_BlackValueMatchesWhiteFlipped) {
    // Mirrored squares must yield identical PST magnitudes across colours.
    constexpr Square64 whiteSq = Square64::SQ64_E4;
    constexpr Square64 blackSq = Square64(flipIndex(static_cast<int>(whiteSq)));
    EXPECT_EQ(mgPieceValue(Piece::W_BISHOP, whiteSq),
              mgPieceValue(Piece::B_BISHOP, blackSq));
    EXPECT_EQ(egPieceValue(Piece::W_BISHOP, whiteSq),
              egPieceValue(Piece::B_BISHOP, blackSq));

    Position whitePos;
    whitePos.set_FEN(BISHOP_CENTER_EG_FEN);
    const auto whiteBreakdown = manualScoreBreakdown(whitePos);
    EXPECT_EQ(Evaluate::calc_score(whitePos), whiteBreakdown.blended);
    const Bitboard whiteKingBb =
        whitePos.get_pieceTypes_bitboard(Color::WHITE, PieceType::KING);
    const Square64 whiteKingSq = Square64(Bitboards::ctz(whiteKingBb));
    const Bitboard whiteBishopBb =
        whitePos.get_pieceTypes_bitboard(Color::WHITE, PieceType::BISHOP);
    const Square64 whiteBishopSq = Square64(Bitboards::ctz(whiteBishopBb));

    Position blackPos;
    const std::string blackFen = mirrorFen(BISHOP_CENTER_EG_FEN);
    blackPos.set_FEN(blackFen.c_str());
    const auto blackBreakdown = manualScoreBreakdown(blackPos);
    EXPECT_EQ(Evaluate::calc_score(blackPos), blackBreakdown.blended);
    const Bitboard blackKingBb =
        blackPos.get_pieceTypes_bitboard(Color::BLACK, PieceType::KING);
    const Square64 blackKingSq = Square64(Bitboards::ctz(blackKingBb));
    const Bitboard blackBishopBb =
        blackPos.get_pieceTypes_bitboard(Color::BLACK, PieceType::BISHOP);
    const Square64 blackBishopSq = Square64(Bitboards::ctz(blackBishopBb));

    const Evaluate::Score whiteBishopMg =
        whiteBreakdown.mgTotals[Color::WHITE] -
        mgPieceValue(Piece::W_KING, whiteKingSq);
    const Evaluate::Score blackBishopMg =
        blackBreakdown.mgTotals[Color::BLACK] -
        mgPieceValue(Piece::B_KING, blackKingSq);
    EXPECT_EQ(whiteBishopMg, mgPieceValue(Piece::W_BISHOP, whiteBishopSq));
    EXPECT_EQ(blackBishopMg, mgPieceValue(Piece::B_BISHOP, blackBishopSq));
    EXPECT_EQ(whiteBishopMg, blackBishopMg);

    const Evaluate::Score whiteBishopEg =
        whiteBreakdown.egTotals[Color::WHITE] -
        egPieceValue(Piece::W_KING, whiteKingSq);
    const Evaluate::Score blackBishopEg =
        blackBreakdown.egTotals[Color::BLACK] -
        egPieceValue(Piece::B_KING, blackKingSq);
    EXPECT_EQ(whiteBishopEg, egPieceValue(Piece::W_BISHOP, whiteBishopSq));
    EXPECT_EQ(blackBishopEg, egPieceValue(Piece::B_BISHOP, blackBishopSq));
    EXPECT_EQ(whiteBishopEg, blackBishopEg);
}

TEST_F(EvaluateTest, Blending_IntermediatePhase_EqualsManualBlend) {
    // Mixed material should keep manual and engine blending identical.
    Position pos;
    pos.set_FEN(INTERMEDIATE_PHASE_FEN);
    const auto breakdown = manualScoreBreakdown(pos);
    EXPECT_GT(breakdown.phase, 5);
    EXPECT_LT(breakdown.phase, 19);
    EXPECT_EQ(Evaluate::calc_score(pos), breakdown.blended);
}

TEST_F(EvaluateTest, PawnMasks_IsolatedOnFileA_OnlyRightNeighborConsidered) {
    // File A isolation must rely solely on the file B neighbour.
    Position isolated;
    isolated.set_FEN(ISOLATED_FILE_A_FEN);
    const auto isolatedBreakdown = manualScoreBreakdown(isolated);
    EXPECT_EQ(Evaluate::calc_score(isolated), isolatedBreakdown.blended);

    Position supported;
    supported.set_FEN(SUPPORTED_FILE_A_FEN);
    const auto supportedBreakdown = manualScoreBreakdown(supported);
    EXPECT_EQ(Evaluate::calc_score(supported), supportedBreakdown.blended);

    const auto isolatedMgAdjustment =
        isolatedBreakdown.mgTotals[Color::WHITE] - accumulateMgBase(isolated, Color::WHITE);
    const auto supportedMgAdjustment =
        supportedBreakdown.mgTotals[Color::WHITE] - accumulateMgBase(supported, Color::WHITE);
    EXPECT_EQ(isolatedMgAdjustment, MG_ISOLATED_PAWN_PENALTY);
    EXPECT_EQ(supportedMgAdjustment, 0);

    const auto isolatedEgAdjustment =
        isolatedBreakdown.egTotals[Color::WHITE] - accumulateEgBase(isolated, Color::WHITE);
    const auto supportedEgAdjustment =
        supportedBreakdown.egTotals[Color::WHITE] - accumulateEgBase(supported, Color::WHITE);
    EXPECT_EQ(isolatedEgAdjustment, EG_ISOLATED_PAWN_PENALTY);
    EXPECT_EQ(supportedEgAdjustment, 0);
}

TEST_F(EvaluateTest, PawnMasks_PassedWhiteOn7th_NoForwardSquaresBonusBeyond7th) {
    // White pawn on the seventh must still earn exactly the rank bonus.
    Position passed;
    passed.set_FEN(WHITE_PASSED_ON_SEVENTH_FEN);
    const auto passedBreakdown = manualScoreBreakdown(passed);
    EXPECT_EQ(Evaluate::calc_score(passed), passedBreakdown.blended);

    Position blocked;
    blocked.set_FEN(WHITE_BLOCKED_ON_SEVENTH_FEN);
    const auto blockedBreakdown = manualScoreBreakdown(blocked);
    EXPECT_EQ(Evaluate::calc_score(blocked), blockedBreakdown.blended);

    const Evaluate::Score expectedMg = MG_PASSED_PAWN_BONUS_BY_RANK[6];
    const Evaluate::Score expectedEg = EG_PASSED_PAWN_BONUS_BY_RANK[6];
    const Evaluate::Score passedMgAdjustment =
        passedBreakdown.mgTotals[Color::WHITE] - accumulateMgBase(passed, Color::WHITE);
    const Evaluate::Score blockedMgAdjustment =
        blockedBreakdown.mgTotals[Color::WHITE] - accumulateMgBase(blocked, Color::WHITE);
    const Evaluate::Score passedEgAdjustment =
        passedBreakdown.egTotals[Color::WHITE] - accumulateEgBase(passed, Color::WHITE);
    const Evaluate::Score blockedEgAdjustment =
        blockedBreakdown.egTotals[Color::WHITE] - accumulateEgBase(blocked, Color::WHITE);
    EXPECT_EQ(passedMgAdjustment - blockedMgAdjustment, expectedMg);
    EXPECT_EQ(passedEgAdjustment - blockedEgAdjustment, expectedEg);
}

TEST_F(EvaluateTest, PawnMasks_PassedBlackOn2nd_SymmetricBehavior) {
    // Black promotions near the second rank should mirror the white logic.
    Position passed;
    passed.set_FEN(BLACK_PASSED_ON_SECOND_FEN);
    const auto passedBreakdown = manualScoreBreakdown(passed);
    EXPECT_EQ(Evaluate::calc_score(passed), passedBreakdown.blended);

    Position blocked;
    blocked.set_FEN(BLACK_BLOCKED_ON_SECOND_FEN);
    const auto blockedBreakdown = manualScoreBreakdown(blocked);
    EXPECT_EQ(Evaluate::calc_score(blocked), blockedBreakdown.blended);

    const Evaluate::Score expectedMg = MG_PASSED_PAWN_BONUS_BY_RANK[6];
    const Evaluate::Score expectedEg = EG_PASSED_PAWN_BONUS_BY_RANK[6];
    const Evaluate::Score passedMgAdjustment =
        passedBreakdown.mgTotals[Color::BLACK] - accumulateMgBase(passed, Color::BLACK);
    const Evaluate::Score blockedMgAdjustment =
        blockedBreakdown.mgTotals[Color::BLACK] - accumulateMgBase(blocked, Color::BLACK);
    const Evaluate::Score passedEgAdjustment =
        passedBreakdown.egTotals[Color::BLACK] - accumulateEgBase(passed, Color::BLACK);
    const Evaluate::Score blockedEgAdjustment =
        blockedBreakdown.egTotals[Color::BLACK] - accumulateEgBase(blocked, Color::BLACK);
    EXPECT_EQ(passedMgAdjustment - blockedMgAdjustment, expectedMg);
    EXPECT_EQ(passedEgAdjustment - blockedEgAdjustment, expectedEg);
}

TEST_F(EvaluateTest, ColorMirror_ScoreNegatesUnderSideToMoveSwap) {
    // Mirroring colours and side-to-move must flip the evaluation sign.
    Position original;
    original.set_FEN(MIRROR_BASE_FEN);
    const auto originalBreakdown = manualScoreBreakdown(original);
    const Evaluate::Score originalScore = Evaluate::calc_score(original);
    EXPECT_EQ(originalScore, originalBreakdown.blended);

    const std::string mirroredFen = mirrorFen(MIRROR_BASE_FEN);
    Position mirrored;
    mirrored.set_FEN(mirroredFen.c_str());
    const auto mirroredBreakdown = manualScoreBreakdown(mirrored);
    const Evaluate::Score mirroredScore = Evaluate::calc_score(mirrored);
    EXPECT_EQ(mirroredScore, mirroredBreakdown.blended);
    EXPECT_EQ(originalBreakdown.phase, mirroredBreakdown.phase);
    EXPECT_EQ(originalScore, mirroredScore);

    const Evaluate::Score originalWhiteMg =
        originalBreakdown.mgTotals[Color::WHITE] - originalBreakdown.mgTotals[Color::BLACK];
    const Evaluate::Score originalWhiteEg =
        originalBreakdown.egTotals[Color::WHITE] - originalBreakdown.egTotals[Color::BLACK];
    const Evaluate::Score mirroredWhiteMg =
        mirroredBreakdown.mgTotals[Color::WHITE] - mirroredBreakdown.mgTotals[Color::BLACK];
    const Evaluate::Score mirroredWhiteEg =
        mirroredBreakdown.egTotals[Color::WHITE] - mirroredBreakdown.egTotals[Color::BLACK];
    EXPECT_EQ(originalWhiteMg, -mirroredWhiteMg);
    EXPECT_EQ(originalWhiteEg, -mirroredWhiteEg);

    const Evaluate::Score originalWhiteBlend =
        ((originalWhiteMg * originalBreakdown.mgWeight) +
         (originalWhiteEg * originalBreakdown.egWeight)) /
        MAX_PHASE_PIECE_WEIGHT;
    const Evaluate::Score mirroredWhiteBlend =
        ((mirroredWhiteMg * mirroredBreakdown.mgWeight) +
         (mirroredWhiteEg * mirroredBreakdown.egWeight)) /
        MAX_PHASE_PIECE_WEIGHT;
    EXPECT_EQ(originalWhiteBlend, -mirroredWhiteBlend);
}

TEST_F(EvaluateTest, Phase_ClampedAtMaximum_WhenMaterialExceedsCap) {
    // Additional promoted material should not push the phase beyond the cap.
    Position base;
    base.set_FEN(PHASE_BASE_FEN);
    const auto baseBreakdown = manualScoreBreakdown(base);
    EXPECT_EQ(baseBreakdown.mgWeight, MAX_PHASE_PIECE_WEIGHT);

    Position overflow;
    overflow.set_FEN(PHASE_OVERFLOW_FEN);
    const auto overflowBreakdown = manualScoreBreakdown(overflow);
    EXPECT_GT(overflowBreakdown.phase, MAX_PHASE_PIECE_WEIGHT);
    EXPECT_EQ(overflowBreakdown.mgWeight, MAX_PHASE_PIECE_WEIGHT);
    EXPECT_EQ(Evaluate::calc_score(base), baseBreakdown.blended);
    EXPECT_EQ(Evaluate::calc_score(overflow), overflowBreakdown.blended);
}
