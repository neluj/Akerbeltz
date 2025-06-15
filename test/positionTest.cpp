#include <gtest/gtest.h>
#include "position.h"
//TODO use gmock
using namespace Xake;

class PositionTest : public ::testing::Test
{
     protected:
     virtual void SetUp()
     {  
        Position::init();    
        Evaluate::init();
     }

     virtual void TearDown()
    {
    }
    public:

    Position position;
    const std::string FEN_INIT_POSITION = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
};

TEST_F(PositionTest, RandomSetPosition){
    const std::string RANDOM_FEN_POSITION = "rnbqk3/PPpBpppP/8/1Q2N3/8/7K/R15p/8 w KQkq c3 34 1";
    //Position position;

    position.set_FEN(RANDOM_FEN_POSITION);

    //TODO UT for mailbox
    EXPECT_EQ(position.get_mailbox_pieceType(WHITE, SQ64_A7), PAWN);
    EXPECT_EQ(position.get_mailbox_pieceType(COLOR_NC, SQ64_A7), PAWN);
    EXPECT_EQ(position.get_side_to_move(), WHITE);

    EXPECT_EQ(position.get_castling_right(), 0xF);
    
    EXPECT_EQ(position.get_enpassant_square(), SQ64_C3);
    
    EXPECT_EQ(position.get_fifty_moves_counter(), 34);
        
    EXPECT_EQ(position.get_moves_counter(), 1);
}
/*
TEST_F(PositionTest, SquareAttacked){

    //Position position;

    //Attacks

    const std::string ATTACK_ROOK_POSITION = "8/8/8/8/5r2/8/8/2R2P2 w - - 0 1";

    position.set_FEN(ATTACK_ROOK_POSITION);

    ASSERT_EQ(position.square_is_attacked(Square64::SQ64_F1), true);

    const std::string ATTACK_BISHOP_POSITION = "8/8/8/8/8/3B3b/8/5P2 w - - 0 1";

    position.set_FEN(ATTACK_BISHOP_POSITION);

    ASSERT_EQ(position.square_is_attacked(Square64::SQ64_F1), true);

    const std::string ATTACK_QUEEN_POSITION = "8/8/8/8/5q2/8/8/2R2P2 w - - 0 1";

    position.set_FEN(ATTACK_QUEEN_POSITION);

    ASSERT_EQ(position.square_is_attacked(Square64::SQ64_F1), true);

    const std::string ATTACK_KNIGHT_POSITION = "8/8/8/8/8/3B4/7n/5P2 w - - 0 1";

    position.set_FEN(ATTACK_KNIGHT_POSITION);

    ASSERT_EQ(position.square_is_attacked(Square64::SQ64_F1), true);

    //No attacks

    const std::string NON_ATTACK_ROOK_POSITION = "8/8/8/8/5R2/8/8/2R2P2 w - - 0 1";

    position.set_FEN(NON_ATTACK_ROOK_POSITION);

    ASSERT_EQ(position.square_is_attacked(Square64::SQ64_F1), false);

    const std::string NON_ATTACK_BISHOP_POSITION = "8/8/8/8/8/3B3B/8/5P2 w - - 0 1";

    position.set_FEN(NON_ATTACK_BISHOP_POSITION);

    ASSERT_EQ(position.square_is_attacked(Square64::SQ64_F1), false);

    const std::string NON_ATTACK_QUEEN_POSITION = "8/8/8/8/5Q2/8/8/2R2P2 w - - 0 1";

    position.set_FEN(NON_ATTACK_QUEEN_POSITION);

    ASSERT_EQ(position.square_is_attacked(Square64::SQ64_F1), false);

    const std::string NON_ATTACK_KNIGHT_POSITION = "8/8/8/8/8/3B4/7N/5P2 w - - 0 1";

    position.set_FEN(NON_ATTACK_KNIGHT_POSITION);

    ASSERT_EQ(position.square_is_attacked(Square64::SQ64_F1), false);
}
*/
TEST_F(PositionTest, MovePiece){

    position.set_FEN(FEN_INIT_POSITION);

    //1 Check if the square D2 is taken by a pawn and D4 is empty

    //mailbox
    PieceType pieceTypewpbd2 = position.get_mailbox_pieceType(Color::WHITE, Square64::SQ64_D2);
    PieceType pieceTypencbd2 = position.get_mailbox_pieceType(Color::COLOR_NC, Square64::SQ64_D2);

    ASSERT_EQ(pieceTypewpbd2, PieceType::PAWN);
    ASSERT_EQ(pieceTypencbd2, PieceType::PAWN);

    PieceType pieceTypewpbd4 = position.get_mailbox_pieceType(Color::WHITE, Square64::SQ64_D4);
    PieceType pieceTypencbd4 = position.get_mailbox_pieceType(Color::WHITE, Square64::SQ64_D4);

    ASSERT_EQ(pieceTypewpbd4, PieceType::NO_PIECE_TYPE);
    ASSERT_EQ(pieceTypencbd4, PieceType::NO_PIECE_TYPE);

    //2 move piece from D2 to D4
    position.move_piece(Square64::SQ64_D2, Square64::SQ64_D4);

    //3 Check D2 square

    //mailbox
    PieceType pieceTypewpad2 = position.get_mailbox_pieceType(Color::WHITE, Square64::SQ64_D2);
    PieceType pieceTypencad2 = position.get_mailbox_pieceType(Color::COLOR_NC, Square64::SQ64_D2);

    ASSERT_EQ(pieceTypewpad2, PieceType::NO_PIECE_TYPE);
    ASSERT_EQ(pieceTypencad2, PieceType::NO_PIECE_TYPE);

    PieceType pieceTypewpad4 = position.get_mailbox_pieceType(Color::WHITE, Square64::SQ64_D4);
    PieceType pieceTypencad4 = position.get_mailbox_pieceType(Color::WHITE, Square64::SQ64_D4);

    ASSERT_EQ(pieceTypewpad4, PieceType::PAWN);
    ASSERT_EQ(pieceTypencad4, PieceType::PAWN);  

}

