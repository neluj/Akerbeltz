#include <gtest/gtest.h>
#include "bitboards.h"

using namespace Xake;
using namespace Bitboards;

TEST(BitboardTest, SetPieces){                               

    Bitboard bitboard = set_pieces(SQ64_C2);
    Bitboard expected = 0b10000000000;

    EXPECT_EQ(bitboard, expected);

    bitboard =  set_pieces(bitboard, SQ64_C2);

    EXPECT_EQ(bitboard, expected);

    bitboard = set_pieces(bitboard, SQ64_A3);
    expected = 0b10000010000000000;

    EXPECT_EQ(bitboard, expected);

    bitboard = set_pieces(SQ64_C2, SQ64_A3);
    EXPECT_EQ(bitboard, expected);

}

TEST(BitboardTest, ClearPieces){     
    
    Bitboard bitboard = 0b10000010000000000;

    bitboard = clear_pieces(bitboard, SQ64_C2);
    Bitboard expected = 0b10000000000000000;

    EXPECT_EQ(bitboard, expected);

    bitboard = clear_pieces(bitboard, SQ64_A3);
    expected = ZERO;

    EXPECT_EQ(bitboard, expected);   

    //Try to clean a piece that does not exist 
    bitboard = clear_pieces(bitboard, SQ64_A3);
    expected = ZERO;

    EXPECT_EQ(bitboard, expected); 
    
    bitboard = 0b10000010000000000;

    bitboard = clear_pieces(bitboard, SQ64_C2, SQ64_A3);
    EXPECT_EQ(bitboard, ZERO); 

}


