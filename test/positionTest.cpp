#include <gtest/gtest.h>
#include "position.h"
//TODO use gmock
using namespace Xake;

TEST(PositionTest, RandomSetPosition){
    const std::string RANDOM_FEN_POSITION = "rnbqk3/PPpBpppP/8/1Q2N3/8/7K/R15p/8 w KQkq c3 34 1";

    Position position;

    position.set_FEN(RANDOM_FEN_POSITION);

    //White pawns
    ASSERT_EQ(position.get_piece_size(W_PAWN), 3);
    EXPECT_EQ(position.get_piece_list(W_PAWN)[0], A7);
    EXPECT_EQ(position.get_piece_list(W_PAWN)[1], B7);
    EXPECT_EQ(position.get_piece_list(W_PAWN)[2], H7);
    EXPECT_EQ(position.get_piece_list(W_PAWN)[3], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_PAWN)[4], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_PAWN)[5], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_PAWN)[6], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_PAWN)[7], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_PAWN)[8], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_PAWN)[9], NO_SQUARE);

    //White knights
    ASSERT_EQ(position.get_piece_size(W_KNIGHT), 1);
    EXPECT_EQ(position.get_piece_list(W_KNIGHT)[0], E5);
    EXPECT_EQ(position.get_piece_list(W_KNIGHT)[1], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_KNIGHT)[2], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_KNIGHT)[3], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_KNIGHT)[4], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_KNIGHT)[5], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_KNIGHT)[6], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_KNIGHT)[7], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_KNIGHT)[8], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_KNIGHT)[9], NO_SQUARE);

    //White bishops
    ASSERT_EQ(position.get_piece_size(W_BISHOP), 1);
    EXPECT_EQ(position.get_piece_list(W_BISHOP)[0], D7);
    EXPECT_EQ(position.get_piece_list(W_BISHOP)[1], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_BISHOP)[2], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_BISHOP)[3], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_BISHOP)[4], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_BISHOP)[5], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_BISHOP)[6], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_BISHOP)[7], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_BISHOP)[8], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_BISHOP)[9], NO_SQUARE);

    //White rooks
    ASSERT_EQ(position.get_piece_size(W_ROOK), 1);
    EXPECT_EQ(position.get_piece_list(W_ROOK)[0], A2);
    EXPECT_EQ(position.get_piece_list(W_ROOK)[1], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_ROOK)[2], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_ROOK)[3], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_ROOK)[4], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_ROOK)[5], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_ROOK)[6], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_ROOK)[7], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_ROOK)[8], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_ROOK)[9], NO_SQUARE);

    ////White queen
    ASSERT_EQ(position.get_piece_size(W_QUEEN), 1);
    EXPECT_EQ(position.get_piece_list(W_QUEEN)[0], B5);
    EXPECT_EQ(position.get_piece_list(W_QUEEN)[1], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_QUEEN)[2], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_QUEEN)[3], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_QUEEN)[4], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_QUEEN)[5], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_QUEEN)[6], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_QUEEN)[7], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_QUEEN)[8], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_QUEEN)[9], NO_SQUARE);

    ////White king
    ASSERT_EQ(position.get_piece_size(W_KING), 1);
    EXPECT_EQ(position.get_piece_list(W_KING)[0], H3);
    EXPECT_EQ(position.get_piece_list(W_KING)[1], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_KING)[2], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_KING)[3], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_KING)[4], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_KING)[5], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_KING)[6], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_KING)[7], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_KING)[8], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(W_KING)[9], NO_SQUARE);

    //Black pawns
    ASSERT_EQ(position.get_piece_size(B_PAWN), 5);
    EXPECT_EQ(position.get_piece_list(B_PAWN)[0], C7);
    EXPECT_EQ(position.get_piece_list(B_PAWN)[1], E7);
    EXPECT_EQ(position.get_piece_list(B_PAWN)[2], F7);
    EXPECT_EQ(position.get_piece_list(B_PAWN)[3], G7);
    EXPECT_EQ(position.get_piece_list(B_PAWN)[4], H2);
    EXPECT_EQ(position.get_piece_list(B_PAWN)[5], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_PAWN)[6], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_PAWN)[7], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_PAWN)[8], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_PAWN)[9], NO_SQUARE);

    //Black knights
    ASSERT_EQ(position.get_piece_size(B_KNIGHT), 1);
    EXPECT_EQ(position.get_piece_list(B_KNIGHT)[0], B8);
    EXPECT_EQ(position.get_piece_list(B_KNIGHT)[1], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_KNIGHT)[2], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_KNIGHT)[3], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_KNIGHT)[4], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_KNIGHT)[5], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_KNIGHT)[6], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_KNIGHT)[7], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_KNIGHT)[8], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_KNIGHT)[9], NO_SQUARE);


    //Black bishops
    ASSERT_EQ(position.get_piece_size(B_BISHOP), 1);
    EXPECT_EQ(position.get_piece_list(B_BISHOP)[0], C8);
    EXPECT_EQ(position.get_piece_list(B_BISHOP)[1], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_BISHOP)[2], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_BISHOP)[3], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_BISHOP)[4], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_BISHOP)[5], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_BISHOP)[6], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_BISHOP)[7], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_BISHOP)[8], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_BISHOP)[9], NO_SQUARE);

    //Black rooks
    ASSERT_EQ(position.get_piece_size(B_ROOK), 1);
    EXPECT_EQ(position.get_piece_list(B_ROOK)[0], A8);
    EXPECT_EQ(position.get_piece_list(B_ROOK)[1], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_ROOK)[2], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_ROOK)[3], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_ROOK)[4], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_ROOK)[5], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_ROOK)[6], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_ROOK)[7], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_ROOK)[8], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_ROOK)[9], NO_SQUARE);

    ////Black queen
    ASSERT_EQ(position.get_piece_size(B_QUEEN), 1);
    EXPECT_EQ(position.get_piece_list(B_QUEEN)[0], D8);
    EXPECT_EQ(position.get_piece_list(B_QUEEN)[1], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_QUEEN)[2], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_QUEEN)[3], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_QUEEN)[4], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_QUEEN)[5], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_QUEEN)[6], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_QUEEN)[7], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_QUEEN)[8], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_QUEEN)[9], NO_SQUARE);

    //Black king
    ASSERT_EQ(position.get_piece_size(B_KING), 1);
    EXPECT_EQ(position.get_piece_list(B_KING)[0], E8);
    EXPECT_EQ(position.get_piece_list(B_KING)[1], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_KING)[2], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_KING)[3], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_KING)[4], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_KING)[5], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_KING)[6], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_KING)[7], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_KING)[8], NO_SQUARE);
    EXPECT_EQ(position.get_piece_list(B_KING)[9], NO_SQUARE);

    //TODO UT for mailbox
    EXPECT_EQ(position.get_mailbox_piece(WHITE, A7), PAWN);
    EXPECT_EQ(position.get_mailbox_piece(COLOR_NC, A7), PAWN);
    EXPECT_EQ(position.get_side_to_move(), WHITE);

    EXPECT_EQ(position.get_castling_right(), 0xF);
    
    EXPECT_EQ(position.get_enpassant_square(), C3);
    
    EXPECT_EQ(position.get_fifty_moves_counter(), 34);
        
    EXPECT_EQ(position.get_moves_counter(), 1);
}

