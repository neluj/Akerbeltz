#include <gtest/gtest.h>
#include "attacks.h"

using namespace Xake;
using namespace Bitboards;
using namespace Attacks;


TEST(AttacksTest, PawnAttacks){                               

    init();
    
    //A8
    EXPECT_EQ(pawnAttacks[WHITE][SQ64_A8], ZERO);

    Bitboard pwAttacksBlA8 = set_pieces(SQ64_B7);
    EXPECT_EQ(pawnAttacks[BLACK][SQ64_A8], pwAttacksBlA8);

    //H8
    EXPECT_EQ(pawnAttacks[WHITE][SQ64_H8], ZERO);

    Bitboard pwAttacksBlH8 = set_pieces(SQ64_G7);
    EXPECT_EQ(pawnAttacks[BLACK][SQ64_H8], pwAttacksBlH8);

    //H1
    Bitboard pwAttacksWtH1 = set_pieces(SQ64_G2);
    EXPECT_EQ(pawnAttacks[WHITE][SQ64_H1], pwAttacksWtH1);

    EXPECT_EQ(pawnAttacks[BLACK][SQ64_H1], ZERO);

    //A1
    Bitboard pwAttacksWtA1 = set_pieces(SQ64_B2);
    EXPECT_EQ(pawnAttacks[WHITE][SQ64_A1], pwAttacksWtA1);

    EXPECT_EQ(pawnAttacks[BLACK][SQ64_A1], ZERO);

    //A4
    Bitboard pwAttacksWtA4 = set_pieces(SQ64_B5);
    EXPECT_EQ(pawnAttacks[WHITE][SQ64_A4], pwAttacksWtA4);

    Bitboard pwAttacksBlA4 = set_pieces(SQ64_B3);
    EXPECT_EQ(pawnAttacks[BLACK][SQ64_A4], pwAttacksBlA4);

    //D8
    EXPECT_EQ(pawnAttacks[WHITE][SQ64_D8], ZERO);

    Bitboard pwAttacksBlD8 = set_pieces(SQ64_C7, SQ64_E7);
    EXPECT_EQ(pawnAttacks[BLACK][SQ64_D8], pwAttacksBlD8);

    //H4
    Bitboard pwAttacksWtH4 = set_pieces(SQ64_G5);
    EXPECT_EQ(pawnAttacks[WHITE][SQ64_H4], pwAttacksWtH4);

    Bitboard pwAttacksBlH4 = set_pieces(SQ64_G3);
    EXPECT_EQ(pawnAttacks[BLACK][SQ64_H4], pwAttacksBlH4);

    //D1
    Bitboard pwAttacksWtD1 = set_pieces(SQ64_C2, SQ64_E2);
    EXPECT_EQ(pawnAttacks[WHITE][SQ64_D1], pwAttacksWtD1);

    EXPECT_EQ(pawnAttacks[BLACK][SQ64_D1], ZERO);

    //D4
    Bitboard pwAttacksWtD4 = set_pieces(SQ64_C5, SQ64_E5);
    EXPECT_EQ(pawnAttacks[WHITE][SQ64_D4], pwAttacksWtD4);

    Bitboard pwAttacksBlD4 = set_pieces(SQ64_C3, SQ64_E3);
    EXPECT_EQ(Attacks::pawnAttacks[BLACK][SQ64_D4], pwAttacksBlD4);

}


