#ifndef INCLUDE_BITBOARD_H
#define INCLUDE_BITBOARD_H

#include "types.h"

namespace Xake{


using Bitboard = unsigned long int;

constexpr Bitboard ONE = 1;
constexpr Bitboard ZERO = 0;

namespace Bitboards {

    void set_piece(Bitboard &bitboard, Square64 square64);
    void clear_piece(Bitboard &bitboard, Square64 square64);
    void move_piece(Bitboard &bitboard, Square64 from, Square64 to);
    
};


} // namespace Xake

#endif // #ifndef INCLUDE_BITBOARD_H



