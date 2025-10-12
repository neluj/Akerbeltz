#include <algorithm>
#include <array>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "attacks.h"
#include "movegen.h"
#include "position.h"

using namespace Xake;

namespace {

std::vector<std::string> movesToAlgebraic(const MoveGen::MoveList& moveList) {
    std::vector<std::string> result;
    result.reserve(moveList.size);
    for (int index = 0; index < moveList.size; ++index) {
        result.emplace_back(algebraic_move(moveList.moves[index]));
    }
    return result;
}

bool containsMove(const std::vector<std::string>& moves, const std::string& target) {
    return std::find(moves.begin(), moves.end(), target) != moves.end();
}

class MoveGenTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        Position::init();
        Attacks::init();
    }

    MoveGen::MoveList generatePseudoMoves(const std::string& fen) const {
        Position position;
        position.set_FEN(fen);
        MoveGen::MoveList moveList;
        MoveGen::generate_pseudo_moves(position, moveList);
        return moveList;
    }

    MoveGen::MoveList generatePseudoCaptures(const std::string& fen) const {
        Position position;
        position.set_FEN(fen);
        MoveGen::MoveList moveList;
        MoveGen::generate_pseudo_captures(position, moveList);
        return moveList;
    }
};

constexpr const char* WHITE_PROMOTION_FEN = "4kb1r/6P1/8/8/8/8/8/4K3 w - - 0 1";
constexpr const char* WHITE_EN_PASSANT_FEN = "4k3/8/8/3Pp3/8/8/8/4K3 w - e6 0 1";
constexpr const char* BLACK_EN_PASSANT_FEN = "4k3/8/8/8/3pP3/8/8/4K3 b - e3 0 1";
constexpr const char* WHITE_CASTLING_FEN = "4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1";
constexpr const char* WHITE_CASTLING_ATTACKED_FEN = "3rk3/8/8/8/1b6/8/8/R3K2R w KQ - 0 1";

}  // namespace

TEST_F(MoveGenTest, PseudoCapturesMatchExpectedCounts) {
    struct CaptureCase {
        const char* name;
        const char* fen;
        int expectedCount;
    };

    const CaptureCase captureCases[] = {
        {"WhitePawnCaptures", "3krrb1/5P2/3p1p2/3PP1Pp/Pp1p4/2p5/3P4/3K4 w - h6 1 1", 13},
        {"BlackPawnCaptures", "3k4/3p4/2P5/pP1P4/3pp1pP/3P1P2/5p2/3KRRB1 b - h3 1 1", 13},
        {"WhiteKnightCaptures", "3k4/8/3P4/4p3/2N5/4p3/8/4K3 w - - 0 1", 2},
        {"BlackKnightCaptures", "3k4/8/1p6/4P3/2n5/4P3/8/4K3 b - - 0 1", 2},
        {"WhiteKingCaptures", "3k4/8/8/8/8/6pP/5p1K/8 w - - 0 1", 1},
        {"BlackKingCaptures", "8/5P1k/6Pp/8/8/8/8/3K4 b - - 0 1", 1},
        {"WhiteBishopCaptures", "3k4/8/1p6/1p1PP3/2B5/4P3/4p3/7K w - - 0 1", 2},
        {"BlackBishopCaptures", "7k/4P3/4p3/2b5/1P1pp3/1P6/8/3K4 b - - 0 1", 2},
        {"WhiteRookCaptures", "7k/2p5/8/2R2P2/8/8/8/3K4 w - - 0 1", 1},
        {"BlackRookCaptures", "3k4/8/8/8/2r2p2/8/2P5/7K b - - 0 1", 1},
        {"WhiteQueenCaptures", "3k4/8/8/3p4/2Q2p2/1P6/2P5/7K w - - 0 1", 2},
        {"BlackQueenCaptures", "7k/2p5/1p6/2q2P2/3P4/8/8/3K4 b - - 0 1", 2},
    };

    for (const auto& testCase : captureCases) {
        SCOPED_TRACE(testCase.name);
        SCOPED_TRACE(testCase.fen);
        const MoveGen::MoveList moveList = generatePseudoCaptures(testCase.fen);
        EXPECT_EQ(moveList.size, testCase.expectedCount);
    }
}

TEST_F(MoveGenTest, PseudoMovesMatchExpectedCounts) {
    struct MoveCase {
        const char* name;
        const char* fen;
        int expectedCount;
    };

    const MoveCase moveCases[] = {
        {"WhitePawnMoves", "3k1p2/6P1/8/pPp1pp2/3P1P2/1pP2P1p/P6P/4K3 w - a6 0 1", 23},
        {"BlackPawnMoves", "4k3/p6p/1Pp2p1P/3p1p2/PpP1PP2/8/6p1/3K1P2 b - a3 0 1", 23},
        {"WhiteKnightMoves", "4kp2/8/3P2N1/2P5/2N5/2p2P2/1p6/5K2 w - - 0 1", 21},
        {"BlackKnightMoves", "5k2/1P6/2P2p2/2n5/2p5/3p2n1/8/4KP2 b - - 0 1", 21},
        {"WhiteKingCastlingMoves", "4k3/8/8/8/8/8/4P2P/3PK2R w K - 0 1", 11},
        {"BlackKingCastlingMoves", "4k2r/4p2p/8/8/8/8/8/4K3 b k - 0 1", 11},
        {"WhiteQueenCastlingMoves", "4k3/8/8/8/8/8/P3P3/R3K3 w Q - 1 1", 12},
        {"BlackQueenCastlingMoves", "r3k3/p3p3/8/8/8/8/8/4K3 b q - 1 1", 12},
        {"WhiteBishopMoves", "4k3/7B/8/2P1pp2/3B4/8/8/P3K1p1 w - - 0 1", 16},
        {"BlackBishopMoves", "p3k1P1/8/8/3b4/2p1PP2/8/7b/4K3 b - - 0 1", 16},
        {"WhiteRookMoves", "4kp2/8/2p2R2/2P5/2R5/2p5/8/5K2 w - - 0 1", 24},
        {"BlackRookMoves", "5k2/8/2P5/2r5/2p5/2P2r2/8/4KP2 b - - 0 1", 24},
        {"WhiteQueenMoves", "4kp2/8/5Q2/2P5/2Q5/2p2P2/8/5K2 w - - 0 1", 45},
        {"BlackQueenMoves", "5k2/8/2P2p2/2q5/2p5/5q2/8/4KP2 b - - 0 1", 45},
    };

    for (const auto& testCase : moveCases) {
        SCOPED_TRACE(testCase.name);
        SCOPED_TRACE(testCase.fen);
        const MoveGen::MoveList moveList = generatePseudoMoves(testCase.fen);
        EXPECT_EQ(moveList.size, testCase.expectedCount);
    }
}

