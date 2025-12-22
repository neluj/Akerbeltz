#include <gtest/gtest.h>
#include <cstdlib>

#include "attacks.h"
#include "bitboards.h"
#include "types.h"
#include "helpers/test_helpers.h"

using namespace Xake;
using namespace Bitboards;
using namespace TestHelpers;

namespace {

Bitboard ray_attacks(Square64 start, int delta, Bitboard occupied) {
    Bitboard result = ZERO;
    int sq = static_cast<int>(start);
    int prevFile = square_file(start);
    while (true) {
        sq += delta;
        if (sq < 0 || sq >= SQ64_SIZE) break;
        const int file = sq % 8;
        // If we wrapped around the board horizontally, stop.
        if (std::abs(file - prevFile) > 1) break;
        prevFile = file;
        result |= (ONE << sq);
        if (occupied & (ONE << sq)) break;
    }
    return result;
}

class AttacksTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() { init_attacks_once(); }
};

TEST_F(AttacksTest, PawnAttacksStayOnBoard) {
    EXPECT_EQ(Attacks::pawnAttacks[WHITE][SQ64_A2], squares({SQ64_B3}));
    EXPECT_EQ(Attacks::pawnAttacks[WHITE][SQ64_H2], squares({SQ64_G3}));
    EXPECT_EQ(Attacks::pawnAttacks[BLACK][SQ64_A7], squares({SQ64_B6}));
    EXPECT_EQ(Attacks::pawnAttacks[BLACK][SQ64_H7], squares({SQ64_G6}));
}

TEST_F(AttacksTest, KnightAndKingAttackCounts) {
    EXPECT_EQ(cpop(Attacks::knightAttacks[SQ64_A1]), 2);
    EXPECT_EQ(cpop(Attacks::knightAttacks[SQ64_D4]), 8);
    EXPECT_EQ(cpop(Attacks::kingAttacks[SQ64_A1]), 3);
    EXPECT_EQ(cpop(Attacks::kingAttacks[SQ64_E4]), 8);
}

TEST_F(AttacksTest, SlidingRookRespectsBlockers) {
    const Square64 origin = SQ64_D4;
    Bitboard occupied = squares({SQ64_D6, SQ64_F4, SQ64_D2, SQ64_B4});
    const Bitboard expected =
        ray_attacks(origin, Direction::NORTH, occupied) |
        ray_attacks(origin, Direction::SOUTH, occupied) |
        ray_attacks(origin, Direction::EAST, occupied) |
        ray_attacks(origin, Direction::WEST, occupied);

    const Bitboard attacks = Attacks::sliding_side_attacks(origin, occupied);
    EXPECT_EQ(attacks, expected);
}

TEST_F(AttacksTest, SlidingBishopRespectsBlockers) {
    const Square64 origin = SQ64_D4;
    Bitboard occupied = squares({SQ64_F6, SQ64_B6, SQ64_F2, SQ64_B2});
    const Bitboard expected =
        ray_attacks(origin, Direction::NORTH_EAST, occupied) |
        ray_attacks(origin, Direction::NORTH_WEST, occupied) |
        ray_attacks(origin, Direction::SOUTH_EAST, occupied) |
        ray_attacks(origin, Direction::SOUTH_WEST, occupied);

    const Bitboard attacks = Attacks::sliding_diagonal_attacks(origin, occupied);
    EXPECT_EQ(attacks, expected);
}

TEST_F(AttacksTest, SlidingBishopEdgeStopsCorrectly) {
    const Square64 origin = SQ64_A1;
    Bitboard occupied = squares({SQ64_C3});
    const Bitboard expected = ray_attacks(origin, Direction::NORTH_EAST, occupied);
    const Bitboard attacks = Attacks::sliding_diagonal_attacks(origin, occupied);
    EXPECT_EQ(attacks, expected);
}

}  // namespace
