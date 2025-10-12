#include "evaluate.h"
#include "position.h"

namespace Xake{

namespace Evaluate {

//Function Declarations
void init_pesto_scores();
void init_isolated_masks();
void init_passed_masks();
int flip(int sq);


//Material Scores
constexpr Score MG_PIECE_SCORES[PIECETYPE_SIZE] = {0,82,337,365,477,1025,20000};
constexpr Score EG_PIECE_SCORES[PIECETYPE_SIZE] = {0,94,281,297,512,936,20000};

//Pawn Structures
Bitboard isolatedPawnMasks[SQ64_SIZE];
constexpr Score MG_ISOLATED_PAWN_PENALTY = -8;
constexpr Score EG_ISOLATED_PAWN_PENALTY = -18;

Bitboard passedMasks[COLOR_SIZE][SQ64_SIZE];
constexpr Score MG_PASSED_PAWN_BONUS_BY_RANK[RANK_SIZE] = {
    0, 0, 5, 12, 25, 45, 85, 0
};
constexpr Score EG_PASSED_PAWN_BONUS_BY_RANK[RANK_SIZE] = {
    0, 0, 10, 25, 50, 90,150, 0
};


/*
Based on PeSTOs evaluation function
https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function
*/
/*
 A1 B1 C1 D1 E1 F1 G1 H1
 A2 B2 C2 D2 E2 F2 G2 H2
 A3 B3 C3 D3 E3 F3 G3 H3
 A4 B4 C4 D4 E4 F4 G4 H4
 A5 B5 C5 D5 E5 F5 G5 H5
 A6 B6 C6 D6 E6 F6 G6 H6
 A7 B7 C7 D7 E7 F7 G7 H7
 A8 B8 C8 D8 E8 F8 G8 H8
*/

constexpr Score NO_PIECE_TABLE[SQ64_SIZE] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
};

constexpr Score MG_PAWN_TABLE[SQ64_SIZE] = {
      0,   0,   0,   0,   0,   0,  0,   0,
    -35,  -1, -20, -23, -15,  24, 38, -22,
    -26,  -4,  -4, -10,   3,   3, 33, -12,
    -27,  -2,  -5,  12,  17,   6, 10, -25,
    -14,  13,   6,  21,  23,  12, 17, -23,
     -6,   7,  26,  31,  65,  56, 25, -20,
     98, 134,  61,  95,  68, 126, 34, -11,
      0,   0,   0,   0,   0,   0,  0,   0,
};

constexpr Score EG_PAWN_TABLE[SQ64_SIZE] = {
      0,   0,   0,   0,   0,   0,   0,   0,
     13,   8,   8,  10,  13,   0,   2,  -7,
      4,   7,  -6,   1,   0,  -5,  -1,  -8,
     13,   9,  -3,  -7,  -7,  -8,   3,  -1,
     32,  24,  13,   5,  -2,   4,  17,  17,
     94, 100,  85,  67,  56,  53,  82,  84,
    178, 173, 158, 134, 147, 132, 165, 187,
      0,   0,   0,   0,   0,   0,   0,   0,
};

constexpr Score MG_KNIGHT_TABLE[SQ64_SIZE] = {
    -105, -21, -58, -33, -17, -28, -19,  -23,
     -29, -53, -12,  -3,  -1,  18, -14,  -19,
     -23,  -9,  12,  10,  19,  17,  25,  -16,
     -13,   4,  16,  13,  28,  19,  21,   -8,
      -9,  17,  19,  53,  37,  69,  18,   22,
     -47,  60,  37,  65,  84, 129,  73,   44,
     -73, -41,  72,  36,  23,  62,   7,  -17,
    -167, -89, -34, -49,  61, -97, -15, -107,
};

constexpr Score EG_KNIGHT_TABLE[SQ64_SIZE] = {
    -29, -51, -23, -15, -22, -18, -50, -64,
    -42, -20, -10,  -5,  -2, -20, -23, -44,
    -23,  -3,  -1,  15,  10,  -3, -20, -22,
    -18,  -6,  16,  25,  16,  17,   4, -18,
    -17,   3,  22,  22,  22,  11,   8, -18,
    -24, -20,  10,   9,  -1,  -9, -19, -41,
    -25,  -8, -25,  -2,  -9, -25, -24, -52,
    -58, -38, -13, -28, -31, -27, -63, -99,

};

constexpr Score MG_BISHOP_TABLE[SQ64_SIZE] = {
    -33,  -3, -14, -21, -13, -12, -39, -21,
      4,  15,  16,   0,   7,  21,  33,   1,
      0,  15,  15,  15,  14,  27,  18,  10,
     -6,  13,  13,  26,  34,  12,  10,   4,
     -4,   5,  19,  50,  37,  37,   7,  -2,
    -16,  37,  43,  40,  35,  50,  37,  -2,
    -26,  16, -18, -13,  30,  59,  18, -47,
    -29,   4, -82, -37, -25, -42,   7,  -8,
};

constexpr Score EG_BISHOP_TABLE[SQ64_SIZE] = {
    -23,  -9, -23,  -5, -9, -16,  -5, -17,
    -14, -18,  -7,  -1,  4,  -9, -15, -27,
    -12,  -3,   8,  10, 13,   3,  -7, -15,
     -6,   3,  13,  19,  7,  10,  -3,  -9,
     -3,   9,  12,   9, 14,  10,   3,   2,
      2,  -8,   0,  -1, -2,   6,   0,   4,
     -8,  -4,   7, -12, -3, -13,  -4, -14,
    -14, -21, -11,  -8, -7,  -9, -17, -24,
};

constexpr Score MG_ROOK_TABLE[SQ64_SIZE] = {
    -19, -13,   1,  17, 16,  7, -37, -26,
    -44, -16, -20,  -9, -1, 11,  -6, -71,
    -45, -25, -16, -17,  3,  0,  -5, -33,
    -36, -26, -12,  -1,  9, -7,   6, -23,
    -24, -11,   7,  26, 24, 35,  -8, -20,
     -5,  19,  26,  36, 17, 45,  61,  16,
     27,  32,  58,  62, 80, 67,  26,  44,
     32,  42,  32,  51, 63,  9,  31,  43,
};

constexpr Score EG_ROOK_TABLE[SQ64_SIZE] = {
    -9,  2,  3, -1, -5, -13,   4, -20,
    -6, -6,  0,  2, -9,  -9, -11,  -3,
    -4,  0, -5, -1, -7, -12,  -8, -16,
     3,  5,  8,  4, -5,  -6,  -8, -11,
     4,  3, 13,  1,  2,   1,  -1,   2,
     7,  7,  7,  5,  4,  -3,  -5,  -3,
    11, 13, 13, 11, -3,   3,   8,   3,
    13, 10, 18, 15, 12,  12,   8,   5,
};

constexpr Score MG_QUEEN_TABLE[SQ64_SIZE] = {
     -1, -18,  -9,  10, -15, -25, -31, -50,
    -35,  -8,  11,   2,   8,  15,  -3,   1,
    -14,   2, -11,  -2,  -5,   2,  14,   5,
     -9, -26,  -9, -10,  -2,  -4,   3,  -3,
    -27, -27, -16, -16,  -1,  17,  -2,   1,
    -13, -17,   7,   8,  29,  56,  47,  57,
    -24, -39,  -5,   1, -16,  57,  28,  54,
    -28,   0,  29,  12,  59,  44,  43,  45,
};

constexpr Score EG_QUEEN_TABLE[SQ64_SIZE] = {
    -33, -28, -22, -43,  -5, -32, -20, -41,
    -22, -23, -30, -16, -16, -23, -36, -32,
    -16, -27,  15,   6,   9,  17,  10,   5,
    -18,  28,  19,  47,  31,  34,  39,  23,
      3,  22,  24,  45,  57,  40,  57,  36,
    -20,   6,   9,  49,  47,  35,  19,   9,
    -17,  20,  32,  41,  58,  25,  30,   0,
     -9,  22,  22,  27,  27,  19,  10,  20,
};

constexpr Score MG_KING_TABLE[SQ64_SIZE] = {
    -15,  36,  12, -54,   8, -28,  24,  14,
      1,   7,  -8, -64, -43, -16,   9,   8,
    -14, -14, -22, -46, -44, -30, -15, -27,
    -49,  -1, -27, -39, -46, -44, -33, -51,
    -17, -20, -12, -27, -30, -25, -14, -36,
     -9,  24,   2, -16, -20,   6,  22, -22,
     29,  -1, -20,  -7,  -8,  -4, -38, -29,
    -65,  23,  16, -15, -56, -34,   2,  13,
};

constexpr Score EG_KING_TABLE[SQ64_SIZE] = {
    -53, -34, -21, -11, -28, -14, -24, -43,
    -27, -11,   4,  13,  14,   4,  -5, -17,
    -19,  -3,  11,  21,  23,  16,   7,  -9,
    -18,  -4,  21,  24,  27,  23,   9, -11,
     -8,  22,  24,  27,  26,  33,  26,   3,
     10,  17,  23,  15,  20,  45,  44,  13,
    -12,  17,  14,  17,  17,  38,  23,  11,
    -74, -35, -18, -18, -11,  15,   4, -17,
};


const Score* MG_PESTO_TABLES[PIECETYPE_SIZE] = {
    NO_PIECE_TABLE,
    MG_PAWN_TABLE,
    MG_KNIGHT_TABLE,
    MG_BISHOP_TABLE,
    MG_ROOK_TABLE,
    MG_QUEEN_TABLE,
    MG_KING_TABLE
};

const Score* EG_PESTO_TABLES[PIECETYPE_SIZE] = {
    NO_PIECE_TABLE,
    EG_PAWN_TABLE,
    EG_KNIGHT_TABLE,
    EG_BISHOP_TABLE,
    EG_ROOK_TABLE,
    EG_QUEEN_TABLE,
    EG_KING_TABLE
};

//TODO hacer mas compacto con &BLACK donde BLACK sea 1000
constexpr GamePhaseWeight PHASE_PIECE_WEIGHT[PIECE_SIZE] = {
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

//In case of early promotion, sum of every pieces will higher, and end game size will be negative.
constexpr Score MAX_PHASE_PIECE_WEIGHT = 24;

Score mgPestoScore[PIECE_SIZE][SQ64_SIZE];
Score egPestoScore[PIECE_SIZE][SQ64_SIZE];

void init(){

    init_pesto_scores();
    init_isolated_masks();
    init_passed_masks();

}

void init_pesto_scores(){

    for(int sq64 = 0; sq64 < SQ64_SIZE; ++sq64){
        mgPestoScore[Piece::NO_PIECE][sq64] = MG_PIECE_SCORES[Piece::NO_PIECE] + MG_PESTO_TABLES[Piece::NO_PIECE][sq64];
        egPestoScore[Piece::NO_PIECE][sq64] = EG_PIECE_SCORES[Piece::NO_PIECE] + EG_PESTO_TABLES[Piece::NO_PIECE][sq64];
    }

    for(int p = Piece::W_PAWN, pt = PieceType::PAWN; p <= Piece::W_KING; ++p, ++pt){
        for(int sq = 0; sq < SQ64_SIZE; ++sq){
            mgPestoScore[p][sq] = MG_PIECE_SCORES[pt] + MG_PESTO_TABLES[pt][sq];
            egPestoScore[p][sq] = EG_PIECE_SCORES[pt] + EG_PESTO_TABLES[pt][sq];
        }
    }

    for(int p = Piece::B_PAWN, pt = PieceType::PAWN; p <= Piece::B_KING; ++p, pt++){
        for(int sq = 0; sq < SQ64_SIZE; ++sq){
            mgPestoScore[p][sq] = MG_PIECE_SCORES[pt] + MG_PESTO_TABLES[pt][flip(sq)];
            egPestoScore[p][sq] = EG_PIECE_SCORES[pt] + EG_PESTO_TABLES[pt][flip(sq)];
        }
    }

}

void init_isolated_masks() {
    for (Square64 sq64 = SQ64_A1; sq64 < SQ64_SIZE; ++sq64) {
        File file = square_file(sq64);

        Bitboard mask = ZERO;
        if (file > 0) mask |= Bitboards::FILE_A_MASK << (file - 1); 
        if (file < 7) mask |= Bitboards::FILE_A_MASK << (file + 1); 

        isolatedPawnMasks[sq64] = mask;
    }
}

void init_passed_masks(){
    for (Square64 sq64 = SQ64_A1; sq64 < SQ64_SIZE; ++sq64) {
        File file = square_file(sq64); 
        Rank rank = square_rank(sq64);

        Bitboard files =
            Bitboards::FILE_A_MASK << file |
            (file > 0 ? Bitboards::FILE_A_MASK << (file - 1) : ZERO) |
            (file < 7 ? Bitboards::FILE_A_MASK << (file + 1) : ZERO);

        Bitboard white_forward = (rank < 7) ? (~ZERO << ((rank + 1) * 8)) : ZERO;
        Bitboard black_forward = (ONE << (rank * 8)) - ONE;

        passedMasks[WHITE][sq64] = files & white_forward;
        passedMasks[BLACK][sq64] = files & black_forward;
    }
}

Score calc_score(const Position &position){

    //TODO chage Score types and GamePhaseWeitht when are not exaclty scores
    Score mgScores[COLOR_SIZE];
    Score egScores[COLOR_SIZE];
    GamePhaseWeight gamePhase = 0;

    mgScores[WHITE] = 0;
    mgScores[BLACK] = 0;
    egScores[WHITE] = 0;
    egScores[BLACK] = 0;

    Bitboard whitePawnIt = position.get_pieceTypes_bitboard(WHITE, PAWN);
    Bitboard whitePawns = whitePawnIt;

    Bitboard blackPawnIt = position.get_pieceTypes_bitboard(BLACK, PAWN);
    Bitboard blackPawns = blackPawnIt;

    while (whitePawnIt) {
        Square64 sq64{Bitboards::ctz(whitePawnIt)};
        mgScores[WHITE] += mgPestoScore[W_PAWN][sq64];
        egScores[WHITE] += egPestoScore[W_PAWN][sq64];

        if ((whitePawns & isolatedPawnMasks[sq64]) == ZERO){
            mgScores[WHITE] += MG_ISOLATED_PAWN_PENALTY;
            egScores[WHITE] += EG_ISOLATED_PAWN_PENALTY;           
        }
        if ((passedMasks[WHITE][sq64] & blackPawns) == ZERO) {
            Rank rank = square_rank(sq64);                 
            mgScores[WHITE] += MG_PASSED_PAWN_BONUS_BY_RANK[rank];
            egScores[WHITE] += EG_PASSED_PAWN_BONUS_BY_RANK[rank];
        }
        //gamePhase += PHASE_PIECE_WEIGHT[W_PAWN];
        whitePawnIt &= whitePawnIt - 1;
    }

    while (blackPawnIt) {
        Square64 sq64{Bitboards::ctz(blackPawnIt)};
        mgScores[BLACK] += mgPestoScore[B_PAWN][sq64];
        egScores[BLACK] += egPestoScore[B_PAWN][sq64];

        if ((blackPawns & isolatedPawnMasks[sq64]) == ZERO){
            mgScores[BLACK] += MG_ISOLATED_PAWN_PENALTY;
            egScores[BLACK] += EG_ISOLATED_PAWN_PENALTY;           
        }
        if ((passedMasks[BLACK][sq64] & whitePawns) == ZERO) {
            Rank rank = square_rank(sq64);                 
            mgScores[BLACK] += MG_PASSED_PAWN_BONUS_BY_RANK[7-rank];
            egScores[BLACK] += EG_PASSED_PAWN_BONUS_BY_RANK[7-rank];
        }
        //gamePhase += PHASE_PIECE_WEIGHT[B_PAWN];
        blackPawnIt &= blackPawnIt - 1;
    }

    Bitboard whiteKnightIt = position.get_pieceTypes_bitboard(WHITE, KNIGHT);
    while (whiteKnightIt) {
        Square64 sq64{Bitboards::ctz(whiteKnightIt)};
        mgScores[WHITE] += mgPestoScore[W_KNIGHT][sq64];
        egScores[WHITE] += egPestoScore[W_KNIGHT][sq64];
        gamePhase += PHASE_PIECE_WEIGHT[W_KNIGHT];
        whiteKnightIt &= whiteKnightIt - 1;
    }

    Bitboard blackKnightIt = position.get_pieceTypes_bitboard(BLACK, KNIGHT);
    while (blackKnightIt) {
        Square64 sq64{Bitboards::ctz(blackKnightIt)};
        mgScores[BLACK] += mgPestoScore[B_KNIGHT][sq64];
        egScores[BLACK] += egPestoScore[B_KNIGHT][sq64];
        gamePhase += PHASE_PIECE_WEIGHT[B_KNIGHT];
        blackKnightIt &= blackKnightIt - 1;
    }

    Bitboard whiteBishopIt = position.get_pieceTypes_bitboard(WHITE, BISHOP);
    while (whiteBishopIt) {
        Square64 sq64{Bitboards::ctz(whiteBishopIt)};
        mgScores[WHITE] += mgPestoScore[W_BISHOP][sq64];
        egScores[WHITE] += egPestoScore[W_BISHOP][sq64];
        gamePhase += PHASE_PIECE_WEIGHT[W_BISHOP];
        whiteBishopIt &= whiteBishopIt - 1;
    }

    Bitboard blackBishopIt = position.get_pieceTypes_bitboard(BLACK, BISHOP);
    while (blackBishopIt) {
        Square64 sq64{Bitboards::ctz(blackBishopIt)};
        mgScores[BLACK] += mgPestoScore[B_BISHOP][sq64];
        egScores[BLACK] += egPestoScore[B_BISHOP][sq64];
        gamePhase += PHASE_PIECE_WEIGHT[B_BISHOP];
        blackBishopIt &= blackBishopIt - 1;
    }

    Bitboard whiteRookIt = position.get_pieceTypes_bitboard(WHITE, ROOK);
    while (whiteRookIt) {
        Square64 sq64{Bitboards::ctz(whiteRookIt)};
        mgScores[WHITE] += mgPestoScore[W_ROOK][sq64];
        egScores[WHITE] += egPestoScore[W_ROOK][sq64];
        gamePhase += PHASE_PIECE_WEIGHT[W_ROOK];
        whiteRookIt &= whiteRookIt - 1;
    }

    Bitboard blackRookIt = position.get_pieceTypes_bitboard(BLACK, ROOK);
    while (blackRookIt) {
        Square64 sq64{Bitboards::ctz(blackRookIt)};
        mgScores[BLACK] += mgPestoScore[B_ROOK][sq64];
        egScores[BLACK] += egPestoScore[B_ROOK][sq64];
        gamePhase += PHASE_PIECE_WEIGHT[B_ROOK];
        blackRookIt &= blackRookIt - 1;
    }

    Bitboard whiteQueenIt = position.get_pieceTypes_bitboard(WHITE, QUEEN);
    while (whiteQueenIt) {
        Square64 sq64{Bitboards::ctz(whiteQueenIt)};
        mgScores[WHITE] += mgPestoScore[W_QUEEN][sq64];
        egScores[WHITE] += egPestoScore[W_QUEEN][sq64];
        gamePhase += PHASE_PIECE_WEIGHT[W_QUEEN];
        whiteQueenIt &= whiteQueenIt - 1;
    }

    Bitboard blackQueenIt = position.get_pieceTypes_bitboard(BLACK, QUEEN);
    while (blackQueenIt) {
        Square64 sq64{Bitboards::ctz(blackQueenIt)};
        mgScores[BLACK] += mgPestoScore[B_QUEEN][sq64];
        egScores[BLACK] += egPestoScore[B_QUEEN][sq64];
        gamePhase += PHASE_PIECE_WEIGHT[B_QUEEN];
        blackQueenIt &= blackQueenIt - 1;
    }

    Bitboard whiteKingIt = position.get_pieceTypes_bitboard(WHITE, KING);
    while (whiteKingIt) {
        Square64 sq64{Bitboards::ctz(whiteKingIt)};
        mgScores[WHITE] += mgPestoScore[W_KING][sq64];
        egScores[WHITE] += egPestoScore[W_KING][sq64];
        gamePhase += PHASE_PIECE_WEIGHT[W_KING];
        whiteKingIt &= whiteKingIt - 1;
    }

    Bitboard blackKingIt = position.get_pieceTypes_bitboard(BLACK, KING);
    while (blackKingIt) {
        Square64 sq64{Bitboards::ctz(blackKingIt)};
        mgScores[BLACK] += mgPestoScore[B_KING][sq64];
        egScores[BLACK] += egPestoScore[B_KING][sq64];
        gamePhase += PHASE_PIECE_WEIGHT[B_KING];
        blackKingIt &= blackKingIt - 1;
    }

    Color siteToMove = position.get_side_to_move();
    Score mgScore = mgScores[siteToMove] - mgScores[~siteToMove];
    Score egScore = egScores[siteToMove] - egScores[~siteToMove];


    GamePhaseWeight mgGamePhaseWeight = 0;
    GamePhaseWeight egGamePhaseWeight = 0;

    mgGamePhaseWeight = (gamePhase > MAX_PHASE_PIECE_WEIGHT) ? MAX_PHASE_PIECE_WEIGHT : gamePhase;
    egGamePhaseWeight = MAX_PHASE_PIECE_WEIGHT - mgGamePhaseWeight;

    return ((mgScore * mgGamePhaseWeight) + (egScore * egGamePhaseWeight)) / 24;

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

int flip(int sq){
    return sq^56;
}

}

}
