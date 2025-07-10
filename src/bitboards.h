#ifndef INCLUDE_BITBOARD_H
#define INCLUDE_BITBOARD_H

#include "types.h"

namespace Xake{


using Bitboard = unsigned long int;

constexpr Bitboard ONE = 1;
constexpr Bitboard ZERO = 0;

namespace Bitboards {

    const Bitboard FILE_A_MASK = 0x0101010101010101ULL;
    const Bitboard FILE_B_MASK = 0x0202020202020202ULL;
    const Bitboard FILE_G_MASK = 0x4040404040404040ULL;
    const Bitboard FILE_H_MASK = 0x8080808080808080ULL;
    const Bitboard RANK_1_MASK = 0x00000000000000FFULL;
    const Bitboard RANK_2_MASK = 0x000000000000FF00ULL;
    const Bitboard RANK_7_MASK = 0x00FF000000000000ULL;
    const Bitboard RANK_8_MASK = 0xFF00000000000000ULL;


    template<typename... Squares>
    inline Bitboard set_pieces(Squares... squares) {
        return ZERO | ((ONE << squares) | ...);
    }

    template<typename... Squares>
    inline Bitboard set_pieces(Bitboard bitboard, Squares... squares) {
        return bitboard | ((ONE << squares) | ...);
    }

    template<typename... Squares>
    inline Bitboard clear_pieces(Bitboard bitboard, Squares... squares) {
        return (bitboard & ... & ~(ONE << squares));
    }

    template<typename... Masks>
    inline Bitboard clear_masks(Bitboard bitboard, Masks... masks) {
        return bitboard & ~(... | masks);
    }
    //TODO revisar
    template<Direction D>
    constexpr Bitboard make_direction(Square64 sq64){

        return D == NORTH            ? clear_masks(set_pieces(sq64 + D), RANK_1_MASK)
             : D == NORTH_NORTH      ? clear_masks(set_pieces(sq64 + D), RANK_1_MASK, RANK_2_MASK)    
             : D == SOUTH            ? clear_masks(set_pieces(sq64 + D), RANK_8_MASK)    
             : D == SOUTH_SOUTH      ? clear_masks(set_pieces(sq64 + D), RANK_8_MASK, RANK_7_MASK)    
             : D == EAST             ? clear_masks(set_pieces(sq64 + D), FILE_A_MASK)    
             : D == WEST             ? clear_masks(set_pieces(sq64 + D), FILE_H_MASK)    
             : D == NORTH_EAST       ? clear_masks(set_pieces(sq64 + D), FILE_A_MASK, RANK_1_MASK)    
             : D == NORTH_WEST       ? clear_masks(set_pieces(sq64 + D), FILE_H_MASK, RANK_1_MASK)    
             : D == SOUTH_EAST       ? clear_masks(set_pieces(sq64 + D), FILE_A_MASK, RANK_8_MASK)    
             : D == SOUTH_WEST       ? clear_masks(set_pieces(sq64 + D), FILE_H_MASK, RANK_8_MASK)    
             : D == NORTH_NORTH_WEST ? clear_masks(set_pieces(sq64 + D), RANK_1_MASK, RANK_2_MASK, FILE_H_MASK)    
             : D == NORTH_NORTH_EAST ? clear_masks(set_pieces(sq64 + D), RANK_1_MASK, RANK_2_MASK, FILE_A_MASK)    
             : D == NORTH_WEST_WEST  ? clear_masks(set_pieces(sq64 + D), RANK_1_MASK, FILE_G_MASK, FILE_H_MASK)    
             : D == NORTH_EAST_EAST  ? clear_masks(set_pieces(sq64 + D), RANK_1_MASK, FILE_A_MASK, FILE_B_MASK)    
             : D == SOUTH_WEST_WEST  ? clear_masks(set_pieces(sq64 + D), RANK_8_MASK, FILE_G_MASK, FILE_H_MASK)    
             : D == SOUTH_EAST_EAST  ? clear_masks(set_pieces(sq64 + D), RANK_8_MASK, FILE_A_MASK, FILE_B_MASK)    
             : D == SOUTH_SOUTH_WEST ? clear_masks(set_pieces(sq64 + D), RANK_8_MASK, RANK_7_MASK, FILE_H_MASK)    
             : D == SOUTH_SOUTH_EAST ? clear_masks(set_pieces(sq64 + D), RANK_8_MASK, RANK_7_MASK, FILE_A_MASK)
                                     : ZERO;

    }

};


} // namespace Xake

#endif // #ifndef INCLUDE_BITBOARD_H



