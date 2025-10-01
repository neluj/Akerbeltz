#include <array>
#include <gtest/gtest.h>

#include "evaluate.h"
#include "position.h"

using namespace Xake;

namespace Xake::Evaluate {
    Score eval_pawn_structure(const Position& pos);
    Score eval_open_files(const Position& pos);
}

namespace {
constexpr Evaluate::Score kIsolatedPawnPenalty = 15;
constexpr std::array<Evaluate::Score, 8> kPassedPawnBonusByRank{{0, 10, 20, 35, 60, 90, 140, 0}};
constexpr Evaluate::Score kRookOpenFileBonus = 32;
constexpr Evaluate::Score kRookSemiOpenFileBonus = 18;
constexpr Evaluate::Score kQueenOpenFileBonus = 14;
constexpr Evaluate::Score kQueenSemiOpenFileBonus = 7;

constexpr const char* kStartFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
constexpr const char* kWhiteAdvantageWhiteToMove = "4k3/8/8/8/4Q3/8/8/4K3 w - - 0 1";
constexpr const char* kWhiteAdvantageBlackToMove = "4k3/8/8/8/4Q3/8/8/4K3 b - - 0 1";
constexpr const char* kPassedPawnFen = "4k3/8/8/8/3P4/2p5/8/4K3 w - - 0 1";
constexpr const char* kBlockedPawnFen = "4k3/8/8/2p5/3P4/8/8/4K3 w - - 0 1";
constexpr const char* kIsolatedPawnsFen = "4k3/8/8/8/2P5/8/8/P3K3 w - - 0 1";
constexpr const char* kSupportedPawnsFen = "4k3/8/8/8/2P5/8/8/1P2K3 w - - 0 1";
constexpr const char* kRookSemiOpenFen = "4k3/8/8/8/3r4/3P4/8/3K4 b - - 0 1";
constexpr const char* kRookOpenFen = "4k3/8/8/8/3r4/4P3/8/3K4 b - - 0 1";
constexpr const char* kQueenSemiOpenFen = "4k3/8/8/8/3q4/3P4/8/3K4 b - - 0 1";
constexpr const char* kQueenOpenFen = "4k3/8/8/8/3q4/4P3/8/3K4 b - - 0 1";
constexpr const char* kBareKingsFen = "4k3/8/8/8/8/8/8/4K3 w - - 0 1";
constexpr const char* kKBKFen = "4k3/8/8/8/8/8/8/3BK3 w - - 0 1";
constexpr const char* kKNKFen = "4k3/8/8/8/8/8/8/3NK3 w - - 0 1";
constexpr const char* kKNNKFen = "4k3/8/8/8/8/8/8/2NNK3 w - - 0 1";
constexpr const char* kKBKBSameColorFen = "4kb2/8/8/8/8/8/8/2B1K3 w - - 0 1";
constexpr const char* kKBNKFen = "4k3/8/8/8/8/8/8/2B1K1N1 w - - 0 1";
constexpr const char* kKRKFen = "4k3/8/8/8/8/8/8/3RK3 w - - 0 1";
}

class EvaluateTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        Position::init();
        Evaluate::init();
    }

    Position position_;
};

TEST_F(EvaluateTest, CalcScoreStartPositionIsNeutral) {
    position_.set_FEN(kStartFen);
    EXPECT_EQ(Evaluate::calc_score(position_), 0);
}

TEST_F(EvaluateTest, CalcScoreDependsOnSideToMove) {
    position_.set_FEN(kWhiteAdvantageWhiteToMove);
    const Evaluate::Score whiteToMove = Evaluate::calc_score(position_);

    position_.set_FEN(kWhiteAdvantageBlackToMove);
    const Evaluate::Score blackToMove = Evaluate::calc_score(position_);

    EXPECT_GT(whiteToMove, 0);
    EXPECT_LT(blackToMove, 0);
    EXPECT_EQ(whiteToMove, -blackToMove);
}

TEST_F(EvaluateTest, PawnStructureRewardsPassedPawns) {
    Position passed;
    Position blocked;
    passed.set_FEN(kPassedPawnFen);
    blocked.set_FEN(kBlockedPawnFen);

    const Evaluate::Score passedScore = Evaluate::eval_pawn_structure(passed);
    const Evaluate::Score blockedScore = Evaluate::eval_pawn_structure(blocked);

    constexpr Evaluate::Score expected =
        kPassedPawnBonusByRank[3] - kPassedPawnBonusByRank[5];
    EXPECT_EQ(passedScore - blockedScore, expected);
}

TEST_F(EvaluateTest, PawnStructurePenalizesIsolatedPawns) {
    Position isolated;
    Position supported;
    isolated.set_FEN(kIsolatedPawnsFen);
    supported.set_FEN(kSupportedPawnsFen);

    const Evaluate::Score isolatedScore = Evaluate::eval_pawn_structure(isolated);
    const Evaluate::Score supportedScore = Evaluate::eval_pawn_structure(supported);

    constexpr Evaluate::Score expected = 2 * kIsolatedPawnPenalty;
    EXPECT_EQ(supportedScore - isolatedScore, expected);
}

TEST_F(EvaluateTest, OpenFilesApplyRookBonuses) {
    Position semiOpen;
    Position open;
    semiOpen.set_FEN(kRookSemiOpenFen);
    open.set_FEN(kRookOpenFen);

    const Evaluate::Score semiOpenScore = Evaluate::eval_open_files(semiOpen);
    const Evaluate::Score openScore = Evaluate::eval_open_files(open);

    constexpr Evaluate::Score expected =
        -(kRookOpenFileBonus - kRookSemiOpenFileBonus);
    EXPECT_EQ(openScore - semiOpenScore, expected);
}

TEST_F(EvaluateTest, OpenFilesApplyQueenBonuses) {
    Position semiOpen;
    Position open;
    semiOpen.set_FEN(kQueenSemiOpenFen);
    open.set_FEN(kQueenOpenFen);

    const Evaluate::Score semiOpenScore = Evaluate::eval_open_files(semiOpen);
    const Evaluate::Score openScore = Evaluate::eval_open_files(open);

    constexpr Evaluate::Score expected =
        -(kQueenOpenFileBonus - kQueenSemiOpenFileBonus);
    EXPECT_EQ(openScore - semiOpenScore, expected);
}

TEST_F(EvaluateTest, MaterialDrawDetectionMatchesCommonCases) {
    Position pos;

    pos.set_FEN(kBareKingsFen);
    EXPECT_TRUE(Evaluate::material_draw(pos));

    pos.set_FEN(kKBKFen);
    EXPECT_TRUE(Evaluate::material_draw(pos));

    pos.set_FEN(kKNKFen);
    EXPECT_TRUE(Evaluate::material_draw(pos));

    pos.set_FEN(kKNNKFen);
    EXPECT_TRUE(Evaluate::material_draw(pos));

    pos.set_FEN(kKBKBSameColorFen);
    EXPECT_TRUE(Evaluate::material_draw(pos));

    pos.set_FEN(kKBNKFen);
    EXPECT_FALSE(Evaluate::material_draw(pos));

    pos.set_FEN(kKRKFen);
    EXPECT_FALSE(Evaluate::material_draw(pos));
}

