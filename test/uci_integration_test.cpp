#include <algorithm>
#include <cctype>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "movegen.h"
#include "position.h"
#include "ttable.h"
#include "uci.h"
#include "helpers/test_helpers.h"

using namespace Akerbeltz;
using namespace TestHelpers;

namespace {

class UciIntegrationTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        init_engine_once();
        init_evaluate_once();
    }
};

std::string run_uci_session(const std::string& input) {
    auto* cin_buf = std::cin.rdbuf();
    auto* cin_tie = std::cin.tie(nullptr);

    std::istringstream in(input);
    std::cin.rdbuf(in.rdbuf());

    testing::internal::CaptureStdout();
    UCI::run();
    const std::string output = testing::internal::GetCapturedStdout();

    std::cin.rdbuf(cin_buf);
    std::cin.tie(cin_tie);
    std::cin.clear();
    return output;
}

std::string extract_bestmove(const std::string& output) {
    const std::string key = "bestmove ";
    const std::size_t pos = output.rfind(key);
    if (pos == std::string::npos) return {};
    const std::size_t start = pos + key.size();
    const std::size_t end = output.find_first_of(" \n\r\t", start);
    return output.substr(start, end == std::string::npos ? output.size() - start : end - start);
}

std::optional<int> extract_remaining_ms(const std::string& output) {
    std::istringstream iss(output);
    std::string token;
    std::optional<int> last;
    while (iss >> token) {
        if (token != "remaining") continue;
        std::string value;
        if (!(iss >> value)) break;
        if (value == "infinite") {
            last.reset();
            continue;
        }
        if (value.size() >= 2 && value.substr(value.size() - 2) == "ms") {
            value.resize(value.size() - 2);
        }
        const auto is_digit = [](unsigned char ch) { return std::isdigit(ch) != 0; };
        if (!value.empty() && std::all_of(value.begin(), value.end(), is_digit)) {
            last = std::stoi(value);
        }
    }
    return last;
}

const std::string kStartFen =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

std::string fen_line(const std::string& fen) {
    return "Fen: " + fen;
}