TEST(AttacksTest, KnigthAttacks){ 

    init();
    
    //A8
    Bitboard khAttacksA8 = set_pieces(SQ64_B6, SQ64_C7);
    EXPECT_EQ(knightAttacks[SQ64_A8], khAttacksA8);

    //H8
    Bitboard khAttacksH8 = set_pieces(SQ64_G6, SQ64_F7);
    EXPECT_EQ(knightAttacks[SQ64_H8], khAttacksH8);

    //H1
    Bitboard khAttacksH1 = set_pieces(SQ64_G3, SQ64_F2);
    EXPECT_EQ(knightAttacks[SQ64_H1], khAttacksH1);

    //A1
    Bitboard khAttacksA1 = set_pieces(SQ64_B3, SQ64_C2);
    EXPECT_EQ(knightAttacks[SQ64_A1], khAttacksA1);

    //A4
    Bitboard khAttacksA4 = set_pieces(SQ64_B6, SQ64_C5, SQ64_C3, SQ64_B2);
    EXPECT_EQ(knightAttacks[SQ64_A4], khAttacksA4);

    //D8
    Bitboard khAttacksD8 = set_pieces(SQ64_B7, SQ64_C6, SQ64_E6, SQ64_F7);
    EXPECT_EQ(knightAttacks[SQ64_D8], khAttacksD8);

    //H4
    Bitboard khAttacksH4 = set_pieces(SQ64_G6, SQ64_F5, SQ64_F3, SQ64_G2);
    EXPECT_EQ(knightAttacks[SQ64_H4], khAttacksH4);

    //D1
    Bitboard khAttacksD1 = set_pieces(SQ64_B2, SQ64_C3, SQ64_E3, SQ64_F2);
    EXPECT_EQ(knightAttacks[SQ64_D1], khAttacksD1);

    //D4
    Bitboard khAttacksD4 = set_pieces(SQ64_C6, SQ64_E6, SQ64_F5, SQ64_F3, SQ64_E2, SQ64_C2, SQ64_B3, SQ64_B5);
    EXPECT_EQ(knightAttacks[SQ64_D4], khAttacksD4);

}

TEST(AttacksTest, KingAttacks){ 

    init();
    
    //A8
    Bitboard kgAttacksA8 = set_pieces(SQ64_A7, SQ64_B7, SQ64_B8);
    EXPECT_EQ(kingAttacks[SQ64_A8], kgAttacksA8);

    //H8
    Bitboard kgAttacksH8 = set_pieces(SQ64_H7, SQ64_G7, SQ64_G8);
    EXPECT_EQ(kingAttacks[SQ64_H8], kgAttacksH8);

    //H1
    Bitboard kgAttacksH1 = set_pieces(SQ64_H2, SQ64_G2, SQ64_G1);
    EXPECT_EQ(kingAttacks[SQ64_H1], kgAttacksH1);

    //A1
    Bitboard kgAttacksA1 = set_pieces(SQ64_A2, SQ64_B2, SQ64_B1);
    EXPECT_EQ(kingAttacks[SQ64_A1], kgAttacksA1);

    //A4
    Bitboard kgAttacksA4 = set_pieces(SQ64_A5, SQ64_B5, SQ64_B4, SQ64_B3, SQ64_A3);
    EXPECT_EQ(kingAttacks[SQ64_A4], kgAttacksA4);
  
    //D8
    Bitboard kgAttacksD8 = set_pieces(SQ64_C8, SQ64_C7, SQ64_D7, SQ64_E7, SQ64_E8);
    EXPECT_EQ(kingAttacks[SQ64_D8], kgAttacksD8);

    //H4
    Bitboard kgAttacksH4 = set_pieces(SQ64_H5, SQ64_G5, SQ64_G4, SQ64_G3, SQ64_H3);
    EXPECT_EQ(kingAttacks[SQ64_H4], kgAttacksH4);

    //D1
    Bitboard kgAttacksD1 = set_pieces(SQ64_C1, SQ64_C2, SQ64_D2, SQ64_E2, SQ64_E1);
    EXPECT_EQ(kingAttacks[SQ64_D1], kgAttacksD1);

    //D4
    Bitboard kgAttacksD4 = set_pieces(SQ64_C5, SQ64_D5, SQ64_E5, SQ64_E4, SQ64_E3, SQ64_D3, SQ64_C3, SQ64_C4);
    EXPECT_EQ(kingAttacks[SQ64_D4], kgAttacksD4);

}

//DELETEME
TEST(AttacksTest, SlidingAttacks){ 

    init();
    Bitboard occupied = set_pieces(SQ64_F6, SQ64_B2, SQ64_D2, SQ64_A4, SQ64_G4, SQ64_A7, SQ64_C3, SQ64_F2);
    sliding_diagonal_attacks(occupied, SQ64_D4);

}