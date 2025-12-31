#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "bitboards.h"
#include "movegen.h"
#include "search.h"
#include "position.h"
#include "helpers/test_helpers.h"

using namespace Akerbeltz;
using namespace TestHelpers;

namespace {

class SearchTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        init_engine_once();
        init_evaluate_once();
    }
};

std::string extract_bestmove_from_output(const std::string& output) {
    const std::string key = "bestmove ";
    const std::size_t pos = output.rfind(key);
    if (pos == std::string::npos) return {};

    const std::size_t start = pos + key.size();
    const std::size_t end = output.find_first_of(" \n\r\t", start);
    return output.substr(start, end == std::string::npos ? output.size() - start : end - start);
}

std::string run_search_output(const std::string& fen, DepthSize depth) {
    Position position;
    position.set_FEN(fen);

    Search::SearchInfo info{};
    info.depth = depth;
    info.nodes = 0;
    info.searchPly = 0;
    info.stop.store(false);
    info.timeManager.allocate_budget({});

    testing::internal::CaptureStdout();
    Search::search(position, info);
    return testing::internal::GetCapturedStdout();
}

std::string run_search_bestmove(const std::string& fen, DepthSize depth) {
    const std::string output = run_search_output(fen, depth);
    return extract_bestmove_from_output(output);
}

std::vector<int> extract_info_depths(const std::string& output) {
    std::vector<int> depths;
    std::istringstream iss(output);
    std::string line;
    while (std::getline(iss, line)) {
        std::istringstream ls(line);
        std::string token;
        ls >> token;
        if (token != "info") continue;
        ls >> token;
        if (token != "depth") continue;
        int depth = 0;
        if (ls >> depth) {
            depths.push_back(depth);
        }
    }
    return depths;
}

std::vector<std::string> extract_last_pv_moves(const std::string& output) {
    std::vector<std::string> pv_moves;
    std::istringstream iss(output);
    std::string line;
    while (std::getline(iss, line)) {
        std::istringstream ls(line);
        std::string token;
        bool saw_pv = false;
        std::vector<std::string> current;
        while (ls >> token) {
            if (saw_pv) {
                current.push_back(token);
            } else if (token == "pv") {
                saw_pv = true;
            }
        }
        if (saw_pv) {
            pv_moves = current;
        }
    }
    return pv_moves;
}

Square64 king_square(const Position& position, Color side) {
    const Bitboard king_bb = position.get_pieceTypes_bitboard(side, KING);
    return Square64(Bitboards::ctz(king_bb));
}

bool has_legal_move(Position& position) {
    MoveGen::MoveList moveList;
    MoveGen::generate_pseudo_moves(position, moveList);
    for (int i = 0; i < moveList.size; ++i) {
        Move move = moveList.moves[i];
        if (position.do_move(move)) {
            position.undo_move();
            return true;
        }
    }
    return false;
}

bool is_checkmate(Position& position) {
    const Color stm = position.get_side_to_move();
    const Square64 ksq = king_square(position, stm);
    if (!position.square_is_attacked_bySide(ksq, ~stm)) {
        return false;
    }
    return !has_legal_move(position);
}

std::vector<std::string> mate_in_one_moves(Position& position) {
    std::vector<std::string> result;
    MoveGen::MoveList moveList;
    MoveGen::generate_pseudo_moves(position, moveList);
    for (int i = 0; i < moveList.size; ++i) {
        Move move = moveList.moves[i];
        if (!position.do_move(move)) continue;
        if (is_checkmate(position)) {
            result.emplace_back(algebraic_move(move));
        }
        position.undo_move();
    }
    return result;
}

std::vector<std::string> mate_in_two_moves(Position& position) {
    std::vector<std::string> result;
    MoveGen::MoveList moveList;
    MoveGen::generate_pseudo_moves(position, moveList);

    for (int i = 0; i < moveList.size; ++i) {
        Move move = moveList.moves[i];
        if (!position.do_move(move)) continue;

        bool has_reply = false;
        bool all_replies_lose = true;
        MoveGen::MoveList replies;
        MoveGen::generate_pseudo_moves(position, replies);

        for (int r = 0; r < replies.size; ++r) {
            Move reply = replies.moves[r];
            if (!position.do_move(reply)) continue;
            has_reply = true;
            if (mate_in_one_moves(position).empty()) {
                all_replies_lose = false;
            }
            position.undo_move();
            if (!all_replies_lose) break;
        }

        position.undo_move();
        if (has_reply && all_replies_lose) {
            result.emplace_back(algebraic_move(move));
        }
    }
    return result;
}

bool apply_algebraic_move(Position& position, const std::string& move_str) {
    MoveGen::MoveList moveList;
    MoveGen::generate_pseudo_moves(position, moveList);
    for (int i = 0; i < moveList.size; ++i) {
        Move move = moveList.moves[i];
        if (algebraic_move(move) != move_str) continue;
        if (position.do_move(move)) {
            return true;
        }
    }
    return false;
}

bool bestmove_is_legal(const std::string& fen, const std::string& bestmove) {
    if (bestmove.empty() || bestmove == "0000") return false;
    Position position;
    position.set_FEN(fen);
    if (!apply_algebraic_move(position, bestmove)) return false;
    position.undo_move();
    return true;
}