bool apply_uci_move(Position& position, const std::string& move_str) {
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

bool bestmove_is_legal(Position& position, const std::string& bestmove) {
    if (bestmove.empty() || bestmove == "0000") return false;
    MoveGen::MoveList moveList;
    MoveGen::generate_pseudo_moves(position, moveList);
    for (int i = 0; i < moveList.size; ++i) {
        Move move = moveList.moves[i];
        if (algebraic_move(move) != bestmove) continue;
        if (position.do_move(move)) {
            position.undo_move();
            return true;
        }
    }
    return false;
}

TEST_F(UciIntegrationTest, UciAndIsReadyProduceExpectedTokens) {
    const std::string output = run_uci_session("uci\nisready\nquit\n");
    EXPECT_NE(output.find("id name"), std::string::npos);
    EXPECT_NE(output.find("id author"), std::string::npos);
    EXPECT_NE(output.find("option name Hash"), std::string::npos);
    EXPECT_NE(output.find("uciok"), std::string::npos);
    EXPECT_NE(output.find("readyok"), std::string::npos);
}

TEST_F(UciIntegrationTest, SetoptionHashResizesTT) {
    TT::resize(64);
    const std::string output = run_uci_session("setoption name Hash value 16\nquit\n");
    EXPECT_NE(output.find("info string Hash set to 16 MB"), std::string::npos);
    EXPECT_EQ(TT::current_size_mb(), 16U);
}

TEST_F(UciIntegrationTest, SetoptionHashClampsToMin) {
    TT::resize(64);
    const std::string output = run_uci_session("setoption name Hash value 1\nquit\n");
    EXPECT_NE(output.find("info string Hash set to 4 MB"), std::string::npos);
    EXPECT_EQ(TT::current_size_mb(), 4U);
}

TEST_F(UciIntegrationTest, PositionStartposWithoutMoves) {
    const std::string output = run_uci_session("position startpos\nd\nquit\n");
    EXPECT_NE(output.find(fen_line(kStartFen)), std::string::npos);
}

TEST_F(UciIntegrationTest, PositionStartposMovesAndDisplayFen) {
    const std::string output =
        run_uci_session("position startpos moves e2e4 e7e5\nd\nquit\n");
    const std::string expected =
        "Fen: rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e6 0 2";
    EXPECT_NE(output.find(expected), std::string::npos);
}

TEST_F(UciIntegrationTest, PositionFenAndDisplayFen) {
    const std::string output =
        run_uci_session("position fen 4k3/8/8/8/8/8/8/4K3 w - - 0 1\nd\nquit\n");
    EXPECT_NE(output.find("Fen: 4k3/8/8/8/8/8/8/4K3 w - - 0 1"), std::string::npos);
}

TEST_F(UciIntegrationTest, PositionFenMovesUpdateState) {
    const std::string output = run_uci_session(
        "position fen 4k3/8/8/8/8/8/4P3/4K3 w - - 0 1 moves e2e4 e8e7\n"
        "d\nquit\n");
    EXPECT_NE(output.find(fen_line("8/4k3/8/8/4P3/8/8/4K3 w - - 1 2")),
              std::string::npos);
}

TEST_F(UciIntegrationTest, UciNewGameResetsPosition) {
    const std::string output = run_uci_session(
        "position startpos moves e2e4 e7e5\n"
        "d\n"
        "ucinewgame\n"
        "d\n"
        "quit\n");
    EXPECT_NE(output.find("Fen: rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e6 0 2"),
              std::string::npos);
    EXPECT_NE(output.find(fen_line(kStartFen)), std::string::npos);
}

TEST_F(UciIntegrationTest, GoOutputsLegalBestmove) {
    const std::string output =
        run_uci_session("position startpos moves e2e4 e7e5\ngo depth 1\nquit\n");
    const std::string bestmove = extract_bestmove(output);
    ASSERT_FALSE(bestmove.empty());

    Position position;
    position.set_FEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    ASSERT_TRUE(apply_uci_move(position, "e2e4"));
    ASSERT_TRUE(apply_uci_move(position, "e7e5"));
    EXPECT_TRUE(bestmove_is_legal(position, bestmove));
}

TEST_F(UciIntegrationTest, GoMovetimeOutputsFiniteBudget) {
    const std::string output =
        run_uci_session("position startpos\ngo movetime 10\nquit\n");
    EXPECT_NE(output.find("info string search depth "), std::string::npos);
    EXPECT_NE(output.find("remaining "), std::string::npos);
    EXPECT_NE(output.find("ms"), std::string::npos);
    EXPECT_EQ(output.find("remaining infinite"), std::string::npos);

    const std::string bestmove = extract_bestmove(output);
    ASSERT_FALSE(bestmove.empty());

    Position position;
    position.set_FEN(kStartFen);
    EXPECT_TRUE(bestmove_is_legal(position, bestmove));
}

TEST_F(UciIntegrationTest, GoUsesWtimeIncAndMovesToGoBudget) {
    const std::string output = run_uci_session(
        "position startpos\n"
        "go wtime 10000 btime 10000 winc 500 binc 0 movestogo 20\n"
        "stop\nquit\n");

    const auto remaining = extract_remaining_ms(output);
    ASSERT_TRUE(remaining.has_value());
    EXPECT_GT(*remaining, 700);
    EXPECT_LT(*remaining, 1500);
}

TEST_F(UciIntegrationTest, GoUsesBtimeWhenBlackToMove) {
    const std::string output = run_uci_session(
        "position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1\n"
        "go wtime 1000 btime 40000 winc 0 binc 0 movestogo 20\n"
        "stop\nquit\n");

    const auto remaining = extract_remaining_ms(output);
    ASSERT_TRUE(remaining.has_value());
    EXPECT_GT(*remaining, 500);
}

TEST_F(UciIntegrationTest, GoPerftReportsTotalNodes) {
    const std::string output = run_uci_session("position startpos\ngo perft 1\nquit\n");
    EXPECT_NE(output.find("total nodes size: 20"), std::string::npos);
}

}  // namespace