TEST(PositionTest, SquareAttacked){

    Position position;

    //Attacks

    const std::string ATTACK_ROOK_POSITION = "8/8/8/8/5r2/8/8/2R2P2 w - - 0 1";

    position.set_FEN(ATTACK_ROOK_POSITION);

    ASSERT_EQ(position.square_is_attacked(Square::F1), true);

    const std::string ATTACK_BISHOP_POSITION = "8/8/8/8/8/3B3b/8/5P2 w - - 0 1";

    position.set_FEN(ATTACK_BISHOP_POSITION);

    ASSERT_EQ(position.square_is_attacked(Square::F1), true);

    const std::string ATTACK_QUEEN_POSITION = "8/8/8/8/5q2/8/8/2R2P2 w - - 0 1";

    position.set_FEN(ATTACK_QUEEN_POSITION);

    ASSERT_EQ(position.square_is_attacked(Square::F1), true);

    const std::string ATTACK_KNIGHT_POSITION = "8/8/8/8/8/3B4/7n/5P2 w - - 0 1";

    position.set_FEN(ATTACK_KNIGHT_POSITION);

    ASSERT_EQ(position.square_is_attacked(Square::F1), true);

    //No attacks

    const std::string NON_ATTACK_ROOK_POSITION = "8/8/8/8/5R2/8/8/2R2P2 w - - 0 1";

    position.set_FEN(NON_ATTACK_ROOK_POSITION);

    ASSERT_EQ(position.square_is_attacked(Square::F1), false);

    const std::string NON_ATTACK_BISHOP_POSITION = "8/8/8/8/8/3B3B/8/5P2 w - - 0 1";

    position.set_FEN(NON_ATTACK_BISHOP_POSITION);

    ASSERT_EQ(position.square_is_attacked(Square::F1), false);

    const std::string NON_ATTACK_QUEEN_POSITION = "8/8/8/8/5Q2/8/8/2R2P2 w - - 0 1";

    position.set_FEN(NON_ATTACK_QUEEN_POSITION);

    ASSERT_EQ(position.square_is_attacked(Square::F1), false);

    const std::string NON_ATTACK_KNIGHT_POSITION = "8/8/8/8/8/3B4/7N/5P2 w - - 0 1";

    position.set_FEN(NON_ATTACK_KNIGHT_POSITION);

    ASSERT_EQ(position.square_is_attacked(Square::F1), false);
}

