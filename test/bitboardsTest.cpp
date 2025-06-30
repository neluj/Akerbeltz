#include <gtest/gtest.h>
#include "bitboards.h"

using namespace Xake;

TEST(BitboardTest, SetPiece){                               

    Bitboard bitboard = ZERO;

    Bitboards::set_piece(bitboard, SQ64_C2);
    Bitboard expected = 0b10000000000;

    EXPECT_EQ(bitboard, expected);

    Bitboards::set_piece(bitboard, SQ64_C2);
    expected = 0b10000000000;

    EXPECT_EQ(bitboard, expected);

    Bitboards::set_piece(bitboard, SQ64_A3);
    expected = 0b10000010000000000;

    EXPECT_EQ(bitboard, expected);

}

TEST(BitboardTest, ClearPiece){     
    
    Bitboard bitboard = 0b10000010000000000;

    Bitboards::clear_piece(bitboard, SQ64_C2);
    Bitboard expected = 0b10000000000000000;

    EXPECT_EQ(bitboard, expected);

    Bitboards::clear_piece(bitboard, SQ64_A3);
    expected = ZERO;

    EXPECT_EQ(bitboard, expected);   

}

TEST(BitboardTest, movePiece){   
    
    Bitboard bitboard = 0b10000000000000010;

    Bitboards::move_piece(bitboard, SQ64_A3, SQ64_C2);
    Bitboard expected = 0b10000000010;

    EXPECT_EQ(bitboard, expected);

}
