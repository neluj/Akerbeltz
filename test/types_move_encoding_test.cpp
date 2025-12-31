#include <gtest/gtest.h>

#include "move.h"
#include "types.h"

using namespace Akerbeltz;

namespace {

TEST(TypesTest, ColorAndPieceHelpersRoundTrip) {
    EXPECT_EQ(~Color::WHITE, Color::BLACK);
    EXPECT_EQ(~Color::BLACK, Color::WHITE);

    for (Color color : {Color::WHITE, Color::BLACK}) {
        for (int pt = PieceType::PAWN; pt <= PieceType::KING; ++pt) {
            const auto pieceType = static_cast<PieceType>(pt);
            const Piece piece = make_piece(color, pieceType);
            EXPECT_EQ(piece_color(piece), color);
            EXPECT_EQ(piece_type(piece), pieceType);
        }
    }
}

TEST(TypesTest, SquareArithmeticMatchesFileAndRank) {
    for (int rank = Rank::RANK_1; rank <= Rank::RANK_8; ++rank) {
        for (int file = File::FILE_A; file <= File::FILE_H; ++file) {
            const auto r = static_cast<Rank>(rank);
            const auto f = static_cast<File>(file);
            const Square64 square = make_square64(r, f);
            EXPECT_EQ(square_rank(square), r);
            EXPECT_EQ(square_file(square), f);
            EXPECT_EQ(SQUARE_NAMES[square][0], 'a' + file);
        }
    }
}

TEST(MoveEncodingTest, QuietMoveEncodesAndDecodesFields) {
    const Move move = make_quiet_move(SQ64_B2, SQ64_B4, SpecialMove::PAWN_START);
    EXPECT_EQ(move_from(move), SQ64_B2);
    EXPECT_EQ(move_to(move), SQ64_B4);
    EXPECT_EQ(move_special(move), SpecialMove::PAWN_START);
    EXPECT_EQ(captured_piece(move), Piece::NO_PIECE);
    EXPECT_EQ(attacker_piece(move), Piece::NO_PIECE);
    EXPECT_FALSE(is_capture(move));
    EXPECT_EQ(algebraic_move(move), "b2b4");
}

TEST(MoveEncodingTest, CaptureMoveEncodesPiecesAndSpecial) {
    const Move move = make_capture_move(
        SQ64_E4, SQ64_F5, SpecialMove::NO_SPECIAL, Piece::W_PAWN, Piece::B_KNIGHT);

    EXPECT_EQ(move_from(move), SQ64_E4);
    EXPECT_EQ(move_to(move), SQ64_F5);
    EXPECT_EQ(move_special(move), SpecialMove::NO_SPECIAL);
    EXPECT_EQ(attacker_piece(move), Piece::W_PAWN);
    EXPECT_EQ(captured_piece(move), Piece::B_KNIGHT);
    EXPECT_TRUE(is_capture(move));
    EXPECT_EQ(algebraic_move(move), "e4f5");
}

TEST(MoveEncodingTest, EnPassantIsCaptureWithoutCapturedPieceField) {
    const Move move = make_enpassant_move(SQ64_E5, SQ64_D6);
    EXPECT_EQ(move_special(move), SpecialMove::ENPASSANT);
    EXPECT_EQ(captured_piece(move), Piece::NO_PIECE);
    EXPECT_TRUE(is_capture(move));
}

TEST(MoveEncodingTest, PromotionAddsSuffixToAlgebraic) {
    const Move move = make_capture_move(
        SQ64_A7, SQ64_B8, SpecialMove::PROMOTION_QUEEN, Piece::W_PAWN, Piece::B_ROOK);
    EXPECT_EQ(move_special(move), SpecialMove::PROMOTION_QUEEN);
    EXPECT_EQ(promoted_piece(move), PieceType::QUEEN);
    EXPECT_EQ(algebraic_move(move), "a7b8q");
}

TEST(MoveEncodingTest, RawMoveIgnoresHeuristicScore) {
    Move base = make_quiet_move(SQ64_D2, SQ64_D4, SpecialMove::PAWN_START);
    const Move scored = set_heuristic_score(base, PV_SCORE | (MoveScore{123} << CAP_SHIFT));
    EXPECT_NE(base, scored);
    EXPECT_TRUE(equal_move(base, scored));
    EXPECT_EQ(raw_move(base), raw_move(scored));
}

}  // namespace