TEST(PositionTest, MovePiece){

    Position position;

    const std::string FEN_INIT_POSITION = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    position.set_FEN(FEN_INIT_POSITION);

    //1 Check if the square D2 is taken by a pawn and D4 is empty

    //mailbox
    PieceType pieceTypewpbd2 = position.get_mailbox_piece(Color::WHITE, Square::D2);
    PieceType pieceTypencbd2 = position.get_mailbox_piece(Color::COLOR_NC, Square::D2);

    ASSERT_EQ(pieceTypewpbd2, PieceType::PAWN);
    ASSERT_EQ(pieceTypencbd2, PieceType::PAWN);

    PieceType pieceTypewpbd4 = position.get_mailbox_piece(Color::WHITE, Square::D4);
    PieceType pieceTypencbd4 = position.get_mailbox_piece(Color::WHITE, Square::D4);

    ASSERT_EQ(pieceTypewpbd4, PieceType::NO_PIECE_TYPE);
    ASSERT_EQ(pieceTypencbd4, PieceType::NO_PIECE_TYPE);

    //Check piece list size
    std::size_t sizewp = position.get_piece_size(Piece::W_PAWN);
    ASSERT_EQ(sizewp, 8);

    //piece list
    bool findD2Before{false};
    bool findD4Before{false};

    const Square* piecewp = position.get_piece_list(Piece::W_PAWN);

    for(std::size_t i = 0; i < position.get_piece_size(Piece::W_PAWN); ++i){
        if(piecewp[i] == Square::D2)
            findD2Before = true;
        if(piecewp[i] == Square::D4)
            findD4Before = true;
    }

    ASSERT_EQ(findD2Before, true);
    ASSERT_EQ(findD4Before, false);

    //2 move piece from D2 to D4
    position.move_piece(Square::D2, Square::D4);

    //3 Check D2 square

    //mailbox
    PieceType pieceTypewpad2 = position.get_mailbox_piece(Color::WHITE, Square::D2);
    PieceType pieceTypencad2 = position.get_mailbox_piece(Color::COLOR_NC, Square::D2);

    ASSERT_EQ(pieceTypewpad2, PieceType::NO_PIECE_TYPE);
    ASSERT_EQ(pieceTypencad2, PieceType::NO_PIECE_TYPE);

    PieceType pieceTypewpad4 = position.get_mailbox_piece(Color::WHITE, Square::D4);
    PieceType pieceTypencad4 = position.get_mailbox_piece(Color::WHITE, Square::D4);

    ASSERT_EQ(pieceTypewpad4, PieceType::PAWN);
    ASSERT_EQ(pieceTypencad4, PieceType::PAWN);  

    //Check piece list size
    std::size_t sizewpr = position.get_piece_size(Piece::W_PAWN);
    ASSERT_EQ(sizewpr, 8); 

    //piece list
    bool findD2After{false};
    bool findD4After{false};
    const Square* piecenp = position.get_piece_list(Piece::W_PAWN);

    for(std::size_t i = 0; i < position.get_piece_size(Piece::W_PAWN); ++i){
        if(piecewp[i] == Square::D2)
            findD2After = true;
        if(piecewp[i] == Square::D4)
            findD4After = true;
    }

    ASSERT_EQ(findD2After, false);
    ASSERT_EQ(findD4After, true);

}