TEST_F(PositionTest, RemovePiece){

    position.set_FEN(FEN_INIT_POSITION);

    //1 Check if the square D2 is taken by a pawn

    //mailbox
    PieceType pieceTypewp = position.get_mailbox_pieceType(Color::WHITE, Square64::SQ64_D2);
    PieceType pieceTypep = position.get_mailbox_pieceType(Color::COLOR_NC, Square64::SQ64_D2);

    ASSERT_EQ(pieceTypewp, PieceType::PAWN);
    ASSERT_EQ(pieceTypep, PieceType::PAWN);

    EXPECT_EQ(position.get_material_score(Color::WHITE), 23892);

    //2 Remove piece located on D2
    position.remove_piece(Square64::SQ64_D2);

    //3 Check D2 square

    //mailbox
    PieceType pieceTypenptw = position.get_mailbox_pieceType(Color::WHITE, Square64::SQ64_D2);
    PieceType pieceTypenptn = position.get_mailbox_pieceType(Color::COLOR_NC, Square64::SQ64_D2);

    ASSERT_EQ(pieceTypenptw, PieceType::NO_PIECE_TYPE);   
    ASSERT_EQ(pieceTypenptn, PieceType::NO_PIECE_TYPE);

    EXPECT_EQ(position.get_material_score(Color::WHITE), 23833);

}
 
