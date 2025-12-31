#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "evaluate.h"
#include "position.h"
#include "helpers/test_helpers.h"

using namespace Akerbeltz;
using namespace TestHelpers;

namespace {

class EvaluateTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        init_engine_once();
        init_evaluate_once();
    }
};

std::string mirror_swap_fen(const std::string& fen) {
    std::istringstream iss(fen);
    std::string placement;
    std::string stm;
    std::string castling;
    std::string ep;
    std::string halfmove;
    std::string fullmove;

    iss >> placement >> stm >> castling >> ep >> halfmove >> fullmove;

    std::vector<std::string> ranks;
    std::string current;
    for (char ch : placement) {
        if (ch == '/') {
            ranks.push_back(current);
            current.clear();
        } else {
            current.push_back(ch);
        }
    }
    ranks.push_back(current);
    std::reverse(ranks.begin(), ranks.end());

    for (auto& rank : ranks) {
        for (char& ch : rank) {
            if (std::isupper(static_cast<unsigned char>(ch))) {
                ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
            } else if (std::islower(static_cast<unsigned char>(ch))) {
                ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
            }
        }
    }

    std::string placement_out;
    for (std::size_t i = 0; i < ranks.size(); ++i) {
        if (i) placement_out.push_back('/');
        placement_out += ranks[i];
    }

    const std::string stm_out = (stm == "w") ? "b" : "w";

    std::string castling_out;
    if (castling == "-") {
        castling_out = "-";
    } else {
        for (char ch : castling) {
            switch (ch) {
                case 'K': castling_out.push_back('k'); break;
                case 'Q': castling_out.push_back('q'); break;
                case 'k': castling_out.push_back('K'); break;
                case 'q': castling_out.push_back('Q'); break;
                default: break;
            }
        }
    }

    std::string ep_out = "-";
    if (ep != "-") {
        const char file = ep[0];
        const int rank = ep[1] - '0';
        const int flipped = 9 - rank;
        ep_out = std::string() + file + static_cast<char>('0' + flipped);
    }

    return placement_out + " " + stm_out + " " + castling_out + " " + ep_out + " " + halfmove + " " + fullmove;
}

TEST_F(EvaluateTest, TempoAppliesSymmetricallyInKingOnlyPosition) {
    Position white_to_move;
    white_to_move.set_FEN("4k3/8/8/8/8/8/8/4K3 w - - 0 1");

    Position black_to_move;
    black_to_move.set_FEN("4k3/8/8/8/8/8/8/4K3 b - - 0 1");

    const auto score_white = Evaluate::calc_score(white_to_move);
    const auto score_black = Evaluate::calc_score(black_to_move);
    EXPECT_EQ(score_white, score_black);
}

TEST_F(EvaluateTest, SymmetryWithColorSwapAndFlipKeepsScore) {
    const std::string fen =
        "r1bqkbnr/pppp1ppp/2n5/4p3/1b1PP3/2N2N2/PPP2PPP/R1BQKB1R w KQkq - 2 4";
    Position position;
    position.set_FEN(fen);
    const auto score = Evaluate::calc_score(position);

    Position mirrored;
    mirrored.set_FEN(mirror_swap_fen(fen));
    const auto mirrored_score = Evaluate::calc_score(mirrored);

    EXPECT_EQ(score, mirrored_score);
}

TEST_F(EvaluateTest, MaterialAdvantageChangesScoreSign) {
    Position white_ahead;
    white_ahead.set_FEN("4k3/8/8/8/8/8/8/4KQ2 w - - 0 1");
    EXPECT_GT(Evaluate::calc_score(white_ahead), 0);

    Position black_ahead;
    black_ahead.set_FEN("4kq2/8/8/8/8/8/8/4K3 w - - 0 1");
    EXPECT_LT(Evaluate::calc_score(black_ahead), 0);
}

TEST_F(EvaluateTest, SideToMoveAffectsScoreWithMaterialImbalance) {
    Position white_to_move;
    white_to_move.set_FEN("4k3/8/8/8/8/8/8/4KQ2 w - - 0 1");
    const auto score_white = Evaluate::calc_score(white_to_move);

    Position black_to_move;
    black_to_move.set_FEN("4k3/8/8/8/8/8/8/4KQ2 b - - 0 1");
    const auto score_black = Evaluate::calc_score(black_to_move);

    EXPECT_GT(score_white, score_black);
}