TEST(PositionTest, RemovePiece){

    Position position;

    const std::string FEN_INIT_POSITION = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    position.set_FEN(FEN_INIT_POSITION);

    //1 Check if the square D2 is taken by a pawn

    //mailbox
    PieceType pieceTypewp = position.get_mailbox_piece(Color::WHITE, Square::D2);
    PieceType pieceTypep = position.get_mailbox_piece(Color::COLOR_NC, Square::D2);

    ASSERT_EQ(pieceTypewp, PieceType::PAWN);
    ASSERT_EQ(pieceTypep, PieceType::PAWN);

    //Check piece list size
    std::size_t sizewp = position.get_piece_size(Piece::W_PAWN);
    ASSERT_EQ(sizewp, 8);

    //piece list
    bool findwp{false};
    const Square* piecewp = position.get_piece_list(Piece::W_PAWN);

    for(std::size_t i = 0; i < position.get_piece_size(Piece::W_PAWN); ++i){
        if(piecewp[i] == Square::D2)
            findwp = true;
    }

    ASSERT_EQ(findwp, true);

    //2 Remove piece located on D2
    position.remove_piece(Square::D2);

    //3 Check D2 square

    //mailbox
    PieceType pieceTypenptw = position.get_mailbox_piece(Color::WHITE, Square::D2);
    PieceType pieceTypenptn = position.get_mailbox_piece(Color::COLOR_NC, Square::D2);

    ASSERT_EQ(pieceTypenptw, PieceType::NO_PIECE_TYPE);   
    ASSERT_EQ(pieceTypenptn, PieceType::NO_PIECE_TYPE);

    //Check piece list size
    std::size_t sizewpr = position.get_piece_size(Piece::W_PAWN);
    ASSERT_EQ(sizewpr, 7); 

    //piece list
    bool findnp{false};
    const Square* piecenp = position.get_piece_list(Piece::W_PAWN);

    for(std::size_t i = 0; i < position.get_piece_size(Piece::W_PAWN); ++i){
        if(piecewp[i] == Square::D2)
            findnp = true;
    }

    ASSERT_EQ(findnp, false);

}
 
TEST(PositionTest, AddPiece){

    Position position;

    const std::string FEN_INIT_POSITION = "rnbqkbnr/pppppppp/8/8/8/8/PPP1PPPP/RNBQKBNR w KQkq - 0 1";

    position.set_FEN(FEN_INIT_POSITION);

    //1 Check if the square D2 is empty

    //mailbox
    PieceType pieceTypewp = position.get_mailbox_piece(Color::WHITE, Square::D2);
    PieceType pieceTypep = position.get_mailbox_piece(Color::COLOR_NC, Square::D2);

    ASSERT_EQ(pieceTypewp, PieceType::NO_PIECE_TYPE);
    ASSERT_EQ(pieceTypep, PieceType::NO_PIECE_TYPE);

    //Check piece list size
    std::size_t sizewp = position.get_piece_size(Piece::W_PAWN);
    ASSERT_EQ(sizewp, 7);

    //piece list
    bool findwp{false};
    const Square* piecewp = position.get_piece_list(Piece::W_PAWN);

    for(std::size_t i = 0; i < position.get_piece_size(Piece::W_PAWN); ++i){
        if(piecewp[i] == Square::D2)
            findwp = true;
    }

    ASSERT_EQ(findwp, false);

    //2 Add piece on D2
    position.add_piece(Square::D2, Piece::W_PAWN);

    //3 Check D2 square

    //mailbox
    PieceType pieceTypenptw = position.get_mailbox_piece(Color::WHITE, Square::D2);
    PieceType pieceTypenptn = position.get_mailbox_piece(Color::COLOR_NC, Square::D2);

    ASSERT_EQ(pieceTypenptw, PieceType::PAWN);   
    ASSERT_EQ(pieceTypenptn, PieceType::PAWN);

    //Check piece list size
    std::size_t sizewpr = position.get_piece_size(Piece::W_PAWN);
    ASSERT_EQ(sizewpr, 8); 

    //piece list
    bool findnp{false};
    const Square* piecenp = position.get_piece_list(Piece::W_PAWN);

    for(std::size_t i = 0; i < position.get_piece_size(Piece::W_PAWN); ++i){
        if(piecewp[i] == Square::D2)
            findnp = true;
    }

    ASSERT_EQ(findnp, true);

}

