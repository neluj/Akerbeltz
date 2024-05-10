#include <gtest/gtest.h>
#include "movegen.h"
#include "position.h"

using namespace Xake;
//Pseudo-legal basic unit test, using random position 

TEST(MovegeneratorTest, WhitePawns){

    const std::string WITHE_PAWN_FEN_POSITION = "3k1p2/6P1/8/pPp1pp2/3P1P2/1pP2P1p/P6P/4K3 w - a6 0 1";
    Position position;
    position.set_FEN(WITHE_PAWN_FEN_POSITION);

    MoveGen::MoveList moveList;

    MoveGen::generate_all_moves(position,moveList);
    EXPECT_EQ(moveList.size, 23);

}

TEST(MovegeneratorTest, BlackPawns){
    const std::string BLACK_PAWN_FEN_POSITION = "4k3/p6p/1Pp2p1P/3p1p2/PpP1PP2/8/6p1/3K1P2 b - a3 0 1";
    Position position;
    position.set_FEN(BLACK_PAWN_FEN_POSITION);

    MoveGen::MoveList moveList;

    MoveGen::generate_all_moves(position,moveList);
    EXPECT_EQ(moveList.size, 23);
}


TEST(MovegeneratorTest, WhiteKnight){
    
    const std::string WHITE_KNIGHT_FEN_POSITION = "4kp2/8/3P2N1/2P5/2N5/2p2P2/1p6/5K2 w - - 0 1";
    Position position;
    position.set_FEN(WHITE_KNIGHT_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_all_moves(position,moveList);
    EXPECT_EQ(moveList.size, 21);

}
 

TEST(MovegeneratorTest, BlackKnight){
    
    const std::string BLACK_KNIGHT_FEN_POSITION = "5k2/1P6/2P2p2/2n5/2p5/3p2n1/8/4KP2 b - - 0 1";
    Position position;
    position.set_FEN(BLACK_KNIGHT_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_all_moves(position,moveList);
    EXPECT_EQ(moveList.size, 21);

}

TEST(MovegeneratorTest, WhiteBishop){
    const std::string WHITE_BISHOP_FEN_POSITION = "4k3/7B/8/2P1pp2/3B4/8/8/P3K1p1 w- - 0 1";
    Position position;
    position.set_FEN(WHITE_BISHOP_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_all_moves(position,moveList);
    EXPECT_EQ(moveList.size, 16);

}

TEST(MovegeneratorTest, BlackBishop){
    const std::string BLACK_BISHOP_FEN_POSITION = "p3k1P1/8/8/3b4/2p1PP2/8/7b/4K3 b - - 0 1";
    Position position;
    position.set_FEN(BLACK_BISHOP_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_all_moves(position,moveList);
    EXPECT_EQ(moveList.size, 16);
} 

TEST(MovegeneratorTest, WhiteRook){
    
    const std::string WHITE_ROOK_FEN_POSITION = "4kp2/8/2p2R2/2P5/2R5/2p5/8/5K2 w - - 0 1";
    Position position;
    position.set_FEN(WHITE_ROOK_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_all_moves(position,moveList);
    EXPECT_EQ(moveList.size, 24);

}
 

TEST(MovegeneratorTest, BlackRook){
    
    const std::string BLACK_ROOK_FEN_POSITION = "5k2/8/2P5/2r5/2p5/2P2r2/8/4KP2 b - - 0 1";
    Position position;
    position.set_FEN(BLACK_ROOK_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_all_moves(position,moveList);
    EXPECT_EQ(moveList.size, 24);

} 

TEST(MovegeneratorTest, WhiteQueen){
    
    const std::string WHITE_QUEEN_FEN_POSITION = "4kp2/8/5Q2/2P5/2Q5/2p2P2/8/5K2 w - - 0 1";
    Position position;
    position.set_FEN(WHITE_QUEEN_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_all_moves(position,moveList);
    EXPECT_EQ(moveList.size, 45);

}
 

TEST(MovegeneratorTest, BlackQueen){
    
    const std::string BLACK_QUEEN_FEN_POSITION = "5k2/8/2P2p2/2q5/2p5/5q2/8/4KP2 b - - 0 1";
    Position position;
    position.set_FEN(BLACK_QUEEN_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_all_moves(position,moveList);
    EXPECT_EQ(moveList.size, 45);

}

TEST(MovegeneratorTest, WhiteCastling){
    
    const std::string WHITE_CASTLING_FEN_POSITION = "4k3/8/8/8/8/8/4P2P/3PK2R w K - 0 1";
    Position position;
    position.set_FEN(WHITE_CASTLING_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_all_moves(position,moveList);
    EXPECT_EQ(moveList.size, 11);

}

TEST(MovegeneratorTest, BlackCastling){
    
    const std::string WHITE_CASTLING_FEN_POSITION = "3pk2r/4p2p/8/8/8/8/8/4K3 b k - 0 1";
    Position position;
    position.set_FEN(WHITE_CASTLING_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_all_moves(position,moveList);
    EXPECT_EQ(moveList.size, 11);

}