TEST_F(PositionTest, AddPiece){

    //Position position;

    const std::string FEN_INIT_NOPAWN_POSITION = "rnbqkbnr/pppppppp/8/8/8/8/PPP1PPPP/RNBQKBNR w KQkq - 0 1";

    position.set_FEN(FEN_INIT_NOPAWN_POSITION);

    //1 Check if the square D2 is empty

    //mailbox
    PieceType pieceTypewp = position.get_mailbox_pieceType(Color::WHITE, Square64::SQ64_D2);
    PieceType pieceTypep = position.get_mailbox_pieceType(Color::COLOR_NC, Square64::SQ64_D2);

    ASSERT_EQ(pieceTypewp, PieceType::NO_PIECE_TYPE);
    ASSERT_EQ(pieceTypep, PieceType::NO_PIECE_TYPE);

    EXPECT_EQ(position.get_material_score(Color::WHITE), 23833);

    //2 Add piece on D2
    position.add_piece(Square64::SQ64_D2, Piece::W_PAWN);

    //3 Check D2 square

    //mailbox
    PieceType pieceTypenptw = position.get_mailbox_pieceType(Color::WHITE, Square64::SQ64_D2);
    PieceType pieceTypenptn = position.get_mailbox_pieceType(Color::COLOR_NC, Square64::SQ64_D2);

    ASSERT_EQ(pieceTypenptw, PieceType::PAWN);   
    ASSERT_EQ(pieceTypenptn, PieceType::PAWN);

    EXPECT_EQ(position.get_material_score(Color::WHITE), 23892);

}
/*
TEST_F(PositionTest, DoMove){

    position.set_FEN(FEN_INIT_POSITION);

    EXPECT_EQ(position.get_side_to_move(), Color::WHITE);
    EXPECT_EQ(position.get_castling_right(), (CastlingRight::WKCA | CastlingRight::WQCA | CastlingRight::BKCA | CastlingRight::BQCA));
    EXPECT_EQ(position.get_enpassant_square(), Square64::SQ64_NO_SQUARE);
    EXPECT_EQ(position.get_fifty_moves_counter(), 0);
    EXPECT_EQ(position.get_moves_counter(), 1);
    EXPECT_EQ(position.get_mailbox_pieceType(Color::WHITE, Square64::SQ64_B2), PieceType::PAWN);
    EXPECT_EQ(position.get_mailbox_pieceType(Color::WHITE, Square64::SQ64_B4), PieceType::NO_PIECE_TYPE);
    EXPECT_EQ(position.get_material_score(Color::WHITE), 23892);

    Move moveb2b4 = make_quiet_move(Square64::SQ64_B2, Square64::SQ64_B4, SpecialMove::PAWN_START);    
    position.do_move(moveb2b4);

    EXPECT_EQ(position.get_side_to_move(), Color::BLACK);
    EXPECT_EQ(position.get_castling_right(), (CastlingRight::WKCA | CastlingRight::WQCA | CastlingRight::BKCA | CastlingRight::BQCA));
    EXPECT_EQ(position.get_enpassant_square(), Square64::SQ64_B3);
    EXPECT_EQ(position.get_fifty_moves_counter(), 0);
    EXPECT_EQ(position.get_moves_counter(), 1);
    EXPECT_EQ(position.get_mailbox_pieceType(Color::WHITE, Square64::SQ64_B2), PieceType::NO_PIECE_TYPE);
    EXPECT_EQ(position.get_mailbox_pieceType(Color::WHITE, Square64::SQ64_B4), PieceType::PAWN);
    EXPECT_EQ(position.get_material_score(Color::WHITE), 23891);

}
*/

/*
TEST_F(PositionTest, UndoMove){

    position.set_FEN(FEN_INIT_POSITION);

    EXPECT_EQ(position.get_side_to_move(), Color::WHITE);
    EXPECT_EQ(position.get_castling_right(), (CastlingRight::WKCA | CastlingRight::WQCA | CastlingRight::BKCA | CastlingRight::BQCA));
    EXPECT_EQ(position.get_enpassant_square(), Square64::SQ64_NO_SQUARE);
    EXPECT_EQ(position.get_fifty_moves_counter(), 0);
    EXPECT_EQ(position.get_moves_counter(), 1);
    EXPECT_EQ(position.get_mailbox_pieceType(Color::WHITE, Square64::SQ64_B2), PieceType::PAWN);
    EXPECT_EQ(position.get_mailbox_pieceType(Color::WHITE, Square64::SQ64_B4), PieceType::NO_PIECE_TYPE);
    EXPECT_EQ(position.get_material_score(Color::WHITE), 23892);

    Move moveb2b4 = make_quiet_move(Square64::SQ64_B2, Square64::SQ64_B4, SpecialMove::PAWN_START); 

    //TODO do this using mocks on undo move   
    position.do_move(moveb2b4);
    position.undo_move();

    EXPECT_EQ(position.get_side_to_move(), Color::WHITE);
    EXPECT_EQ(position.get_castling_right(), (CastlingRight::WKCA | CastlingRight::WQCA | CastlingRight::BKCA | CastlingRight::BQCA));
    EXPECT_EQ(position.get_enpassant_square(), Square64::SQ64_NO_SQUARE);
    EXPECT_EQ(position.get_fifty_moves_counter(), 0);
    EXPECT_EQ(position.get_moves_counter(), 1);
    EXPECT_EQ(position.get_mailbox_pieceType(Color::WHITE, Square64::SQ64_B2), PieceType::PAWN);
    EXPECT_EQ(position.get_mailbox_pieceType(Color::WHITE, Square64::SQ64_B4), PieceType::NO_PIECE_TYPE);
    EXPECT_EQ(position.get_material_score(Color::WHITE), 23892);

}

*/

