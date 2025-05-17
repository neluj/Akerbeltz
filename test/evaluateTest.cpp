#include <gtest/gtest.h>
#include "evaluate.h"
#include "position.h"

using namespace Xake;

class EvaluateTest : public ::testing::Test
{
     protected:
     virtual void SetUp()
     {      
         Evaluate::init(); 
     }

     virtual void TearDown()
    {
    }

    public:
    
    Position pos;
    const std::string START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

};

TEST_F(EvaluateTest, MaterialTable){

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

    EXPECT_EQ(Evaluate::calc_material_table(WHITE, PieceType::KING, SQ120_D5), 19973);
    EXPECT_EQ(Evaluate::calc_material_table(BLACK, PieceType::KING, SQ120_D4), 19973);

}

TEST_F(EvaluateTest, Score){

    pos.set_FEN(START_FEN);

    EXPECT_EQ(0, Evaluate::calc_score(pos));

    pos.do_move(make_quiet_move(SQ120_D2, SQ120_D4, SpecialMove::NO_SPECIAL));

    //Value is negative because side to move has move to black
    EXPECT_EQ(-35, Evaluate::calc_score(pos));


}

//TODO castles and promotions