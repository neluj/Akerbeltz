#include <array>
#include <initializer_list>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "attacks.h"

using namespace Xake;
using namespace Bitboards;
using namespace Attacks;

namespace {

Bitboard makeBitboard(std::initializer_list<Square64> squares) {
    Bitboard bitboard = ZERO;
    for (Square64 square : squares) {
        bitboard = set_pieces(bitboard, square);
    }
    return bitboard;
}

Square64 makeSquare(int rank, int file) {
    return make_square64(static_cast<Rank>(rank), static_cast<File>(file));
}

Bitboard accumulateRay(Square64 origin, int fileStep, int rankStep, Bitboard occupied) {
    Bitboard result = ZERO;
    int file = static_cast<int>(square_file(origin));
    int rank = static_cast<int>(square_rank(origin));

    int nextFile = file + fileStep;
    int nextRank = rank + rankStep;

    while (nextFile >= 0 && nextFile < 8 && nextRank >= 0 && nextRank < 8) {
        Square64 target = makeSquare(nextRank, nextFile);
        result = set_pieces(result, target);
        if (occupied & (ONE << target)) {
            break;
        }

        nextFile += fileStep;
        nextRank += rankStep;
    }

    return result;
}

template <size_t N>
Bitboard accumulateDirections(Square64 origin,
                              const std::array<std::pair<int, int>, N>& directions,
                              Bitboard occupied) {
    Bitboard result = ZERO;
    for (const auto& direction : directions) {
        result |= accumulateRay(origin, direction.first, direction.second, occupied);
    }
    return result;
}

template <size_t N>
std::vector<Square64> collectDirectionalSquares(
    Square64 origin,
    const std::array<std::pair<int, int>, N>& directions) {
    std::vector<Square64> squares;
    squares.reserve(14);  // Maximum rook ray length from a central square.
    for (const auto& direction : directions) {
        int file = static_cast<int>(square_file(origin));
        int rank = static_cast<int>(square_rank(origin));
        int nextFile = file + direction.first;
        int nextRank = rank + direction.second;
        while (nextFile >= 0 && nextFile < 8 && nextRank >= 0 && nextRank < 8) {
            squares.push_back(makeSquare(nextRank, nextFile));
            nextFile += direction.first;
            nextRank += direction.second;
        }
    }
    return squares;
}

Bitboard makeBitboardFromMask(const std::vector<Square64>& squares, uint64_t mask) {
    Bitboard bitboard = ZERO;
    for (size_t bit = 0; bit < squares.size(); ++bit) {
        if (mask & (1ULL << bit)) {
            bitboard = set_pieces(bitboard, squares[bit]);
        }
    }
    return bitboard;
}

class AttacksTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        init();
    }
};

}  // namespace

TEST_F(AttacksTest, PawnAttacks) {
    struct PawnCase {
        Color color;
        Square64 square;
        std::initializer_list<Square64> expected;
    };

    const PawnCase cases[] = {
        {WHITE, SQ64_A8, {}},
        {BLACK, SQ64_A8, {SQ64_B7}},
        {WHITE, SQ64_H8, {}},
        {BLACK, SQ64_H8, {SQ64_G7}},
        {WHITE, SQ64_H1, {SQ64_G2}},
        {BLACK, SQ64_H1, {}},
        {WHITE, SQ64_A1, {SQ64_B2}},
        {BLACK, SQ64_A1, {}},
        {WHITE, SQ64_A4, {SQ64_B5}},
        {BLACK, SQ64_A4, {SQ64_B3}},
        {WHITE, SQ64_D8, {}},
        {BLACK, SQ64_D8, {SQ64_C7, SQ64_E7}},
        {WHITE, SQ64_H4, {SQ64_G5}},
        {BLACK, SQ64_H4, {SQ64_G3}},
        {WHITE, SQ64_D1, {SQ64_C2, SQ64_E2}},
        {BLACK, SQ64_D1, {}},
        {WHITE, SQ64_D4, {SQ64_C5, SQ64_E5}},
        {BLACK, SQ64_D4, {SQ64_C3, SQ64_E3}},
    };

    for (const auto& testCase : cases) {
        EXPECT_EQ(pawnAttacks[testCase.color][testCase.square],
                  makeBitboard(testCase.expected));
    }
}

TEST_F(AttacksTest, KnightAttacks) {
    struct KnightCase {
        Square64 square;
        std::initializer_list<Square64> expected;
    };

    const KnightCase cases[] = {
        {SQ64_A8, {SQ64_B6, SQ64_C7}},
        {SQ64_H8, {SQ64_G6, SQ64_F7}},
        {SQ64_H1, {SQ64_G3, SQ64_F2}},
        {SQ64_A1, {SQ64_B3, SQ64_C2}},
        {SQ64_A4, {SQ64_B6, SQ64_C5, SQ64_C3, SQ64_B2}},
        {SQ64_D8, {SQ64_B7, SQ64_C6, SQ64_E6, SQ64_F7}},
        {SQ64_H4, {SQ64_G6, SQ64_F5, SQ64_F3, SQ64_G2}},
        {SQ64_D1, {SQ64_B2, SQ64_C3, SQ64_E3, SQ64_F2}},
        {SQ64_D4, {SQ64_C6, SQ64_E6, SQ64_F5, SQ64_F3, SQ64_E2, SQ64_C2, SQ64_B3, SQ64_B5}},
    };

    for (const auto& testCase : cases) {
        EXPECT_EQ(knightAttacks[testCase.square],
                  makeBitboard(testCase.expected));
    }
}