TEST_F(SearchTest, ReturnsBestmove0000WhenNoLegalMoves) {
    const std::string bestmove =
        run_search_bestmove("7k/6Q1/6K1/8/8/8/8/8 b - - 0 1", 2);
    ASSERT_FALSE(bestmove.empty());
    EXPECT_EQ(bestmove, "0000");
}

TEST_F(SearchTest, FindsForcedOnlyMove) {
    const std::string bestmove =
        run_search_bestmove("8/8/8/8/8/8/7r/5k1K w - - 0 1", 2);
    ASSERT_FALSE(bestmove.empty());
    EXPECT_EQ(bestmove, "h1h2");
}

TEST_F(SearchTest, FindsMateInOne) {
    const std::string bestmove =
        run_search_bestmove("7k/8/5KQ1/8/8/8/8/8 w - - 0 1", 2);
    ASSERT_FALSE(bestmove.empty());
    EXPECT_EQ(bestmove, "g6g7");
}

TEST_F(SearchTest, MateInOneSelectsFromAcceptableMoves) {
    const std::string fen = "7k/5K2/6Q1/8/8/8/8/8 w - - 0 1";
    Position position;
    position.set_FEN(fen);
    const auto mates = mate_in_one_moves(position);
    ASSERT_FALSE(mates.empty());

    const std::string bestmove = run_search_bestmove(fen, 2);
    ASSERT_FALSE(bestmove.empty());
    EXPECT_NE(std::find(mates.begin(), mates.end(), bestmove), mates.end());
}

TEST_F(SearchTest, BestmoveIsLegalWhenMultipleOptions) {
    const std::string fen = "4k3/8/8/8/8/8/4P3/4K3 w - - 0 1";
    const std::string bestmove = run_search_bestmove(fen, 2);
    ASSERT_FALSE(bestmove.empty());
    EXPECT_TRUE(bestmove_is_legal(fen, bestmove));
}

TEST_F(SearchTest, MateInTwoSelectsFromAcceptableMoves) {
    const char* candidates[] = {
        "4r1k1/5ppp/4K3/7Q/8/8/8/7R w - - 0 1",
        "6k1/5ppp/6Q1/8/6K1/8/8/7R w - - 0 1",
        "6k1/6pp/6Q1/8/6K1/8/8/7R w - - 0 1",
        "4k3/8/4K3/6Q1/8/8/8/7R w - - 0 1"
    };

    std::string fen;
    std::vector<std::string> mate2;
    for (const auto* candidate : candidates) {
        Position position;
        position.set_FEN(candidate);
        if (!mate_in_one_moves(position).empty()) {
            continue;
        }
        mate2 = mate_in_two_moves(position);
        if (!mate2.empty()) {
            fen = candidate;
            break;
        }
    }

    if (fen.empty()) {
        GTEST_SKIP() << "No mate-in-two candidate produced a forced mate.";
    }

    const std::string bestmove = run_search_bestmove(fen, 4);
    ASSERT_FALSE(bestmove.empty());
    EXPECT_NE(std::find(mate2.begin(), mate2.end(), bestmove), mate2.end());
}

TEST_F(SearchTest, IterativeDeepeningReportsSequentialDepths) {
    const std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    const std::string output = run_search_output(fen, 3);
    const auto depths = extract_info_depths(output);

    std::vector<int> expected{1, 2, 3};
    EXPECT_EQ(depths, expected);
}

TEST_F(SearchTest, PvFirstMoveMatchesBestmoveAndIsLegal) {
    const std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    const std::string output = run_search_output(fen, 3);
    const std::string bestmove = extract_bestmove_from_output(output);
    ASSERT_FALSE(bestmove.empty());

    const auto pv_moves = extract_last_pv_moves(output);
    ASSERT_FALSE(pv_moves.empty());
    EXPECT_EQ(pv_moves.front(), bestmove);

    Position position;
    position.set_FEN(fen);
    for (const auto& move_str : pv_moves) {
        ASSERT_TRUE(apply_algebraic_move(position, move_str));
    }
}

TEST_F(SearchTest, PvIsLegalAcrossConsecutiveSearches) {
    const std::string fen1 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    const std::string out1 = run_search_output(fen1, 3);
    const auto pv1 = extract_last_pv_moves(out1);
    const std::string best1 = extract_bestmove_from_output(out1);
    ASSERT_FALSE(pv1.empty());
    EXPECT_EQ(pv1.front(), best1);

    Position pos1;
    pos1.set_FEN(fen1);
    for (const auto& move_str : pv1) {
        ASSERT_TRUE(apply_algebraic_move(pos1, move_str));
    }

    const std::string fen2 =
        "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1";
    const std::string out2 = run_search_output(fen2, 3);
    const auto pv2 = extract_last_pv_moves(out2);
    const std::string best2 = extract_bestmove_from_output(out2);
    ASSERT_FALSE(pv2.empty());
    EXPECT_EQ(pv2.front(), best2);

    Position pos2;
    pos2.set_FEN(fen2);
    for (const auto& move_str : pv2) {
        ASSERT_TRUE(apply_algebraic_move(pos2, move_str));
    }
}

}  // namespace