TEST_F(MoveGenTest, WhitePawnPromotionMovesIncludeAllPromotionPieces) {
    const MoveGen::MoveList moveList = generatePseudoMoves(WHITE_PROMOTION_FEN);
    const std::vector<std::string> algebraicMoves = movesToAlgebraic(moveList);

    const std::array<std::string, 4> expectedQuietPromotions = {"g7g8q", "g7g8n", "g7g8r", "g7g8b"};
    for (const auto& expectedMove : expectedQuietPromotions) {
        EXPECT_TRUE(containsMove(algebraicMoves, expectedMove)) << "Expected quiet promotion " << expectedMove;
    }

    const std::array<std::string, 4> expectedCapturePromotions = {"g7h8q", "g7h8n", "g7h8r", "g7h8b"};
    for (const auto& expectedMove : expectedCapturePromotions) {
        EXPECT_TRUE(containsMove(algebraicMoves, expectedMove)) << "Expected capture promotion " << expectedMove;
    }

    const std::array<std::string, 4> expectedCapturePromotionsWest = {"g7f8q", "g7f8n", "g7f8r", "g7f8b"};
    for (const auto& expectedMove : expectedCapturePromotionsWest) {
        EXPECT_TRUE(containsMove(algebraicMoves, expectedMove)) << "Expected capture promotion " << expectedMove;
    }
}

TEST_F(MoveGenTest, WhitePawnPromotionCapturesIncludeAllPromotionPieces) {
    const MoveGen::MoveList moveList = generatePseudoCaptures(WHITE_PROMOTION_FEN);
    ASSERT_EQ(moveList.size, 8);

    const std::vector<std::string> algebraicMoves = movesToAlgebraic(moveList);
    const std::array<std::string, 8> expectedCapturePromotions = {
        "g7h8q", "g7h8n", "g7h8r", "g7h8b",
        "g7f8q", "g7f8n", "g7f8r", "g7f8b"
    };

    for (const auto& expectedMove : expectedCapturePromotions) {
        EXPECT_TRUE(containsMove(algebraicMoves, expectedMove)) << "Missing capture promotion " << expectedMove;
    }
}

TEST_F(MoveGenTest, EnPassantCaptureIsGeneratedForWhite) {
    const MoveGen::MoveList moveList = generatePseudoCaptures(WHITE_EN_PASSANT_FEN);
    ASSERT_EQ(moveList.size, 1);

    const Move move = moveList.moves[0];
    EXPECT_EQ(algebraic_move(move), "d5e6");
    EXPECT_EQ(move_special(move), SpecialMove::ENPASSANT);
}

TEST_F(MoveGenTest, EnPassantCaptureIsGeneratedForBlack) {
    const MoveGen::MoveList moveList = generatePseudoCaptures(BLACK_EN_PASSANT_FEN);
    ASSERT_EQ(moveList.size, 1);

    const Move move = moveList.moves[0];
    EXPECT_EQ(algebraic_move(move), "d4e3");
    EXPECT_EQ(move_special(move), SpecialMove::ENPASSANT);
}

TEST_F(MoveGenTest, WhiteCastlingMovesGeneratedWhenSquaresSafe) {
    const MoveGen::MoveList moveList = generatePseudoMoves(WHITE_CASTLING_FEN);

    int castleCount = 0;
    bool seenKingSideCastle = false;
    bool seenQueenSideCastle = false;

    for (int index = 0; index < moveList.size; ++index) {
        const Move move = moveList.moves[index];
        if (move_special(move) == SpecialMove::CASTLE) {
            ++castleCount;
            if (move_to(move) == SQ64_G1) {
                seenKingSideCastle = true;
            }
            if (move_to(move) == SQ64_C1) {
                seenQueenSideCastle = true;
            }
        }
    }

    EXPECT_EQ(castleCount, 2);
    EXPECT_TRUE(seenKingSideCastle);
    EXPECT_TRUE(seenQueenSideCastle);
}

TEST_F(MoveGenTest, WhiteCastlingMovesNotGeneratedWhenSquaresAttacked) {
    const MoveGen::MoveList moveList = generatePseudoMoves(WHITE_CASTLING_ATTACKED_FEN);

    bool castleFound = false;
    for (int index = 0; index < moveList.size; ++index) {
        const Move move = moveList.moves[index];
        if (move_special(move) == SpecialMove::CASTLE) {
            castleFound = true;
            break;
        }
    }

    EXPECT_FALSE(castleFound);
}