TEST(PositionTest, DoMove){

    Position position;

    const std::string FEN_INIT_POSITION = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    position.set_FEN(FEN_INIT_POSITION);

    EXPECT_EQ(position.get_side_to_move(), Color::WHITE);
    EXPECT_EQ(position.get_castling_right(), (CastlingRight::WKCA | CastlingRight::WQCA | CastlingRight::BKCA | CastlingRight::BQCA));
    EXPECT_EQ(position.get_enpassant_square(), Square::NO_SQUARE);
    EXPECT_EQ(position.get_fifty_moves_counter(), 0);
    EXPECT_EQ(position.get_moves_counter(), 1);
    EXPECT_EQ(position.get_mailbox_piece(Color::WHITE, Square::B2), PieceType::PAWN);
    EXPECT_EQ(position.get_mailbox_piece(Color::WHITE, Square::B4), PieceType::NO_PIECE_TYPE);

    Move moveb2b4 = make_move(Square::B2, Square::B4, SpecialMove::PAWN_START, Piece::NO_PIECE);    
    position.do_move(moveb2b4);

    EXPECT_EQ(position.get_side_to_move(), Color::BLACK);
    EXPECT_EQ(position.get_castling_right(), (CastlingRight::WKCA | CastlingRight::WQCA | CastlingRight::BKCA | CastlingRight::BQCA));
    EXPECT_EQ(position.get_enpassant_square(), Square::B3);
    EXPECT_EQ(position.get_fifty_moves_counter(), 0);
    EXPECT_EQ(position.get_moves_counter(), 1);
    EXPECT_EQ(position.get_mailbox_piece(Color::WHITE, Square::B2), PieceType::NO_PIECE_TYPE);
    EXPECT_EQ(position.get_mailbox_piece(Color::WHITE, Square::B4), PieceType::PAWN);

}

TEST(PositionTest, UndoMove){

     Position position;

    const std::string FEN_INIT_POSITION = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    position.set_FEN(FEN_INIT_POSITION);

    EXPECT_EQ(position.get_side_to_move(), Color::WHITE);
    EXPECT_EQ(position.get_castling_right(), (CastlingRight::WKCA | CastlingRight::WQCA | CastlingRight::BKCA | CastlingRight::BQCA));
    EXPECT_EQ(position.get_enpassant_square(), Square::NO_SQUARE);
    EXPECT_EQ(position.get_fifty_moves_counter(), 0);
    EXPECT_EQ(position.get_moves_counter(), 1);
    EXPECT_EQ(position.get_mailbox_piece(Color::WHITE, Square::B2), PieceType::PAWN);
    EXPECT_EQ(position.get_mailbox_piece(Color::WHITE, Square::B4), PieceType::NO_PIECE_TYPE);

    Move moveb2b4 = make_move(Square::B2, Square::B4, SpecialMove::PAWN_START, Piece::NO_PIECE); 

    //TODO do this using mocks on undo move   
    position.do_move(moveb2b4);
    position.undo_move();

    EXPECT_EQ(position.get_side_to_move(), Color::WHITE);
    EXPECT_EQ(position.get_castling_right(), (CastlingRight::WKCA | CastlingRight::WQCA | CastlingRight::BKCA | CastlingRight::BQCA));
    EXPECT_EQ(position.get_enpassant_square(), Square::NO_SQUARE);
    EXPECT_EQ(position.get_fifty_moves_counter(), 0);
    EXPECT_EQ(position.get_moves_counter(), 1);
    EXPECT_EQ(position.get_mailbox_piece(Color::WHITE, Square::B2), PieceType::PAWN);
    EXPECT_EQ(position.get_mailbox_piece(Color::WHITE, Square::B4), PieceType::NO_PIECE_TYPE);


}
