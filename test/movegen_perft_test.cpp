#include <algorithm>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "movegen.h"
#include "position.h"
#include "helpers/test_helpers.h"

using namespace Akerbeltz;
using namespace TestHelpers;

namespace {

NodesSize perft(Position& position, DepthSize depth) {
    if (depth == 0) return 1;
    MoveGen::MoveList moveList;
    MoveGen::generate_pseudo_moves(position, moveList);
    NodesSize nodes = 0;
    for (int i = 0; i < moveList.size; ++i) {
        Move move = moveList.moves[i];
        if (!position.do_move(move)) continue;
        nodes += perft(position, depth - 1);
        position.undo_move();
    }
    return nodes;
}

std::vector<std::string> moves_to_algebraic(const MoveGen::MoveList& moveList) {
    std::vector<std::string> result;
    result.reserve(moveList.size);
    for (int i = 0; i < moveList.size; ++i) {
        result.emplace_back(algebraic_move(moveList.moves[i]));
    }
    return result;
}

bool contains_move(const std::vector<std::string>& moves, const std::string& target) {
    return std::find(moves.begin(), moves.end(), target) != moves.end();
}

class MoveGenPerftTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() { init_engine_once(); }
};

TEST_F(MoveGenPerftTest, StartPositionPerftDepth2) {
    Position position;
    position.set_FEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    EXPECT_EQ(perft(position, 2), 400);
}

TEST_F(MoveGenPerftTest, StartPositionPerftDepth3) {
    Position position;
    position.set_FEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    EXPECT_EQ(perft(position, 3), 8902);
}

TEST_F(MoveGenPerftTest, KiwipetePerftDepth2) {
    Position position;
    position.set_FEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    EXPECT_EQ(perft(position, 2), 2039);
}

TEST_F(MoveGenPerftTest, GeneratesEnPassantMove) {
    Position position;
    position.set_FEN("4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1");
    MoveGen::MoveList moveList;
    MoveGen::generate_pseudo_moves(position, moveList);
    const auto moves = moves_to_algebraic(moveList);
    EXPECT_TRUE(contains_move(moves, "e5d6"));
}

TEST_F(MoveGenPerftTest, GeneratesCastlingMovesWhenLegal) {
    Position position;
    position.set_FEN("4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1");
    MoveGen::MoveList moveList;
    MoveGen::generate_pseudo_moves(position, moveList);
    const auto moves = moves_to_algebraic(moveList);
    EXPECT_TRUE(contains_move(moves, "e1g1"));
    EXPECT_TRUE(contains_move(moves, "e1c1"));
}

}  // namespace
