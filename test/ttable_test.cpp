#include <gtest/gtest.h>

#include "helpers/test_helpers.h"
#include "position.h"
#include "ttable.h"

using namespace Akerbeltz;
using namespace TestHelpers;

namespace {

std::size_t entry_count_for_mb(std::size_t size_mb) {
    const std::size_t bytes = size_mb * 1024ULL * 1024ULL;
    const std::size_t count = bytes / sizeof(TT::Entry);
    return count ? count : 1;
}

class TTableTest : public ::testing::Test {
protected:
    void SetUp() override {
        TT::resize(4);
        TT::clear();
    }
};

TEST_F(TTableTest, ResizeClampsToMinimum) {
    TT::resize(1);
    EXPECT_EQ(TT::current_size_mb(), TT::MIN_TT_MB);
}

TEST_F(TTableTest, StoreAndProbeRoundTrip) {
    const Key key = 0x12345678ULL;
    const Move move = set_heuristic_score(make_quiet_move(SQ64_A2, SQ64_A3, SpecialMove::NO_SPECIAL), PV_SCORE);
    TT::store(key, 5, 42, TT::FLAG_EXACT, move);

    TT::Entry entry{};
    ASSERT_TRUE(TT::probe(key, entry));
    EXPECT_EQ(entry.key, key);
    EXPECT_EQ(entry.depth, 5);
    EXPECT_EQ(entry.score, 42);
    EXPECT_EQ(entry.flag, TT::FLAG_EXACT);
    EXPECT_EQ(entry.move, raw_move(move));
}

TEST_F(TTableTest, ClearRemovesEntries) {
    const Key key = 0xABCDEFULL;
    TT::store(key, 3, -7, TT::FLAG_UPPERBOUND, NOMOVE);
    TT::clear();

    TT::Entry entry{};
    EXPECT_FALSE(TT::probe(key, entry));
}

TEST_F(TTableTest, CollisionReplacementRespectsDepth) {
    const std::size_t entries = entry_count_for_mb(TT::current_size_mb());
    const Key key1 = 0x1000ULL;
    const Key key2 = key1 + entries;

    TT::store(key1, 5, 10, TT::FLAG_EXACT, NOMOVE);
    TT::store(key2, 3, 20, TT::FLAG_EXACT, NOMOVE);

    TT::Entry entry{};
    EXPECT_TRUE(TT::probe(key1, entry));
    EXPECT_FALSE(TT::probe(key2, entry));

    TT::store(key2, 6, 30, TT::FLAG_EXACT, NOMOVE);
    EXPECT_TRUE(TT::probe(key2, entry));
    EXPECT_FALSE(TT::probe(key1, entry));
}

TEST_F(TTableTest, LoadPvLineUsesStoredMovesAndRestoresPosition) {
    init_engine_once();

    Position position;
    position.set_FEN("4k3/8/8/8/8/8/8/4K3 w - - 0 1");
    const std::string fen_before = position.get_FEN();
    const Key key1 = position.get_key();

    const Move move1 = make_quiet_move(SQ64_E1, SQ64_E2, SpecialMove::NO_SPECIAL);
    TT::store(key1, 4, 0, TT::FLAG_EXACT, move1);

    ASSERT_TRUE(position.do_move(move1));
    const Key key2 = position.get_key();
    const Move move2 = make_quiet_move(SQ64_E8, SQ64_E7, SpecialMove::NO_SPECIAL);
    TT::store(key2, 3, 0, TT::FLAG_EXACT, move2);
    position.undo_move();

    TT::PVLine line{};
    TT::load_pv_line(position, line, 4);

    EXPECT_EQ(line.depth, 2);
    EXPECT_EQ(line.moves[0], raw_move(move1));
    EXPECT_EQ(line.moves[1], raw_move(move2));
    EXPECT_EQ(position.get_FEN(), fen_before);
}

}  // namespace