TEST_F(AttacksTest, KingAttacks) {
    struct KingCase {
        Square64 square;
        std::initializer_list<Square64> expected;
    };

    const KingCase cases[] = {
        {SQ64_A8, {SQ64_A7, SQ64_B7, SQ64_B8}},
        {SQ64_H8, {SQ64_H7, SQ64_G7, SQ64_G8}},
        {SQ64_H1, {SQ64_H2, SQ64_G2, SQ64_G1}},
        {SQ64_A1, {SQ64_A2, SQ64_B2, SQ64_B1}},
        {SQ64_A4, {SQ64_A5, SQ64_B5, SQ64_B4, SQ64_B3, SQ64_A3}},
        {SQ64_D8, {SQ64_C8, SQ64_C7, SQ64_D7, SQ64_E7, SQ64_E8}},
        {SQ64_H4, {SQ64_H5, SQ64_G5, SQ64_G4, SQ64_G3, SQ64_H3}},
        {SQ64_D1, {SQ64_C1, SQ64_C2, SQ64_D2, SQ64_E2, SQ64_E1}},
        {SQ64_D4, {SQ64_C5, SQ64_D5, SQ64_E5, SQ64_E4, SQ64_E3, SQ64_D3, SQ64_C3, SQ64_C4}},
    };

    for (const auto& testCase : cases) {
        EXPECT_EQ(kingAttacks[testCase.square],
                  makeBitboard(testCase.expected));
    }
}

TEST_F(AttacksTest, SlidingDiagonalAttacks) {
    constexpr std::array<std::pair<int, int>, 4> directions = {{
        {1, 1},  {1, -1},
        {-1, 1}, {-1, -1},
    }};

    struct SlidingCase {
        Square64 origin;
        std::initializer_list<Square64> occupied;
    };

    const SlidingCase cases[] = {
        {SQ64_D4, {SQ64_F6, SQ64_B2, SQ64_D2, SQ64_A4, SQ64_G4, SQ64_A7, SQ64_C3, SQ64_F2}},
        {SQ64_D4, {}},
        {SQ64_A1, {SQ64_B2, SQ64_C3, SQ64_D4, SQ64_F6}},
        {SQ64_H8, {SQ64_G7, SQ64_E5, SQ64_D4, SQ64_C3, SQ64_B2, SQ64_A1}},
        {SQ64_C6, {SQ64_A4, SQ64_E8, SQ64_F3, SQ64_C7}},
    };

    for (const auto& testCase : cases) {
        Bitboard occupied = makeBitboard(testCase.occupied);
        Bitboard expected = accumulateDirections(testCase.origin, directions, occupied);
        EXPECT_EQ(sliding_diagonal_attacks(testCase.origin, occupied), expected);
    }
}

TEST_F(AttacksTest, SlidingSideAttacks) {
    constexpr std::array<std::pair<int, int>, 4> directions = {{
        {0, 1},  {0, -1},
        {1, 0},  {-1, 0},
    }};

    struct SlidingCase {
        Square64 origin;
        std::initializer_list<Square64> occupied;
    };

    const SlidingCase cases[] = {
        {SQ64_D4, {SQ64_F6, SQ64_B2, SQ64_D2, SQ64_A4, SQ64_G4, SQ64_A7, SQ64_C3, SQ64_F2}},
        {SQ64_D4, {}},
        {SQ64_A1, {SQ64_A2, SQ64_A8, SQ64_B1, SQ64_H1, SQ64_C3}},
        {SQ64_H8, {SQ64_H7, SQ64_H1, SQ64_G8, SQ64_A8}},
        {SQ64_E2, {SQ64_E7, SQ64_E1, SQ64_H2, SQ64_A2, SQ64_B5}},
    };

    for (const auto& testCase : cases) {
        Bitboard occupied = makeBitboard(testCase.occupied);
        Bitboard expected = accumulateDirections(testCase.origin, directions, occupied);
        EXPECT_EQ(sliding_side_attacks(testCase.origin, occupied), expected);
    }
}

TEST_F(AttacksTest, SlidingDiagonalAttacksCoverAllRayOccupancies) {
    constexpr std::array<std::pair<int, int>, 4> directions = {{
        {1, 1},  {1, -1},
        {-1, 1}, {-1, -1},
    }};

    for (Square64 origin = SQ64_A1; origin < SQ64_SIZE; ++origin) {
        const std::vector<Square64> raySquares = collectDirectionalSquares(origin, directions);
        const uint64_t subsetCount = 1ULL << raySquares.size();
        for (uint64_t mask = 0; mask < subsetCount; ++mask) {
            const Bitboard occupied = makeBitboardFromMask(raySquares, mask);
            const Bitboard expected = accumulateDirections(origin, directions, occupied);
            ASSERT_EQ(sliding_diagonal_attacks(origin, occupied), expected);
        }
    }
}

TEST_F(AttacksTest, SlidingSideAttacksCoverAllRayOccupancies) {
    constexpr std::array<std::pair<int, int>, 4> directions = {{
        {0, 1},  {0, -1},
        {1, 0},  {-1, 0},
    }};

    for (Square64 origin = SQ64_A1; origin < SQ64_SIZE; ++origin) {
        const std::vector<Square64> raySquares = collectDirectionalSquares(origin, directions);
        const uint64_t subsetCount = 1ULL << raySquares.size();
        for (uint64_t mask = 0; mask < subsetCount; ++mask) {
            const Bitboard occupied = makeBitboardFromMask(raySquares, mask);
            const Bitboard expected = accumulateDirections(origin, directions, occupied);
            ASSERT_EQ(sliding_side_attacks(origin, occupied), expected);
        }
    }
}
