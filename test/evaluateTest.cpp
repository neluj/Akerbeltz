#include <gtest/gtest.h>
#include "evaluate.h"
#include "position.h"

using namespace Xake;
using namespace Evaluate;

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

    EXPECT_EQ(calc_material_table(Piece::NO_PIECE, SQ64_A1), 0);
    EXPECT_EQ(calc_material_table(Piece::NO_PIECE, SQ64_A1), 0);


    EXPECT_EQ(calc_material_table(Piece::W_PAWN, SQ64_A1), 82);
    EXPECT_EQ(calc_material_table(Piece::W_PAWN, SQ64_B1), 82);
    EXPECT_EQ(calc_material_table(Piece::W_PAWN, SQ64_A2), 47);
    EXPECT_EQ(calc_material_table(Piece::W_PAWN, SQ64_B7), 216);

    EXPECT_EQ(calc_material_table(Piece::B_PAWN, SQ64_A8), 82);
    EXPECT_EQ(calc_material_table(Piece::B_PAWN, SQ64_B8), 82);
    EXPECT_EQ(calc_material_table(Piece::B_PAWN, SQ64_A7), 47);
    EXPECT_EQ(calc_material_table(Piece::B_PAWN, SQ64_B2), 216);

    EXPECT_EQ(calc_material_table(Piece::W_KING, SQ64_D5), 19973);
    EXPECT_EQ(calc_material_table(Piece::B_KING, SQ64_D4), 19973);

}

TEST_F(EvaluateTest, Score){

    pos.set_FEN(START_FEN);

    EXPECT_EQ(0, calc_score(pos));

    pos.do_move(make_quiet_move(SQ64_D2, SQ64_D4, SpecialMove::NO_SPECIAL));

    //Value is negative because side to move has move to black
    EXPECT_EQ(-35, calc_score(pos));

}

TEST_F(EvaluateTest, PassedPawnDeltaExact) {

    const std::string FEN_PASSED =     "4k3/8/8/8/3P4/2p5/8/4K3 w - - 0 1";

    const std::string FEN_NOT_PASSED = "4k3/8/8/2p5/3P4/8/8/4K3 w - - 0 1";

    Position p_passed, p_not;
    p_passed.set_FEN(FEN_PASSED);
    p_not.set_FEN(FEN_NOT_PASSED);

    const Score s_passed = Evaluate::calc_score(p_passed);
    const Score s_not    = Evaluate::calc_score(p_not);

    const Score delta_b_pawn =
        Evaluate::calc_material_table(Piece::B_PAWN, SQ64_C5) -
        Evaluate::calc_material_table(Piece::B_PAWN, SQ64_C3);


    constexpr Score WHITE_PASSED_R3 = 35;
    constexpr Score BLACK_PASSED_R5 = 90;

    EXPECT_EQ(s_passed - s_not, WHITE_PASSED_R3 - BLACK_PASSED_R5 + delta_b_pawn);
}

TEST_F(EvaluateTest, IsolatedPawnDeltaExact) {

    const std::string FEN_ISOLATED =     "4k3/8/8/8/2P5/8/8/P3K3 w - - 0 1";

    const std::string FEN_NOT_ISOLATED = "4k3/8/8/8/2P5/8/8/1P2K3 w - - 0 1";

    Position p_iso, p_not;
    p_iso.set_FEN(FEN_ISOLATED);
    p_not.set_FEN(FEN_NOT_ISOLATED);

    const Score s_iso = Evaluate::calc_score(p_iso);
    const Score s_not = Evaluate::calc_score(p_not);


    const Score delta_w_pawn =
        Evaluate::calc_material_table(Piece::W_PAWN, SQ64_B1) -
        Evaluate::calc_material_table(Piece::W_PAWN, SQ64_A1);


    constexpr Score ISOLATED_PAWN_PENALTY = 15;


    EXPECT_EQ(s_not - s_iso, 2 * ISOLATED_PAWN_PENALTY + delta_w_pawn);
}