/*
//TODO ajustar esto...que se quiere probar? y que se quiere obtener? probar uno por uno...esto seria mas un test complementario, no unitario
TEST_F(PositionTest, CalcKeyDoUndoBasicMoveWhite){

    //Position position;

    const std::string POS1 = "3kb3/3pp3/8/8/8/8/4PP2/4KB2 w - - 0 1";
    position.set_FEN(POS1);
    Key keyPos1Before = position.get_key();

    Move move = make_quiet_move(SQ64_F1, SQ64_G2, NO_SPECIAL);

    position.do_move(move);

    Key keyPos1Aft = position.get_key();

    position.undo_move();
    Key keyPos1BeforeUndo = position.get_key();
    EXPECT_EQ(keyPos1Before, keyPos1BeforeUndo);

    const std::string POS2 = "3kb3/3pp3/8/8/8/8/4PPB1/4K3 b - - 0 1";
    position.set_FEN(POS2);
    Key keyPos2Bef = position.get_key();
    
    EXPECT_EQ(keyPos1Aft, keyPos2Bef);    

}
*/

/*
TEST_F(PositionTest, CalcKeyDoUndoBasicMoveBlack){

    //Position position;

    const std::string POS1 = "3kb3/3pp3/8/8/8/8/4PP2/4KB2 b - - 0 1";
    position.set_FEN(POS1);
    Key keyPos1Before = position.get_key();

    Move move = make_quiet_move(SQ64_E8, SQ64_F7, NO_SPECIAL);

    position.do_move(move);

    Key keyPos1Aft = position.get_key();

    position.undo_move();
    Key keyPos1BeforeUndo = position.get_key();
    EXPECT_EQ(keyPos1Before, keyPos1BeforeUndo);

    const std::string POS2 = "3k4/3ppb2/8/8/8/8/4PP2/4KB2 w - - 0 1";
    position.set_FEN(POS2);
    Key keyPos2Bef = position.get_key();
    
    EXPECT_EQ(keyPos1Aft, keyPos2Bef);    
}
*/

