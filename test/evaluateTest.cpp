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

    EXPECT_EQ(Evaluate::calc_material_table(Piece::NO_PIECE, SQ64_A1), 0);
    EXPECT_EQ(Evaluate::calc_material_table(Piece::NO_PIECE, SQ64_A1), 0);


    EXPECT_EQ(Evaluate::calc_material_table(Piece::W_PAWN, SQ64_A1), 82);
    EXPECT_EQ(Evaluate::calc_material_table(Piece::W_PAWN, SQ64_B1), 82);
    EXPECT_EQ(Evaluate::calc_material_table(Piece::W_PAWN, SQ64_A2), 47);
    EXPECT_EQ(Evaluate::calc_material_table(Piece::W_PAWN, SQ64_B7), 216);

    EXPECT_EQ(Evaluate::calc_material_table(Piece::B_PAWN, SQ64_A8), 82);
    EXPECT_EQ(Evaluate::calc_material_table(Piece::B_PAWN, SQ64_B8), 82);
    EXPECT_EQ(Evaluate::calc_material_table(Piece::B_PAWN, SQ64_A7), 47);
    EXPECT_EQ(Evaluate::calc_material_table(Piece::B_PAWN, SQ64_B2), 216);

    EXPECT_EQ(Evaluate::calc_material_table(Piece::W_KING, SQ64_D5), 19973);
    EXPECT_EQ(Evaluate::calc_material_table(Piece::B_KING, SQ64_D4), 19973);

}

TEST_F(EvaluateTest, Score){

    pos.set_FEN(START_FEN);

    EXPECT_EQ(0, Evaluate::calc_score(pos));

    pos.do_move(make_quiet_move(SQ64_D2, SQ64_D4, SpecialMove::NO_SPECIAL));

    //Value is negative because side to move has move to black
    EXPECT_EQ(-35, Evaluate::calc_score(pos));

}

//TODO castles and promotions