TEST_F(EvaluateTest, OpenVsSemiOpen_RookBlack_DeltaExact) {

    const std::string FEN_SEMIOPEN = "4k3/8/8/8/3r4/3P4/8/3K4 b - - 0 1";
    const std::string FEN_OPEN =     "4k3/8/8/8/3r4/4P3/8/3K4 b - - 0 1";

    Position p_open, p_semi;
    p_open.set_FEN(FEN_OPEN);
    p_semi.set_FEN(FEN_SEMIOPEN);

    const Score s_open = Evaluate::calc_score(p_open);
    const Score s_semi = Evaluate::calc_score(p_semi);

    const Score delta_w_pawn =
        Evaluate::calc_material_table(Piece::W_PAWN, SQ64_E3) -
        Evaluate::calc_material_table(Piece::W_PAWN, SQ64_D3);

    constexpr Score ROOK_OPEN_FILE_BONUS     = 20;
    constexpr Score ROOK_SEMIOPEN_FILE_BONUS = 12;

    EXPECT_EQ(s_open - s_semi, (ROOK_OPEN_FILE_BONUS - ROOK_SEMIOPEN_FILE_BONUS) - delta_w_pawn);
}

TEST_F(EvaluateTest, OpenVsSemiOpen_QueenBlack_DeltaExact) {

    const std::string FEN_SEMIOPEN = "4k3/8/8/8/3q4/3P4/8/3K4 b - - 0 1";
    const std::string FEN_OPEN =     "4k3/8/8/8/3q4/4P3/8/3K4 b - - 0 1";

    Position p_open, p_semi;
    p_open.set_FEN(FEN_OPEN);
    p_semi.set_FEN(FEN_SEMIOPEN);

    const Score s_open = Evaluate::calc_score(p_open);
    const Score s_semi = Evaluate::calc_score(p_semi);

    const Score delta_w_pawn =
        Evaluate::calc_material_table(Piece::W_PAWN, SQ64_E3) -
        Evaluate::calc_material_table(Piece::W_PAWN, SQ64_D3);

    constexpr Score QUEEN_OPEN_FILE_BONUS     = 6;
    constexpr Score QUEEN_SEMIOPEN_FILE_BONUS = 3;

    EXPECT_EQ(s_open - s_semi, (QUEEN_OPEN_FILE_BONUS - QUEEN_SEMIOPEN_FILE_BONUS) - delta_w_pawn);
}

TEST_F(EvaluateTest, MaterialDraws){

    const std::string FEN_KK   = "4k3/8/8/8/8/8/8/4K3 w - - 0 1";
    const std::string FEN_KBK  = "4k3/8/8/8/8/8/8/3BK3 w - - 0 1";
    const std::string FEN_KNK  = "4k3/8/8/8/8/8/8/3NK3 w - - 0 1";
    const std::string FEN_KNNK = "4k3/8/8/8/8/8/8/2NNK3 w - - 0 1";
    const std::string FEN_KBKB = "4kb2/8/8/8/8/8/8/2B1K3 w - - 0 1";
    const std::string FEN_KBNK = "4k3/8/8/8/8/8/8/2B1K1N1 w - - 0 1";
    const std::string FEN_KRK  = "4k3/8/8/8/8/8/8/3RK3 w - - 0 1";

    Position position;

    position.set_FEN(FEN_KK);
    EXPECT_TRUE(Evaluate::material_draw(position));

    position.set_FEN(FEN_KBK);
    EXPECT_TRUE(Evaluate::material_draw(position));

    position.set_FEN(FEN_KNK);
    EXPECT_TRUE(Evaluate::material_draw(position));

    position.set_FEN(FEN_KNNK);
    EXPECT_TRUE(Evaluate::material_draw(position));

    position.set_FEN(FEN_KBKB);
    EXPECT_TRUE(Evaluate::material_draw(position));

    position.set_FEN(FEN_KBNK);
    EXPECT_FALSE(Evaluate::material_draw(position));

    position.set_FEN(FEN_KRK);
    EXPECT_FALSE(Evaluate::material_draw(position));
}