TEST_F(EvaluateTest, MaterialMonotonicityForSideToMove) {
    Position base;
    base.set_FEN("4k3/8/8/8/8/8/8/4K3 w - - 0 1");
    const auto base_score = Evaluate::calc_score(base);

    Position white_plus;
    white_plus.set_FEN("4k3/8/8/8/3Q4/8/8/4K3 w - - 0 1");
    const auto white_score = Evaluate::calc_score(white_plus);
    EXPECT_GT(white_score, base_score);

    Position black_plus;
    black_plus.set_FEN("4k3/8/8/3q4/8/8/8/4K3 w - - 0 1");
    const auto black_score = Evaluate::calc_score(black_plus);
    EXPECT_LT(black_score, base_score);
}

TEST_F(EvaluateTest, MaterialDrawDetectionBasicCases) {
    Position position;

    position.set_FEN("4k3/8/8/8/8/8/8/4K3 w - - 0 1");
    EXPECT_TRUE(Evaluate::material_draw(position));

    position.set_FEN("4k3/8/8/8/8/8/8/4KN2 w - - 0 1");
    EXPECT_TRUE(Evaluate::material_draw(position));

    position.set_FEN("4k3/8/8/8/8/8/8/4KB2 w - - 0 1");
    EXPECT_TRUE(Evaluate::material_draw(position));

    position.set_FEN("4kb2/8/8/8/8/8/8/2B1K3 w - - 0 1");
    EXPECT_TRUE(Evaluate::material_draw(position));

    position.set_FEN("4b2k/8/8/8/8/8/8/2B1K3 w - - 0 1");
    EXPECT_FALSE(Evaluate::material_draw(position));

    position.set_FEN("4k3/8/8/8/8/8/8/2B1KB2 w - - 0 1");
    EXPECT_FALSE(Evaluate::material_draw(position));

    position.set_FEN("4k3/8/8/8/8/8/4P3/4K3 w - - 0 1");
    EXPECT_FALSE(Evaluate::material_draw(position));
}

TEST_F(EvaluateTest, MaterialDrawKnightsThreshold) {
    Position position;

    position.set_FEN("1n2k1n1/8/8/8/8/8/8/4K3 w - - 0 1");
    EXPECT_TRUE(Evaluate::material_draw(position));

    position.set_FEN("1n2k1n1/8/8/8/8/8/8/3NK3 w - - 0 1");
    EXPECT_FALSE(Evaluate::material_draw(position));
}

TEST_F(EvaluateTest, EndgamePhaseThresholdBoundary) {
    Position at_threshold;
    at_threshold.set_FEN("4k3/8/8/8/8/8/8/3QK2R w - - 0 1");
    EXPECT_EQ(at_threshold.game_phase_weight(), 6);
    EXPECT_TRUE(at_threshold.is_endgame_phase());

    Position above_threshold;
    above_threshold.set_FEN("4k3/8/8/8/8/8/8/3QK1NR w - - 0 1");
    EXPECT_EQ(above_threshold.game_phase_weight(), 7);
    EXPECT_FALSE(above_threshold.is_endgame_phase());
}

TEST_F(EvaluateTest, EndgamePhaseThresholdStartposIsMiddlegame) {
    Position startpos;
    startpos.set_FEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    EXPECT_EQ(startpos.game_phase_weight(), 24);
    EXPECT_FALSE(startpos.is_endgame_phase());
}

TEST_F(EvaluateTest, EndgameKingCentralizationIsPreferred) {
    Position central;
    central.set_FEN("4k3/8/8/8/4K3/8/8/8 w - - 0 1");
    const auto central_score = Evaluate::calc_score(central);

    Position corner;
    corner.set_FEN("4k3/8/8/8/8/8/8/K7 w - - 0 1");
    const auto corner_score = Evaluate::calc_score(corner);

    EXPECT_GT(central_score, corner_score);
}

}  // namespace
