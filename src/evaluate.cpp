#include "evaluate.h"
#include "position.h"

namespace Xake{

namespace Evaluate {

//Function Declarations
void init_isolated_masks();
void init_passed_masks();
Score eval_pawn_structure(const Position& pos);
int flip(int sq);


//Material Scores
Score material_evaluation_array[] = {0,82,337,365,477, 1025,20000};

//Pawn Structures
constexpr Score ISOLATED_PAWN_PENALTY = 15;
constexpr Score PASSED_PAWN_BONUS_BY_RANK[8] = {
    0, 10, 20, 35, 60, 90, 140, 0
};


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

int nopiece_table[SQUARE_SIZE_64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,
};

int pawn_table[SQUARE_SIZE_64] = {
      0,   0,   0,   0,   0,   0,  0,   0,
    -35,  -1, -20, -23, -15,  24, 38, -22,
    -26,  -4,  -4, -10,   3,   3, 33, -12,
    -27,  -2,  -5,  12,  17,   6, 10, -25,
    -14,  13,   6,  21,  23,  12, 17, -23,
     -6,   7,  26,  31,  65,  56, 25, -20,
     98, 134,  61,  95,  68, 126, 34, -11,
      0,   0,   0,   0,   0,   0,  0,   0,
};

int knight_table[SQUARE_SIZE_64] = {
    -105, -21, -58, -33, -17, -28, -19,  -23,
     -29, -53, -12,  -3,  -1,  18, -14,  -19,
     -23,  -9,  12,  10,  19,  17,  25,  -16,
     -13,   4,  16,  13,  28,  19,  21,   -8,
      -9,  17,  19,  53,  37,  69,  18,   22,
     -47,  60,  37,  65,  84, 129,  73,   44,
     -73, -41,  72,  36,  23,  62,   7,  -17,
    -167, -89, -34, -49,  61, -97, -15, -107,
};

int bishop_table[SQUARE_SIZE_64] = {
    -33,  -3, -14, -21, -13, -12, -39, -21,
      4,  15,  16,   0,   7,  21,  33,   1,
      0,  15,  15,  15,  14,  27,  18,  10,
     -6,  13,  13,  26,  34,  12,  10,   4,
     -4,   5,  19,  50,  37,  37,   7,  -2,
    -16,  37,  43,  40,  35,  50,  37,  -2,
    -26,  16, -18, -13,  30,  59,  18, -47,
    -29,   4, -82, -37, -25, -42,   7,  -8,
};

int rook_table[SQUARE_SIZE_64] = {
    -19, -13,   1,  17, 16,  7, -37, -26,
    -44, -16, -20,  -9, -1, 11,  -6, -71,
    -45, -25, -16, -17,  3,  0,  -5, -33,
    -36, -26, -12,  -1,  9, -7,   6, -23,
    -24, -11,   7,  26, 24, 35,  -8, -20,
     -5,  19,  26,  36, 17, 45,  61,  16,
     27,  32,  58,  62, 80, 67,  26,  44,
     32,  42,  32,  51, 63,  9,  31,  43,
};

int queen_table[SQUARE_SIZE_64] = {
     -1, -18,  -9,  10, -15, -25, -31, -50,
    -35,  -8,  11,   2,   8,  15,  -3,   1,
    -14,   2, -11,  -2,  -5,   2,  14,   5,
     -9, -26,  -9, -10,  -2,  -4,   3,  -3,
    -27, -27, -16, -16,  -1,  17,  -2,   1,
    -13, -17,   7,   8,  29,  56,  47,  57,
    -24, -39,  -5,   1, -16,  57,  28,  54,
    -28,   0,  29,  12,  59,  44,  43,  45,
};

int king_table[SQUARE_SIZE_64] = {
    -15,  36,  12, -54,   8, -28,  24,  14,
      1,   7,  -8, -64, -43, -16,   9,   8,
    -14, -14, -22, -46, -44, -30, -15, -27,
    -49,  -1, -27, -39, -46, -44, -33, -51,
    -17, -20, -12, -27, -30, -25, -14, -36,
     -9,  24,   2, -16, -20,   6,  22, -22,
     29,  -1, -20,  -7,  -8,  -4, -38, -29,
    -65,  23,  16, -15, -56, -34,   2,  13,
};


int* piecesTypes_tables[PIECETYPE_SIZE] = {
    nopiece_table,
    pawn_table,
    knight_table,
    bishop_table,
    rook_table,
    queen_table,
    king_table
};

Score material_table[PIECE_SIZE][SQUARE_SIZE_64];

Bitboard isolated_masks[SQUARE_SIZE_64];
Bitboard passed_masks[COLOR_SIZE][SQUARE_SIZE_64];

void init(){

    for(int sq = 0; sq < SQUARE_SIZE_64; ++sq){
        material_table[Piece::NO_PIECE][sq] = material_evaluation_array[Piece::NO_PIECE] + piecesTypes_tables[Piece::NO_PIECE][sq];
    }

    for(int p = Piece::W_PAWN, pt = PieceType::PAWN; p <= Piece::W_KING; ++p, ++pt){
        for(int sq = 0; sq < SQUARE_SIZE_64; ++sq){
            material_table[p][sq] = material_evaluation_array[pt] + piecesTypes_tables[pt][sq];
        }
    }

    for(int p = Piece::B_PAWN, pt = PieceType::PAWN; p <= Piece::B_KING; ++p, pt++){
        for(int sq = 0; sq < SQUARE_SIZE_64; ++sq){
            material_table[p][sq] = material_evaluation_array[pt] + piecesTypes_tables[pt][flip(sq)];
        }
    }

    init_isolated_masks();
    init_passed_masks();

}

void init_isolated_masks() {
    for (Square64 sq64 = SQ64_A1; sq64 < SQUARE_SIZE_64; ++sq64) {
        File file = square_file(sq64);

        Bitboard mask = ZERO;
        if (file > 0) mask |= Bitboards::FILE_A_MASK << (file - 1); 
        if (file < 7) mask |= Bitboards::FILE_A_MASK << (file + 1); 

        isolated_masks[sq64] = mask;
    }
}

void init_passed_masks(){
    for (Square64 sq64 = SQ64_A1; sq64 < SQUARE_SIZE_64; ++sq64) {
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

    Score score = position.get_material_score(WHITE) - position.get_material_score(BLACK);
    score += eval_pawn_structure(position);
    return score * (~position.get_side_to_move() - position.get_side_to_move());

}

Score calc_material_table(Piece piece, Square64 square){
    return material_table[piece][square];
}

Score eval_pawn_structure(const Position& pos) {
    
    Score score = 0;

    const Bitboard wPawns = pos.get_pieceTypes_bitboard(WHITE, PAWN);
    const Bitboard bPawns = pos.get_pieceTypes_bitboard(BLACK, PAWN);

    Bitboard itWPawns = wPawns;
    
    while (itWPawns) {
        Square64 sq64{__builtin_ctzll(itWPawns)}; 
        if ((isolated_masks[sq64] & wPawns) == ZERO)
            score -= ISOLATED_PAWN_PENALTY;
        if ((passed_masks[WHITE][sq64] & bPawns) == ZERO) {
            Rank rank = square_rank(sq64);                 
            score += PASSED_PAWN_BONUS_BY_RANK[rank];
        }
        itWPawns &= itWPawns - 1;
    }

    Bitboard itBPawns = bPawns;
    
    while (itBPawns) {
        Square64 sq64{__builtin_ctzll(itBPawns)}; 
        if ((isolated_masks[sq64] & bPawns) == ZERO)
            score += ISOLATED_PAWN_PENALTY;  
        if ((passed_masks[BLACK][sq64] & wPawns) == ZERO) {
            Rank rank = square_rank(sq64);                 
            score -= PASSED_PAWN_BONUS_BY_RANK[7 - rank];   
        } 
        itBPawns &= itBPawns - 1;
    }

    return score;
}

int flip(int sq){
    return sq^56;
}

}

}