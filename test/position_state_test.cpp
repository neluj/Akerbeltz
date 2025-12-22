#include <gtest/gtest.h>

#include "bitboards.h"
#include "move.h"
#include "position.h"
#include "helpers/test_helpers.h"

using namespace Xake;
using namespace TestHelpers;

namespace {

class PositionStateTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() { init_engine_once(); }
};

constexpr const char* START_FEN =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

TEST_F(PositionStateTest, FenRoundTripInitialPosition) {
    Position position;
    position.set_FEN(START_FEN);
    EXPECT_EQ(position.get_FEN(), START_FEN);
    EXPECT_EQ(position.get_side_to_move(), WHITE);
    EXPECT_EQ(position.get_castling_right(),
              CastlingRight::WKCA | CastlingRight::WQCA | CastlingRight::BKCA | CastlingRight::BQCA);
    EXPECT_EQ(position.get_enpassant_square(), SQ64_NO_SQUARE);
    EXPECT_EQ(position.get_fifty_moves_counter(), 0);
    EXPECT_EQ(position.get_moves_counter(), 1);
}

TEST_F(PositionStateTest, DoUndoQuietMoveRestoresState) {
    Position position;
    position.set_FEN(START_FEN);
    const Key key_before = position.get_key();
    const std::string fen_before = position.get_FEN();

    const Move move = make_quiet_move(SQ64_G1, SQ64_F3, SpecialMove::NO_SPECIAL);
    ASSERT_TRUE(position.do_move(move));
    EXPECT_EQ(position.get_side_to_move(), BLACK);
    EXPECT_EQ(position.get_mailbox_piece(SQ64_G1), Piece::NO_PIECE);
    EXPECT_EQ(position.get_mailbox_piece(SQ64_F3), Piece::W_KNIGHT);
    EXPECT_EQ(position.get_fifty_moves_counter(), 1);
    EXPECT_EQ(position.get_enpassant_square(), SQ64_NO_SQUARE);
    EXPECT_EQ(position.get_castling_right(),
              CastlingRight::WKCA | CastlingRight::WQCA | CastlingRight::BKCA | CastlingRight::BQCA);

    position.undo_move();
    EXPECT_EQ(position.get_key(), key_before);
    EXPECT_EQ(position.get_FEN(), fen_before);
}

TEST_F(PositionStateTest, PawnStartSetsEnPassantSquare) {
    Position position;
    position.set_FEN(START_FEN);

    const Move move = make_quiet_move(SQ64_E2, SQ64_E4, SpecialMove::PAWN_START);
    ASSERT_TRUE(position.do_move(move));
    EXPECT_EQ(position.get_enpassant_square(), SQ64_E3);
    EXPECT_EQ(position.get_fifty_moves_counter(), 0);
    EXPECT_EQ(position.get_FEN(),
              "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");

    position.undo_move();
    EXPECT_EQ(position.get_FEN(), START_FEN);
}

TEST_F(PositionStateTest, CaptureAndUndoRestoresState) {
    const char* fen = "4k3/8/8/5p2/4P3/8/8/4K3 w - - 0 1";
    Position position;
    position.set_FEN(fen);
    const Key key_before = position.get_key();

    const Move move = make_capture_move(SQ64_E4, SQ64_F5, SpecialMove::NO_SPECIAL, Piece::W_PAWN, Piece::B_PAWN);
    ASSERT_TRUE(position.do_move(move));
    EXPECT_EQ(position.get_mailbox_piece(SQ64_F5), Piece::W_PAWN);
    EXPECT_EQ(position.get_mailbox_piece(SQ64_E4), Piece::NO_PIECE);
    EXPECT_EQ(position.get_pieceTypes_bitboard(BLACK, PAWN), Bitboard{0});
    EXPECT_EQ(position.get_fifty_moves_counter(), 0);

    position.undo_move();
    EXPECT_EQ(position.get_key(), key_before);
    EXPECT_EQ(position.get_FEN(), fen);
}

TEST_F(PositionStateTest, EnPassantCaptureRemovesPawn) {
    const char* fen = "4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1";
    Position position;
    position.set_FEN(fen);
    const Key key_before = position.get_key();

    const Move move = make_enpassant_move(SQ64_E5, SQ64_D6);
    ASSERT_TRUE(position.do_move(move));
    EXPECT_EQ(position.get_mailbox_piece(SQ64_D6), Piece::W_PAWN);
    EXPECT_EQ(position.get_mailbox_piece(SQ64_E5), Piece::NO_PIECE);
    EXPECT_EQ(position.get_mailbox_piece(SQ64_D5), Piece::NO_PIECE);
    EXPECT_EQ(position.get_enpassant_square(), SQ64_NO_SQUARE);

    position.undo_move();
    EXPECT_EQ(position.get_key(), key_before);
    EXPECT_EQ(position.get_FEN(), fen);
}

TEST_F(PositionStateTest, CastlingMovesRookAndClearsRights) {
    const char* fen = "4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1";
    Position position;
    position.set_FEN(fen);
    const Key key_before = position.get_key();

    const Move move = make_quiet_move(SQ64_E1, SQ64_G1, SpecialMove::CASTLE);
    ASSERT_TRUE(position.do_move(move));
    EXPECT_EQ(position.get_mailbox_piece(SQ64_G1), Piece::W_KING);
    EXPECT_EQ(position.get_mailbox_piece(SQ64_F1), Piece::W_ROOK);
    EXPECT_EQ(position.get_castling_right(), CastlingRight::NO_RIGHT);

    position.undo_move();
    EXPECT_EQ(position.get_key(), key_before);
    EXPECT_EQ(position.get_FEN(), fen);
}

TEST_F(PositionStateTest, PromotionReplacesPawnAndUndoRestores) {
    const char* fen = "4k3/P7/8/8/8/8/8/4K3 w - - 0 1";
    Position position;
    position.set_FEN(fen);
    const Key key_before = position.get_key();

    const Move move = make_quiet_move(SQ64_A7, SQ64_A8, SpecialMove::PROMOTION_QUEEN);
    ASSERT_TRUE(position.do_move(move));
    EXPECT_EQ(position.get_mailbox_piece(SQ64_A8), Piece::W_QUEEN);
    EXPECT_EQ(position.get_mailbox_piece(SQ64_A7), Piece::NO_PIECE);

    position.undo_move();
    EXPECT_EQ(position.get_key(), key_before);
    EXPECT_EQ(position.get_FEN(), fen);
}

TEST_F(PositionStateTest, NullMoveTogglesSideAndClearsEnPassant) {
    const char* fen = "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1";
    Position position;
    position.set_FEN(fen);
    const Key key_before = position.get_key();

    position.do_null_move();
    EXPECT_EQ(position.get_side_to_move(), WHITE);
    EXPECT_EQ(position.get_enpassant_square(), SQ64_NO_SQUARE);
    EXPECT_EQ(position.get_moves_counter(), 2);

    position.undo_null_move();
    EXPECT_EQ(position.get_key(), key_before);
    EXPECT_EQ(position.get_FEN(), fen);
}

}  // namespace
