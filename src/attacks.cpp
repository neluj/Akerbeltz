#include "attacks.h"

namespace Xake{

namespace Attacks {

Bitboard pawnAttacks[COLOR_SIZE][SQUARE_SIZE_64];
Bitboard knightAttacks[SQUARE_SIZE_64];
Bitboard kingAttacks[SQUARE_SIZE_64];
Bitboard slidingSideRays[SIDE_ATTACK_DIR_SIZE][SQUARE_SIZE_64];
Bitboard slidingDiagonalRays[DIAGONAL_ATTACK_DIR_SIZE][SQUARE_SIZE_64];

void init_pawn_attacks();
void init_knight_attacks();
void init_king_attacks();
void init_sliding_side_attacks();
void init_sliding_diagonal_attacks();

void init(){

    init_pawn_attacks();
    init_knight_attacks();
    init_king_attacks();
    init_sliding_side_attacks();
    init_sliding_diagonal_attacks();
    
}
//TODO unify as non sliding attacks
void init_pawn_attacks(){

    for(Square64 sq64 = SQ64_A1; sq64 < SQUARE_SIZE_64; ++sq64)
    {

        pawnAttacks[WHITE][sq64] = Bitboards::make_direction<NORTH_WEST>(Bitboards::set_pieces(sq64)) 
                                 | Bitboards::make_direction<NORTH_EAST>(Bitboards::set_pieces(sq64)); 

        pawnAttacks[BLACK][sq64] = Bitboards::make_direction<SOUTH_WEST>(Bitboards::set_pieces(sq64)) 
                                 | Bitboards::make_direction<SOUTH_EAST>(Bitboards::set_pieces(sq64)); 
    }
    
}

void init_knight_attacks(){

    for(Square64 sq64 = SQ64_A1; sq64 < SQUARE_SIZE_64; ++sq64)
    {

        knightAttacks[sq64] = Bitboards::make_direction<NORTH_NORTH_WEST>(Bitboards::set_pieces(sq64))
                            | Bitboards::make_direction<NORTH_NORTH_EAST>(Bitboards::set_pieces(sq64))
                            | Bitboards::make_direction<NORTH_EAST_EAST> (Bitboards::set_pieces(sq64))
                            | Bitboards::make_direction<SOUTH_EAST_EAST> (Bitboards::set_pieces(sq64))
                            | Bitboards::make_direction<SOUTH_SOUTH_EAST>(Bitboards::set_pieces(sq64))
                            | Bitboards::make_direction<SOUTH_SOUTH_WEST>(Bitboards::set_pieces(sq64))
                            | Bitboards::make_direction<SOUTH_WEST_WEST> (Bitboards::set_pieces(sq64))
                            | Bitboards::make_direction<NORTH_WEST_WEST> (Bitboards::set_pieces(sq64))
                            ;
    }

}

void init_king_attacks(){

    for(Square64 sq64 = SQ64_A1; sq64 < SQUARE_SIZE_64; ++sq64)
    {

        kingAttacks[sq64] = Bitboards::make_direction<NORTH>      (Bitboards::set_pieces(sq64))
                          | Bitboards::make_direction<SOUTH>      (Bitboards::set_pieces(sq64))
                          | Bitboards::make_direction<EAST>       (Bitboards::set_pieces(sq64))
                          | Bitboards::make_direction<WEST>       (Bitboards::set_pieces(sq64))
                          | Bitboards::make_direction<NORTH_EAST> (Bitboards::set_pieces(sq64))
                          | Bitboards::make_direction<NORTH_WEST> (Bitboards::set_pieces(sq64))
                          | Bitboards::make_direction<SOUTH_EAST> (Bitboards::set_pieces(sq64))
                          | Bitboards::make_direction<SOUTH_WEST> (Bitboards::set_pieces(sq64))
                          ;
    }

}

void init_sliding_side_attacks(){

    for(Square64 sq64 = SQ64_A1; sq64 < SQUARE_SIZE_64; ++sq64)
    {
        slidingSideRays[NORTH_ATTACK][sq64] = 0x0101010101010100ULL << sq64;          
        slidingSideRays[SOUTH_ATTACK][sq64] = 0x0080808080808080ULL >> (63 - sq64);
        slidingSideRays[EAST_ATTACK][sq64] = 2 * ((ONE << (sq64 | 7)) - (ONE << sq64));
        slidingSideRays[WEST_ATTACK][sq64] = ((ONE << sq64) - (ONE << (sq64 & 56)));;

    }                         

}

void init_sliding_diagonal_attacks(){

    auto westN = [](Bitboard board, int n) {
        for (int i = 0; i < n; ++i)
            board = (board >> 1) & ~Bitboards::FILE_H_MASK;
        return board;
    };

    auto eastN = [](Bitboard board, int n) {
        for (int i = 0; i < n; ++i)
            board = (board << 1) & ~Bitboards::FILE_A_MASK;
        return board;
    };

    for (Rank rank = RANK_8; rank >= RANK_1; --rank)
    {
        for (File file = FILE_A; file <= FILE_H; ++file)
        {

            Square64 sq64 = make_square64(rank, file);

            slidingDiagonalRays[NORTH_EAST_ATTACK][sq64] = eastN(0x8040201008040200ULL, file)    << (rank * 8);
            slidingDiagonalRays[NORTH_WEST_ATTACK][sq64] = westN(0x102040810204000ULL, 7 - file) << (rank * 8); 
            slidingDiagonalRays[SOUTH_EAST_ATTACK][sq64] = eastN(0x2040810204080ULL, file)       >> ((7 - rank) * 8);
            slidingDiagonalRays[SOUTH_WEST_ATTACK][sq64] = westN(0x40201008040201ULL, 7 - file)  >> ((7 - rank) * 8);

        }
    }
                       
}


Bitboard sliding_side_attacks(Bitboard occupied,Square64 sq64){

    //North
    Bitboard bloquersN = occupied & slidingSideRays[NORTH_ATTACK][sq64];
    Square64 sq64BloquerN{__builtin_ctzll(bloquersN)};
    Bitboard nAttacks = slidingSideRays[NORTH_ATTACK][sq64] & ~slidingSideRays[NORTH_ATTACK][sq64BloquerN];

    //South
    Bitboard bloquersS = occupied & slidingSideRays[SOUTH_ATTACK][sq64];
    Square64 sq64BloquerS{SQ64_H8 - __builtin_clzl(bloquersS)};
    Bitboard sAttacks = slidingSideRays[SOUTH_ATTACK][sq64] & ~slidingSideRays[SOUTH_ATTACK][sq64BloquerS];

    //East
    Bitboard bloquersE = occupied & slidingSideRays[EAST_ATTACK][sq64];
    Square64 sq64BloquerE{__builtin_ctzll(bloquersE)};
    Bitboard eAttacks = slidingSideRays[EAST_ATTACK][sq64] & ~slidingSideRays[EAST_ATTACK][sq64BloquerE];

    //West
    Bitboard bloquersW = occupied & slidingSideRays[WEST_ATTACK][sq64];
    Square64 sq64BloquerW{SQ64_H8 - __builtin_clzl(bloquersW)};
    Bitboard wAttacks = slidingSideRays[WEST_ATTACK][sq64] & ~slidingSideRays[WEST_ATTACK][sq64BloquerW];
 
    return nAttacks | sAttacks | eAttacks | wAttacks;

}


Bitboard sliding_diagonal_attacks(Bitboard occupied,Square64 sq64){

    //North East
    Bitboard bloquersNE = occupied & slidingDiagonalRays[NORTH_EAST_ATTACK][sq64];
    Square64 sq64BloquerNE{__builtin_ctzll(bloquersNE)};
    Bitboard neAttacks = slidingDiagonalRays[NORTH_EAST_ATTACK][sq64] & ~slidingDiagonalRays[NORTH_EAST_ATTACK][sq64BloquerNE];

    //South East
    Bitboard bloquersSE = occupied & slidingDiagonalRays[SOUTH_EAST_ATTACK][sq64];
    Square64 sq64BloquerSE{SQ64_H8 - __builtin_clzl(bloquersSE)};
    Bitboard seAttacks = slidingDiagonalRays[SOUTH_EAST_ATTACK][sq64] & ~slidingDiagonalRays[SOUTH_EAST_ATTACK][sq64BloquerSE];

    //North West
    Bitboard bloquersNW = occupied & slidingDiagonalRays[NORTH_WEST_ATTACK][sq64];
    Square64 sq64BloquerNW{__builtin_ctzll(bloquersNW)};
    Bitboard nwAttacks = slidingDiagonalRays[NORTH_WEST_ATTACK][sq64] & ~slidingDiagonalRays[NORTH_WEST_ATTACK][sq64BloquerNW];

    //South West
    Bitboard bloquersSW = occupied & slidingDiagonalRays[SOUTH_WEST_ATTACK][sq64];
    Square64 sq64BloquerSW{SQ64_H8 - __builtin_clzl(bloquersSW)};
    Bitboard swAttacks = slidingDiagonalRays[SOUTH_WEST_ATTACK][sq64] & ~slidingDiagonalRays[SOUTH_WEST_ATTACK][sq64BloquerSW];

    return neAttacks | seAttacks | nwAttacks | swAttacks;

}

}

}