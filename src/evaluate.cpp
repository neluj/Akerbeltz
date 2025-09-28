#include "evaluate.h"
#include "position.h"

namespace Xake{

namespace Evaluate {

//Function Declarations
void init_isolated_masks();
void init_passed_masks();
Score eval_pawn_structure(const Position& pos);
Score eval_open_files(const Position& pos);
int flip(int sq);


//Material Scores
constexpr Score MG_PIECE_SCORES[PIECETYPE_SIZE] = {0,82,337,365,477,1025,20000};
constexpr Score EG_PIECE_SCORES[PIECETYPE_SIZE] = {0,94,281,297,512,936,20000};

//Pawn Structures
constexpr Score ISOLATED_PAWN_PENALTY = 15;
constexpr Score PASSED_PAWN_BONUS_BY_RANK[FILE_SIZE] = {
    0, 10, 20, 35, 60, 90, 140, 0
};

//Open File
constexpr Score ROOK_OPEN_FILE_BONUS      = 32; 
constexpr Score ROOK_SEMIOPEN_FILE_BONUS  = 18; 
constexpr Score QUEEN_OPEN_FILE_BONUS     = 14; 
constexpr Score QUEEN_SEMIOPEN_FILE_BONUS = 7;  


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

constexpr Score nopiece_table[SQ64_SIZE] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
};

constexpr Score mg_pawn_table[SQ64_SIZE] = {
      0,   0,   0,   0,   0,   0,  0,   0,
    -35,  -1, -20, -23, -15,  24, 38, -22,
    -26,  -4,  -4, -10,   3,   3, 33, -12,
    -27,  -2,  -5,  12,  17,   6, 10, -25,
    -14,  13,   6,  21,  23,  12, 17, -23,
     -6,   7,  26,  31,  65,  56, 25, -20,
     98, 134,  61,  95,  68, 126, 34, -11,
      0,   0,   0,   0,   0,   0,  0,   0,
};

constexpr Score eg_pawn_table[SQ64_SIZE] = {
      0,   0,   0,   0,   0,   0,   0,   0,
     13,   8,   8,  10,  13,   0,   2,  -7,
      4,   7,  -6,   1,   0,  -5,  -1,  -8,
     13,   9,  -3,  -7,  -7,  -8,   3,  -1,
     32,  24,  13,   5,  -2,   4,  17,  17,
     94, 100,  85,  67,  56,  53,  82,  84,
    178, 173, 158, 134, 147, 132, 165, 187,
      0,   0,   0,   0,   0,   0,   0,   0,
};

constexpr Score mg_knight_table[SQ64_SIZE] = {
    -105, -21, -58, -33, -17, -28, -19,  -23,
     -29, -53, -12,  -3,  -1,  18, -14,  -19,
     -23,  -9,  12,  10,  19,  17,  25,  -16,
     -13,   4,  16,  13,  28,  19,  21,   -8,
      -9,  17,  19,  53,  37,  69,  18,   22,
     -47,  60,  37,  65,  84, 129,  73,   44,
     -73, -41,  72,  36,  23,  62,   7,  -17,
    -167, -89, -34, -49,  61, -97, -15, -107,
};

constexpr Score eg_knight_table[SQ64_SIZE] = {
    -29, -51, -23, -15, -22, -18, -50, -64,
    -42, -20, -10,  -5,  -2, -20, -23, -44,
    -23,  -3,  -1,  15,  10,  -3, -20, -22,
    -18,  -6,  16,  25,  16,  17,   4, -18,
    -17,   3,  22,  22,  22,  11,   8, -18,
    -24, -20,  10,   9,  -1,  -9, -19, -41,
    -25,  -8, -25,  -2,  -9, -25, -24, -52,
    -58, -38, -13, -28, -31, -27, -63, -99,

};

constexpr Score mg_bishop_table[SQ64_SIZE] = {
    -33,  -3, -14, -21, -13, -12, -39, -21,
      4,  15,  16,   0,   7,  21,  33,   1,
      0,  15,  15,  15,  14,  27,  18,  10,
     -6,  13,  13,  26,  34,  12,  10,   4,
     -4,   5,  19,  50,  37,  37,   7,  -2,
    -16,  37,  43,  40,  35,  50,  37,  -2,
    -26,  16, -18, -13,  30,  59,  18, -47,
    -29,   4, -82, -37, -25, -42,   7,  -8,
};

constexpr Score eg_bishop_table[SQ64_SIZE] = {
    -23,  -9, -23,  -5, -9, -16,  -5, -17,
    -14, -18,  -7,  -1,  4,  -9, -15, -27,
    -12,  -3,   8,  10, 13,   3,  -7, -15,
     -6,   3,  13,  19,  7,  10,  -3,  -9,
     -3,   9,  12,   9, 14,  10,   3,   2,
      2,  -8,   0,  -1, -2,   6,   0,   4,
     -8,  -4,   7, -12, -3, -13,  -4, -14,
    -14, -21, -11,  -8, -7,  -9, -17, -24,

};

constexpr Score mg_rook_table[SQ64_SIZE] = {
    -19, -13,   1,  17, 16,  7, -37, -26,
    -44, -16, -20,  -9, -1, 11,  -6, -71,
    -45, -25, -16, -17,  3,  0,  -5, -33,
    -36, -26, -12,  -1,  9, -7,   6, -23,
    -24, -11,   7,  26, 24, 35,  -8, -20,
     -5,  19,  26,  36, 17, 45,  61,  16,
     27,  32,  58,  62, 80, 67,  26,  44,
     32,  42,  32,  51, 63,  9,  31,  43,
};

constexpr Score eg_rook_table[SQ64_SIZE] = {

    -9,  2,  3, -1, -5, -13,   4, -20,
    -6, -6,  0,  2, -9,  -9, -11,  -3,
    -4,  0, -5, -1, -7, -12,  -8, -16,
     3,  5,  8,  4, -5,  -6,  -8, -11,
     4,  3, 13,  1,  2,   1,  -1,   2,
     7,  7,  7,  5,  4,  -3,  -5,  -3,
    11, 13, 13, 11, -3,   3,   8,   3,
    13, 10, 18, 15, 12,  12,   8,   5,


};

constexpr Score mg_queen_table[SQ64_SIZE] = {
     -1, -18,  -9,  10, -15, -25, -31, -50,
    -35,  -8,  11,   2,   8,  15,  -3,   1,
    -14,   2, -11,  -2,  -5,   2,  14,   5,
     -9, -26,  -9, -10,  -2,  -4,   3,  -3,
    -27, -27, -16, -16,  -1,  17,  -2,   1,
    -13, -17,   7,   8,  29,  56,  47,  57,
    -24, -39,  -5,   1, -16,  57,  28,  54,
    -28,   0,  29,  12,  59,  44,  43,  45,
};

constexpr Score eg_queen_table[SQ64_SIZE] = {
    -33, -28, -22, -43,  -5, -32, -20, -41,
    -22, -23, -30, -16, -16, -23, -36, -32,
    -16, -27,  15,   6,   9,  17,  10,   5,
    -18,  28,  19,  47,  31,  34,  39,  23,
      3,  22,  24,  45,  57,  40,  57,  36,
    -20,   6,   9,  49,  47,  35,  19,   9,
    -17,  20,  32,  41,  58,  25,  30,   0,
     -9,  22,  22,  27,  27,  19,  10,  20,
};

constexpr Score mg_king_table[SQ64_SIZE] = {
    -15,  36,  12, -54,   8, -28,  24,  14,
      1,   7,  -8, -64, -43, -16,   9,   8,
    -14, -14, -22, -46, -44, -30, -15, -27,
    -49,  -1, -27, -39, -46, -44, -33, -51,
    -17, -20, -12, -27, -30, -25, -14, -36,
     -9,  24,   2, -16, -20,   6,  22, -22,
     29,  -1, -20,  -7,  -8,  -4, -38, -29,
    -65,  23,  16, -15, -56, -34,   2,  13,
};

constexpr Score eg_king_table[SQ64_SIZE] = {
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
    nopiece_table,
    mg_pawn_table,
    mg_knight_table,
    mg_bishop_table,
    mg_rook_table,
    mg_queen_table,
    mg_king_table
};

const Score* EG_PESTO_TABLES[PIECETYPE_SIZE] = {
    nopiece_table,
    eg_pawn_table,
    eg_knight_table,
    eg_bishop_table,
    eg_rook_table,
    eg_queen_table,
    eg_king_table
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

Score mg_pesto_score[PIECE_SIZE][SQ64_SIZE];
Score eg_pesto_score[PIECE_SIZE][SQ64_SIZE];

Bitboard isolated_masks[SQ64_SIZE];
Bitboard passed_masks[COLOR_SIZE][SQ64_SIZE];

void init(){

    for(int sq64 = 0; sq64 < SQ64_SIZE; ++sq64){
        mg_pesto_score[Piece::NO_PIECE][sq64] = MG_PIECE_SCORES[Piece::NO_PIECE] + MG_PESTO_TABLES[Piece::NO_PIECE][sq64];
        eg_pesto_score[Piece::NO_PIECE][sq64] = EG_PIECE_SCORES[Piece::NO_PIECE] + EG_PESTO_TABLES[Piece::NO_PIECE][sq64];
    }

    for(int p = Piece::W_PAWN, pt = PieceType::PAWN; p <= Piece::W_KING; ++p, ++pt){
        for(int sq = 0; sq < SQ64_SIZE; ++sq){
            mg_pesto_score[p][sq] = MG_PIECE_SCORES[pt] + MG_PESTO_TABLES[pt][sq];
            eg_pesto_score[p][sq] = EG_PIECE_SCORES[pt] + EG_PESTO_TABLES[pt][sq];
        }
    }
    //TODO hacer mas compacto con &BLACK donde BLACK sea 1000
    for(int p = Piece::B_PAWN, pt = PieceType::PAWN; p <= Piece::B_KING; ++p, pt++){
        for(int sq = 0; sq < SQ64_SIZE; ++sq){
            mg_pesto_score[p][sq] = MG_PIECE_SCORES[pt] + MG_PESTO_TABLES[pt][flip(sq)];
            eg_pesto_score[p][sq] = EG_PIECE_SCORES[pt] + EG_PESTO_TABLES[pt][flip(sq)];
        }
    }

    init_isolated_masks();
    init_passed_masks();

}

void init_isolated_masks() {
    for (Square64 sq64 = SQ64_A1; sq64 < SQ64_SIZE; ++sq64) {
        File file = square_file(sq64);

        Bitboard mask = ZERO;
        if (file > 0) mask |= Bitboards::FILE_A_MASK << (file - 1); 
        if (file < 7) mask |= Bitboards::FILE_A_MASK << (file + 1); 

        isolated_masks[sq64] = mask;
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

        passed_masks[WHITE][sq64] = files & white_forward;
        passed_masks[BLACK][sq64] = files & black_forward;
    }
}

Score calc_score(const Position &position){

    //TODO chage Score types and GamePhaseWeitht when are not exaclty scores
    Score mg_scores[COLOR_SIZE];
    Score eg_scores[COLOR_SIZE];
    GamePhaseWeight game_phase = 0;

    mg_scores[WHITE] = 0;
    mg_scores[BLACK] = 0;
    eg_scores[WHITE] = 0;
    eg_scores[BLACK] = 0;

    for(Square64 sq64 = SQ64_A1; sq64 < SQ64_SIZE; ++sq64){
        Piece piece = position.get_mailbox_piece(sq64);
        if(piece != NO_PIECE){
            mg_scores[piece_color(piece)] += mg_pesto_score[piece][sq64];
            eg_scores[piece_color(piece)] += eg_pesto_score[piece][sq64];
            game_phase += PHASE_PIECE_WEIGHT[piece];
        }
    }

    Color site_to_move = position.get_side_to_move();
    Score mg_score = mg_scores[site_to_move] - mg_scores[~site_to_move];
    Score eg_score = eg_scores[site_to_move] - eg_scores[~site_to_move];


    GamePhaseWeight mg_game_phase_weight = 0;
    GamePhaseWeight eg_game_phase_weight = 0;

    mg_game_phase_weight = (game_phase > MAX_PHASE_PIECE_WEIGHT) ? MAX_PHASE_PIECE_WEIGHT : game_phase;
    eg_game_phase_weight = MAX_PHASE_PIECE_WEIGHT - mg_game_phase_weight;

    return ((mg_score * mg_game_phase_weight) + (eg_score * eg_game_phase_weight)) / 24;
    //score += eval_pawn_structure(position);
    //score += eval_open_files(position);



    //return score * (~position.get_side_to_move() - position.get_side_to_move());

}

//Score calc_material_table(Piece piece, Square64 square){
//    return mg_pesto_score[piece][square];
//}

Score eval_pawn_structure(const Position& pos) {
    
    Score score = 0;

    const Bitboard wPawns = pos.get_pieceTypes_bitboard(WHITE, PAWN);
    const Bitboard bPawns = pos.get_pieceTypes_bitboard(BLACK, PAWN);

    Bitboard itWPawns = wPawns;
    
    while (itWPawns) {
        Square64 sq64{__builtin_ctzll(itWPawns)};
        itWPawns &= itWPawns - 1;
        
        if ((isolated_masks[sq64] & wPawns) == ZERO)
            score -= ISOLATED_PAWN_PENALTY;
        if ((passed_masks[WHITE][sq64] & bPawns) == ZERO) {
            Rank rank = square_rank(sq64);                 
            score += PASSED_PAWN_BONUS_BY_RANK[rank];
        }
    }

    Bitboard itBPawns = bPawns;
    
    while (itBPawns) {
        Square64 sq64{__builtin_ctzll(itBPawns)}; 
        itBPawns &= itBPawns - 1;

        if ((isolated_masks[sq64] & bPawns) == ZERO)
            score += ISOLATED_PAWN_PENALTY;  
        if ((passed_masks[BLACK][sq64] & wPawns) == ZERO) {
            Rank rank = square_rank(sq64);                 
            score -= PASSED_PAWN_BONUS_BY_RANK[7 - rank];   
        } 
    }

    return score;
}

Score eval_open_files(const Position& pos) {

    Score score = 0;

    const Bitboard wPawns = pos.get_pieceTypes_bitboard(WHITE, PAWN);
    const Bitboard bPawns = pos.get_pieceTypes_bitboard(BLACK, PAWN);
    const Bitboard allPawns = (wPawns | bPawns);

    //White rooks
    Bitboard rooks = pos.get_pieceTypes_bitboard(WHITE, ROOK);
    while (rooks){
        Square64 sq64 = Square64(__builtin_ctzll(rooks));
        rooks &= rooks - 1;

        const File file = square_file(sq64);
        const Bitboard mask = Bitboards::FILE_A_MASK << file;

        const Bitboard ownInFile = (wPawns & mask);
        if (ownInFile == ZERO){
            const Bitboard occInFile = (allPawns & mask);
            score += (occInFile == ZERO ? ROOK_OPEN_FILE_BONUS : ROOK_SEMIOPEN_FILE_BONUS);
        }
    }

    //Black rooks
    rooks = pos.get_pieceTypes_bitboard(BLACK, ROOK);
    while (rooks){
        Square64 sq64 = Square64(__builtin_ctzll(rooks));
        rooks &= rooks - 1;

        const File file = square_file(sq64);
        const Bitboard mask = Bitboards::FILE_A_MASK << file;

        const Bitboard ownInFile = (bPawns & mask);
        if (ownInFile == ZERO){
            const Bitboard occInFile = (allPawns & mask);
            score -= (occInFile == ZERO ? ROOK_OPEN_FILE_BONUS : ROOK_SEMIOPEN_FILE_BONUS);
        }
    }

    //White queens
    Bitboard queens = pos.get_pieceTypes_bitboard(WHITE, QUEEN);
    while (queens){
        Square64 sq64 = Square64(__builtin_ctzll(queens));
        queens &= queens - 1;

        const File file = square_file(sq64);
        const Bitboard mask = Bitboards::FILE_A_MASK << file;

        const Bitboard ownInFile = (wPawns & mask);
        if (ownInFile == ZERO){
            const Bitboard occInFile = (allPawns & mask);
            score += (occInFile == ZERO ? QUEEN_OPEN_FILE_BONUS : QUEEN_SEMIOPEN_FILE_BONUS);
        }
    }

    //Black queens
    queens = pos.get_pieceTypes_bitboard(BLACK, QUEEN);
    while (queens){
        Square64 sq64 = Square64(__builtin_ctzll(queens));
        queens &= queens - 1;

        const File file = square_file(sq64);
        const Bitboard mask = Bitboards::FILE_A_MASK << file;

        const Bitboard ownInFile = (bPawns & mask);
        if (ownInFile == ZERO){
            const Bitboard occInFile = (allPawns & mask);
            score -= (occInFile == ZERO ? QUEEN_OPEN_FILE_BONUS : QUEEN_SEMIOPEN_FILE_BONUS);
        }
    }

    return score;
}

bool material_draw(const Position& pos){

    const Bitboard wP = pos.get_pieceTypes_bitboard(WHITE, PAWN);
    const Bitboard bP = pos.get_pieceTypes_bitboard(BLACK, PAWN);
    if (wP || bP)
        return false;

    const Bitboard wQ = pos.get_pieceTypes_bitboard(WHITE, QUEEN);
    const Bitboard bQ = pos.get_pieceTypes_bitboard(BLACK, QUEEN);
    const Bitboard wR = pos.get_pieceTypes_bitboard(WHITE, ROOK);
    const Bitboard bR = pos.get_pieceTypes_bitboard(BLACK, ROOK);

    if (!wR && !bR && !wQ && !bQ){

        const Bitboard wB = pos.get_pieceTypes_bitboard(WHITE, BISHOP);
        const Bitboard bB = pos.get_pieceTypes_bitboard(BLACK, BISHOP);
        const Bitboard wN = pos.get_pieceTypes_bitboard(WHITE, KNIGHT);
        const Bitboard bN = pos.get_pieceTypes_bitboard(BLACK, KNIGHT);

        if (!wB && !bB){
            const int wn = __builtin_popcountll(wN);
            const int bn = __builtin_popcountll(bN);
            return (wn < 3 && bn == 0) || (bn < 3 && wn == 0);
        } else if (!wN && !bN){
            Bitboard bishops = wB | bB;
            const int totalB = __builtin_popcountll(bishops);
            if (totalB == 1)
                return true;
            if (totalB == 2){
                constexpr Bitboard DARK_SQUARES = 0xAA55AA55AA55AA55ULL;
                return ((bishops & DARK_SQUARES) == bishops) ||
                       ((bishops & ~DARK_SQUARES) == bishops);
            }
        }

    } else if (!wQ && !bQ){

        const Bitboard wB = pos.get_pieceTypes_bitboard(WHITE, BISHOP);
        const Bitboard bB = pos.get_pieceTypes_bitboard(BLACK, BISHOP);
        const Bitboard wN = pos.get_pieceTypes_bitboard(WHITE, KNIGHT);
        const Bitboard bN = pos.get_pieceTypes_bitboard(BLACK, KNIGHT);

        if (!wR && !wB && !wN){
            const int bn = __builtin_popcountll(bN);
            const int bb = __builtin_popcountll(bB);
            return !bR && (bn + bb) < 2;
        } else if (!bR && !bB && !bN){
            const int wn = __builtin_popcountll(wN);
            const int wb = __builtin_popcountll(wB);
            return !wR && (wn + wb) < 2;
        }
    }

    return false;
}

int flip(int sq){
    return sq^56;
}

}

}