/*
TEST_F(PositionTest, CalcKeyBasicCapture){
    const std::string POS1 = "3kb3/4p3/8/3p4/4P3/8/5P2/4KB2 w - - 0 1";

    position.set_FEN(POS1);
    Key keyBeforeCapture = position.get_key();

    Move captureMove = make_capture_move(SQ64_E4, SQ64_D5, NO_SPECIAL, W_PAWN, B_PAWN);

    position.do_move(captureMove);

    Key keyAfterCapture = position.get_key();

    position.undo_move();
    Key keyUndoCapture = position.get_key();

    //position key before move and after undo move must be same
    EXPECT_EQ(keyBeforeCapture, keyUndoCapture);

    const std::string POS2 = "3kb3/4p3/8/3P4/8/8/5P2/4KB2 b - - 0 1";
    position.set_FEN(POS2);
    Key keyNew= position.get_key();

    //position ley after move and POS2 must be same
    EXPECT_EQ(keyNew, keyAfterCapture);
     
 
}
*/
/*
TEST_F(PositionTest, CalcKeyDoUndoEnpassant){

    const std::string POS1 = "3kb3/8/8/8/4p3/8/3PPP2/4KB2 w - - 0 1";
    position.set_FEN(POS1);
    Key keyBeforeMovePawnstart = position.get_key();
    Move movePawnstart = make_quiet_move(SQ64_D2, SQ64_D4, PAWN_START);

    position.do_move(movePawnstart);
    Key keyAfterMovePawnstart = position.get_key();

    position.undo_move();
    Key keyAfterUndomovePawnstart = position.get_key();

    //making move and unmaking it with pawn start, the key must be the same
    EXPECT_EQ(keyBeforeMovePawnstart, keyAfterUndomovePawnstart);

    const std::string POS2 = "3kb3/8/8/8/3Pp3/8/4PP2/4KB2 b - d3 0 1";
    position.set_FEN(POS2);

    Key keyPawnstart = position.get_key();
    
    //making move with pawn start must be pos2's same key
    EXPECT_EQ(keyAfterMovePawnstart, keyPawnstart);
    //BUG si es enpassant move, no hayt que poner la pieza comida, o crear una nueva función para make_enpassant_move
    Move moveEnpassant = make_enpassant_move(SQ64_E4, SQ64_D3);

    position.do_move(moveEnpassant);
    Key keyAfterMoveEnpassant = position.get_key();

    position.undo_move();
    Key keyBeforeMoveEnpassant = position.get_key();

    //making and unmaking enpassant capture move, the key must be the same
    EXPECT_EQ(keyPawnstart, keyBeforeMoveEnpassant);

    const std::string POS3= "3kb3/8/8/8/8/3p4/4PP2/4KB2 w - - 0 1";
    position.set_FEN(POS3);

    Key keyEnpassant = position.get_key();

    //making enpsassant move must be pos3's same key
    EXPECT_EQ(keyAfterMoveEnpassant, keyEnpassant);

}
*/
/*
TEST_F(PositionTest, CalcKeyBasicCastling){

    ////////////////////////
    /////////White/////////
    ///////////////////////

    const std::string POS_W_CAST = "rnbqkbnr/pppppppp/8/8/8/BPN2NP1/PQPPPPBP/R3K2R w KQkq - 0 1"; 

    ////////////////////////
    //White King Castling//
    ///////////////////////
    position.set_FEN(POS_W_CAST);

    Key keyBeforetWhiteCastling = position.get_key();

    Move moveWhiteKCasling = make_quiet_move(SQ64_E1, SQ64_G1, CASTLE);

    position.do_move(moveWhiteKCasling);
    Key keyAftertWhiteKCastling = position.get_key();

    position.undo_move();
    Key keyAftertWhiteKCastlingUndo = position.get_key();

    //Position key before do King Cast and after undo must be same
    EXPECT_EQ(keyBeforetWhiteCastling, keyAftertWhiteKCastlingUndo);

    const std::string POS_W_K_CAST = "rnbqkbnr/pppppppp/8/8/8/BPN2NP1/PQPPPPBP/R4RK1 b kq - 1 1"; 
    position.set_FEN(POS_W_K_CAST);

    Key keyWhiteKCastling = position.get_key();

    //Position key before do King Cast and new position must be same
    EXPECT_EQ(keyWhiteKCastling, keyAftertWhiteKCastling);
    
    ////////////////////////
    //White Queen Castling//
    ////////////////////////
    position.set_FEN(POS_W_CAST);

    Move moveWhiteQCasling = make_quiet_move(SQ64_E1, SQ64_C1, CASTLE);

    position.do_move(moveWhiteQCasling);
    Key keyAftertWhiteQCastling = position.get_key();

    position.undo_move();
    Key keyAftertWhiteQCastlingUndo = position.get_key();

    //Position key before do Queen Cast and after undo must be same
    EXPECT_EQ(keyBeforetWhiteCastling, keyAftertWhiteQCastlingUndo);
    
    const std::string POS_W_Q_CAST = "rnbqkbnr/pppppppp/8/8/8/BPN2NP1/PQPPPPBP/2KR3R b kq - 1 1";
    position.set_FEN(POS_W_Q_CAST);

    Key keyWhiteQCastling = position.get_key();

    //Position key before do Queen Cast and new position must be same
    EXPECT_EQ(keyAftertWhiteQCastling, keyWhiteQCastling);

    ////////////////////////
    /////////Black/////////
    ///////////////////////

    const std::string POS_B_CAST = "r3k2r/pqppppbp/bpn2np1/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1";

    ////////////////////////
    //Black King Castling//
    ///////////////////////

    position.set_FEN(POS_B_CAST);

    Key keyBeforetBlackCastling = position.get_key();

    Move moveBlackKCasling = make_quiet_move(SQ64_E8, SQ64_G8, CASTLE);

    position.do_move(moveBlackKCasling);
    Key keyAftertBlackKCastling = position.get_key();

    position.undo_move();
    Key keyAftertBlackKCastlingUndo = position.get_key();

    //Position key before do King Cast and after undo must be same
    EXPECT_EQ(keyBeforetBlackCastling, keyAftertBlackKCastlingUndo);

    const std::string POS_B_K_CAST = "r4rk1/pqppppbp/bpn2np1/8/8/8/PPPPPPPP/RNBQKBNR w KQ - 1 2"; 
    position.set_FEN(POS_B_K_CAST);

    Key keyBlackKCastling = position.get_key();

    //Position key before do King Cast and new position must be same
    EXPECT_EQ(keyBlackKCastling, keyAftertBlackKCastling);

    ////////////////////////
    //Black Queen Castling//
    ////////////////////////

    position.set_FEN(POS_B_CAST);

    Move moveBlackQCasling = make_quiet_move(SQ64_E8, SQ64_C8, CASTLE);

    position.do_move(moveBlackQCasling);
    Key keyAftertBlackQCastling = position.get_key();

    position.undo_move();
    Key keyAftertBlackQCastlingUndo = position.get_key();

    //Position key before do Queen Cast and after undo must be same
    EXPECT_EQ(keyBeforetBlackCastling, keyAftertBlackQCastlingUndo);
    
    const std::string POSBQCAST = "2kr3r/pqppppbp/bpn2np1/8/8/8/PPPPPPPP/RNBQKBNR w KQ - 1 2 ";
    position.set_FEN(POSBQCAST);

    Key keyBlackQCastling = position.get_key();

    //Position key before do Queen Cast and new position must be same
    EXPECT_EQ(keyAftertBlackQCastling, keyBlackQCastling);
    
 
}
*/
/*
TEST_F(PositionTest, CalcKeyBasicPromotion){

    ////////////////////////
    /////////White/////////
    ///////////////////////

    const std::string POS_W = "1bk3r1/p6P/1p6/8/8/8/8/3NK3 w - - 0 1";
    position.set_FEN(POS_W);

    Key keyWhitePromotionBefore = position.get_key();
    
    Move moveWPromBishop = make_capture_move(SQ64_H7, SQ64_G8, PROMOTION_BISHOP, W_PAWN, B_ROOK);

    position.do_move(moveWPromBishop);
    Key keyWhitePromotionAfter = position.get_key();
    position.undo_move();
    Key keyWhitePromotionAfterUndo = position.get_key();

    EXPECT_EQ(keyWhitePromotionBefore, keyWhitePromotionAfterUndo);

    const std::string POS_W_PROM = "1bk3B1/p7/1p6/8/8/8/8/3NK3 b - - 0 1";
    position.set_FEN(POS_W_PROM);
     
    Key keyWhitePromotion = position.get_key();

    EXPECT_EQ(keyWhitePromotion, keyWhitePromotionAfter);

    ////////////////////////
    /////////Black/////////
    ///////////////////////
    
    const std::string POS_B = "3nk3/8/8/8/8/1P6/P6p/1BK3R1 b - - 0 1";
    position.set_FEN(POS_B);

    Key keyBlackPromotionBefore = position.get_key();
    
    Move moveBPromBishop = make_capture_move(SQ64_H2, SQ64_G1, PROMOTION_BISHOP, B_PAWN, W_ROOK);

    position.do_move(moveBPromBishop);
    Key keyBlackPromotionAfter = position.get_key();
    position.undo_move();
    Key keyBlackPromotionAfterUndo = position.get_key();

    EXPECT_EQ(keyBlackPromotionBefore, keyBlackPromotionAfterUndo);

    const std::string POS_B_PROM = "3nk3/8/8/8/8/1P6/P7/1BK3b1 w - - 0 1";
    position.set_FEN(POS_B_PROM);
     
    Key keyBlackPromotion = position.get_key();

    EXPECT_EQ(keyBlackPromotion, keyBlackPromotionAfter);

}
*/
/*
TEST_F(PositionTest, IsRepetition){

    position.set_FEN(FEN_INIT_POSITION);

    Move move_wk = make_quiet_move(SQ64_B1, SQ64_A3, NO_SPECIAL);
    position.do_move(move_wk);
    EXPECT_EQ(position.is_repetition(), false);

    Move move_bk = make_quiet_move(SQ64_B8, SQ64_A6 , NO_SPECIAL);
    position.do_move(move_bk);
    EXPECT_EQ(position.is_repetition(), false);

    Move unmove_wk = make_quiet_move(SQ64_A3, SQ64_B1, NO_SPECIAL);
    position.do_move(unmove_wk);
    EXPECT_EQ(position.is_repetition(), false);

    Move unmove_bk = make_quiet_move(SQ64_A6, SQ64_B8, NO_SPECIAL);
    position.do_move(unmove_bk);
    EXPECT_EQ(position.is_repetition(), true);

}
*/