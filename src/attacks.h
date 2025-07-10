#ifndef INCLUDE_ATTACKS_H
#define INCLUDE_ATTACKS_H

#include "bitboards.h"

namespace Xake{


namespace Attacks {

    enum SIDE_ATTACK_DIRECTION{
        NORTH_ATTACK,
        EAST_ATTACK,
        SOUTH_ATTACK,
        WEST_ATTACK,
        SIDE_ATTACK_DIR_SIZE
    };

    enum DIAGONAL_ATTACK_DIRECTION{
        NORTH_EAST_ATTACK,
        NORTH_WEST_ATTACK,
        SOUTH_EAST_ATTACK,
        SOUTH_WEST_ATTACK,
        DIAGONAL_ATTACK_DIR_SIZE
    };

    extern Bitboard pawnAttacks[COLOR_SIZE][SQUARE_SIZE_64];
    extern Bitboard knightAttacks[SQUARE_SIZE_64];
    extern Bitboard kingAttacks[SQUARE_SIZE_64];
    
    void init();
    Bitboard sliding_side_attacks(Bitboard occupied,Square64 sq64);
    Bitboard sliding_diagonal_attacks(Bitboard occupied,Square64 sq64);
};  

} // namespace Xake

#endif // #ifndef INCLUDE_ATTACKS_H