#include <gtest/gtest.h>
#include <chrono>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

#include "search.h"
#include "position.h"
#include "attacks.h"
#include "evaluate.h"
#include "pvtable.h"

using namespace Xake;

namespace {

int extract_last_info_score_cp(const std::string& output) {
    std::istringstream stream(output);
    std::string line;
    int lastScore = std::numeric_limits<int>::max();

    while (std::getline(stream, line)) {
        const std::string token = "score cp ";
        const auto pos = line.find(token);
        if (pos != std::string::npos) {
            std::istringstream valueStream(line.substr(pos + token.size()));
            valueStream >> lastScore;
        }
    }

    return lastScore;
}

void configure_search_info(Search::SearchInfo& info, int depth) {
    info.depth = depth;
    info.nodes = 0;
    info.searchPly = 0;
    info.startTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    info.stopTime = NO_TIME;
    info.moveTime = NO_TIME;
    info.realTime = 0;
    info.timeOver = false;
    info.stop = false;
}

} // namespace

class SearchTest : public ::testing::Test {
protected:
    void SetUp() override {
        static bool initialized = false;
        if (!initialized) {
            Position::init();
            Evaluate::init();
            Attacks::init();
            initialized = true;
        }
        PVTable::init();
    }

    Position position;
};

namespace Xake {
namespace Search {
Evaluate::Score quiescence_search(Position &position,
                                  Search::SearchInfo &searchInfo,
                                  Evaluate::Score alpha,
                                  Evaluate::Score beta);
}
}

TEST_F(SearchTest, MaterialDrawReturnsDrawScore) {
    position.set_FEN("4k3/8/8/8/8/8/8/4K3 w - - 0 1");

    Search::SearchInfo searchInfo{};
    configure_search_info(searchInfo, 3);

    testing::internal::CaptureStdout();
    Search::search(position, searchInfo);
    const std::string output = testing::internal::GetCapturedStdout();

    const int lastScore = extract_last_info_score_cp(output);
    EXPECT_EQ(lastScore, Evaluate::DRAW_SOCORE);
}

TEST_F(SearchTest, MaterialAdvantageProducesNonZeroScore) {
    position.set_FEN("4k3/8/8/8/3P4/8/8/4K3 w - - 0 1");

    Search::SearchInfo searchInfo{};
    configure_search_info(searchInfo, 2);

    testing::internal::CaptureStdout();
    Search::search(position, searchInfo);
    const std::string output = testing::internal::GetCapturedStdout();

    const int lastScore = extract_last_info_score_cp(output);
    ASSERT_NE(lastScore, std::numeric_limits<int>::max());
    EXPECT_NE(lastScore, Evaluate::DRAW_SOCORE);
}

TEST_F(SearchTest, QuiescenceReturnsDrawScoreOnMaterialDrawLeaf) {
    position.set_FEN("4k3/8/8/8/8/8/8/3BK3 w - - 0 1");

    Search::SearchInfo searchInfo{};
    configure_search_info(searchInfo, 0);

    const Evaluate::Score score = Search::quiescence_search(
        position,
        searchInfo,
        -Evaluate::CHECKMATE_SCORE,
        Evaluate::CHECKMATE_SCORE);

    EXPECT_EQ(score, Evaluate::DRAW_SOCORE);
}

TEST_F(SearchTest, FiftyMoveRuleReturnsDrawScore) {
    position.set_FEN("4k3/8/8/8/8/8/8/4K3 w - - 100 1");

    Search::SearchInfo searchInfo{};
    configure_search_info(searchInfo, 2);

    testing::internal::CaptureStdout();
    Search::search(position, searchInfo);
    const std::string output = testing::internal::GetCapturedStdout();

    const int lastScore = extract_last_info_score_cp(output);
    EXPECT_EQ(lastScore, Evaluate::DRAW_SOCORE);
}

TEST_F(SearchTest, RepetitionReturnsDrawScore) {
    static constexpr const char* START_FEN =
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    position.set_FEN(START_FEN);

    position.do_move(make_quiet_move(SQ64_G1, SQ64_F3, SpecialMove::NO_SPECIAL));
    position.do_move(make_quiet_move(SQ64_B8, SQ64_C6, SpecialMove::NO_SPECIAL));
    position.do_move(make_quiet_move(SQ64_F3, SQ64_G1, SpecialMove::NO_SPECIAL));
    position.do_move(make_quiet_move(SQ64_C6, SQ64_B8, SpecialMove::NO_SPECIAL));

    ASSERT_TRUE(position.is_repetition());

    Search::SearchInfo searchInfo{};
    configure_search_info(searchInfo, 2);

    testing::internal::CaptureStdout();
    Search::search(position, searchInfo);
    const std::string output = testing::internal::GetCapturedStdout();

    const int lastScore = extract_last_info_score_cp(output);
    EXPECT_EQ(lastScore, Evaluate::DRAW_SOCORE);
}
