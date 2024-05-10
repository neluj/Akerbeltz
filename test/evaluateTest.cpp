#include <gtest/gtest.h>
#include "evaluate.h"

using namespace Xake;

TEST(EvaluateTest, MaterialTableScore){

    Evaluate::init();

    EXPECT_EQ(Evaluate::calc_material_table(WHITE, PieceType::NO_PIECE_TYPE, SQ120_A1), 0);
    EXPECT_EQ(Evaluate::calc_material_table(BLACK, PieceType::NO_PIECE_TYPE, SQ120_A1), 0);


    EXPECT_EQ(Evaluate::calc_material_table(WHITE, PieceType::PAWN, SQ120_A1), 82);
    EXPECT_EQ(Evaluate::calc_material_table(WHITE, PieceType::PAWN, SQ120_B1), 82);
    EXPECT_EQ(Evaluate::calc_material_table(WHITE, PieceType::PAWN, SQ120_A2), 47);
    EXPECT_EQ(Evaluate::calc_material_table(WHITE, PieceType::PAWN, SQ120_B7), 216);

    EXPECT_EQ(Evaluate::calc_material_table(BLACK, PieceType::PAWN, SQ120_A8), 82);
    EXPECT_EQ(Evaluate::calc_material_table(BLACK, PieceType::PAWN, SQ120_B8), 82);
    EXPECT_EQ(Evaluate::calc_material_table(BLACK, PieceType::PAWN, SQ120_A7), 47);
    EXPECT_EQ(Evaluate::calc_material_table(BLACK, PieceType::PAWN, SQ120_B2), 216);

}