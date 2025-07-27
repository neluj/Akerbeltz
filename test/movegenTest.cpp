#include <gtest/gtest.h>
#include "movegen.h"
#include "position.h"

using namespace Xake;
//Pseudo-legal basic unit test, using random position 

TEST(MovegeneratorTest, WithePawnCapture){
    
    const std::string WHITE_PAWN_CAPTURE_FEN_POSITION = "3krrb1/5P2/3p1p2/3PP1Pp/Pp1p4/2p5/3P4/3K4 w - h6 1 1";
    Position position;
    position.set_FEN(WHITE_PAWN_CAPTURE_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_capture_moves(position,moveList);
    EXPECT_EQ(moveList.size, 13);

}

TEST(MovegeneratorTest, WhitePawns){

    const std::string WITHE_PAWN_FEN_POSITION = "3k1p2/6P1/8/pPp1pp2/3P1P2/1pP2P1p/P6P/4K3 w - a6 0 1";
    Position position;
    position.set_FEN(WITHE_PAWN_FEN_POSITION);

    MoveGen::MoveList moveList;

    MoveGen::generate_all_moves(position,moveList);
    EXPECT_EQ(moveList.size, 23);

}

TEST(MovegeneratorTest, BlackPawnCapture){
    
    const std::string BLACK_PAWN_CAPTURE_FEN_POSITION = "3k4/3p4/2P5/pP1P4/3pp1pP/3P1P2/5p2/3KRRB1 b - h3 1 1";
    Position position;
    position.set_FEN(BLACK_PAWN_CAPTURE_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_capture_moves(position,moveList);
    EXPECT_EQ(moveList.size, 13);

}


TEST(MovegeneratorTest, BlackPawns){
    const std::string BLACK_PAWN_FEN_POSITION = "4k3/p6p/1Pp2p1P/3p1p2/PpP1PP2/8/6p1/3K1P2 b - a3 0 1";
    Position position;
    position.set_FEN(BLACK_PAWN_FEN_POSITION);

    MoveGen::MoveList moveList;

    MoveGen::generate_all_moves(position,moveList);
    EXPECT_EQ(moveList.size, 23);
}


TEST(MovegeneratorTest, WhiteKnightCapture){
    
    const std::string WHITE_KNIGHT_CAPTURE_FEN_POSITION = "3k4/8/3P4/4p3/2N5/4p3/8/4K3 w - - 0 1";
    Position position;
    position.set_FEN(WHITE_KNIGHT_CAPTURE_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_capture_moves(position,moveList);
    EXPECT_EQ(moveList.size, 2);
    
}

TEST(MovegeneratorTest, WhiteKnight){
    
    const std::string WHITE_KNIGHT_FEN_POSITION = "4kp2/8/3P2N1/2P5/2N5/2p2P2/1p6/5K2 w - - 0 1";
    Position position;
    position.set_FEN(WHITE_KNIGHT_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_all_moves(position,moveList);
    EXPECT_EQ(moveList.size, 21);

}

TEST(MovegeneratorTest, BlackKnightCapture){
    
    const std::string BLACK_KNIGHT_CAPTURE_FEN_POSITION = "3k4/8/1p6/4P3/2n5/4P3/8/4K3 b - - 0 1";
    Position position;
    position.set_FEN(BLACK_KNIGHT_CAPTURE_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_capture_moves(position,moveList);
    EXPECT_EQ(moveList.size, 2);
    
}

TEST(MovegeneratorTest, BlackKnight){
    
    const std::string BLACK_KNIGHT_FEN_POSITION = "5k2/1P6/2P2p2/2n5/2p5/3p2n1/8/4KP2 b - - 0 1";
    Position position;
    position.set_FEN(BLACK_KNIGHT_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_all_moves(position,moveList);
    EXPECT_EQ(moveList.size, 21);

}

TEST(MovegeneratorTest, WhiteKingCapture){
    
    const std::string WHITE_KING_CAPTURE_FEN_POSITION = "3k4/8/8/8/8/6pP/5p1K/8 w - - 0 1";
    Position position;
    position.set_FEN(WHITE_KING_CAPTURE_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_capture_moves(position,moveList);
    EXPECT_EQ(moveList.size, 1);

}

TEST(MovegeneratorTest, BlackKingCapture){
    
    const std::string BLACK_KING_CAPTURE_FEN_POSITION = "8/5P1k/6Pp/8/8/8/8/3K4 b - - 0 1";
    Position position;
    position.set_FEN(BLACK_KING_CAPTURE_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_capture_moves(position,moveList);
    EXPECT_EQ(moveList.size, 1);

}

TEST(MovegeneratorTest, WhiteKingCastling){
    
    const std::string WHITE_CASTLING_FEN_POSITION = "4k3/8/8/8/8/8/4P2P/3PK2R w K - 0 1";
    Position position;
    position.set_FEN(WHITE_CASTLING_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_all_moves(position,moveList);
    EXPECT_EQ(moveList.size, 11);

}

TEST(MovegeneratorTest, BlackKingCastling){
    
    const std::string BLACK_CASTLING_FEN_POSITION = "4k2r/4p2p/8/8/8/8/8/4K3 b k - 0 1";
    Position position;
    position.set_FEN(BLACK_CASTLING_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_all_moves(position,moveList);
    EXPECT_EQ(moveList.size, 11);

}

TEST(MovegeneratorTest, WhiteQueenCastling){
    
    const std::string WHITE_CASTLING_FEN_POSITION = "4k3/8/8/8/8/8/P3P3/R3K3 w Q - 1 1";
    Position position;
    position.set_FEN(WHITE_CASTLING_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_all_moves(position,moveList);
    EXPECT_EQ(moveList.size, 12);

}

TEST(MovegeneratorTest, BlackQueenCastling){
    
    const std::string BLACK_CASTLING_FEN_POSITION = "r3k3/p3p3/8/8/8/8/8/4K3 b q - 1 1";
    Position position;
    position.set_FEN(BLACK_CASTLING_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_all_moves(position,moveList);
    EXPECT_EQ(moveList.size, 12);

} 

TEST(MovegeneratorTest, WhiteBishopCapture){
    const std::string WHITE_BISHOP_CAPTURE_FEN_POSITION = "3k4/8/1p6/1p1PP3/2B5/4P3/4p3/7K w - - 0 1";
    Position position;
    position.set_FEN(WHITE_BISHOP_CAPTURE_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_capture_moves(position,moveList);
    EXPECT_EQ(moveList.size, 2);

}

TEST(MovegeneratorTest, WhiteBishop){
    const std::string WHITE_BISHOP_FEN_POSITION = "4k3/7B/8/2P1pp2/3B4/8/8/P3K1p1 w - - 0 1";
    Position position;
    position.set_FEN(WHITE_BISHOP_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_all_moves(position,moveList);
    EXPECT_EQ(moveList.size, 16);

}

TEST(MovegeneratorTest, BlackBishopCapture){
    const std::string BLACK_BISHOP_CAPTURE_FEN_POSITION = "7k/4P3/4p3/2b5/1P1pp3/1P6/8/3K4 b - - 0 1";
    Position position;
    position.set_FEN(BLACK_BISHOP_CAPTURE_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_capture_moves(position,moveList);
    EXPECT_EQ(moveList.size, 2);
}

TEST(MovegeneratorTest, BlackBishop){
    const std::string BLACK_BISHOP_FEN_POSITION = "p3k1P1/8/8/3b4/2p1PP2/8/7b/4K3 b - - 0 1";
    Position position;
    position.set_FEN(BLACK_BISHOP_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_all_moves(position,moveList);
    EXPECT_EQ(moveList.size, 16);
}

TEST(MovegeneratorTest, WhiteRookCapture){
    
    const std::string WHITE_ROOK_CAPTURE_FEN_POSITION = "7k/2p5/8/2R2P2/8/8/8/3K4 w - - 0 1";
    Position position;
    position.set_FEN(WHITE_ROOK_CAPTURE_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_capture_moves(position,moveList);
    EXPECT_EQ(moveList.size, 1);

}

TEST(MovegeneratorTest, WhiteRook){
    
    const std::string WHITE_ROOK_FEN_POSITION = "4kp2/8/2p2R2/2P5/2R5/2p5/8/5K2 w - - 0 1";
    Position position;
    position.set_FEN(WHITE_ROOK_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_all_moves(position,moveList);
    EXPECT_EQ(moveList.size, 24);

}

TEST(MovegeneratorTest, BlackRookCapture){
    
    const std::string BLACK_ROOK_CAPTURE_FEN_POSITION = "3k4/8/8/8/2r2p2/8/2P5/7K b - - 0 1";
    Position position;
    position.set_FEN(BLACK_ROOK_CAPTURE_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_capture_moves(position,moveList);
    EXPECT_EQ(moveList.size, 1);

}

TEST(MovegeneratorTest, BlackRook){
    
    const std::string BLACK_ROOK_FEN_POSITION = "5k2/8/2P5/2r5/2p5/2P2r2/8/4KP2 b - - 0 1";
    Position position;
    position.set_FEN(BLACK_ROOK_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_all_moves(position,moveList);
    EXPECT_EQ(moveList.size, 24);

} 

TEST(MovegeneratorTest, WhiteQueenCapture){
    
    const std::string WHITE_QUEEN_CAPTURE_FEN_POSITION = "3k4/8/8/3p4/2Q2p2/1P6/2P5/7K w - - 0 1";
    Position position;
    position.set_FEN(WHITE_QUEEN_CAPTURE_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_capture_moves(position,moveList);
    EXPECT_EQ(moveList.size, 2);

}

TEST(MovegeneratorTest, WhiteQueen){
    
    const std::string WHITE_QUEEN_FEN_POSITION = "4kp2/8/5Q2/2P5/2Q5/2p2P2/8/5K2 w - - 0 1";
    Position position;
    position.set_FEN(WHITE_QUEEN_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_all_moves(position,moveList);
    EXPECT_EQ(moveList.size, 45);

}

TEST(MovegeneratorTest, BlackQueenCapture){
    
    const std::string BLACK_QUEEN_CAPTURE_FEN_POSITION = "7k/2p5/1p6/2q2P2/3P4/8/8/3K4 b - - 0 1";
    Position position;
    position.set_FEN(BLACK_QUEEN_CAPTURE_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_capture_moves(position,moveList);
    EXPECT_EQ(moveList.size, 2);

}

TEST(MovegeneratorTest, BlackQueen){
    
    const std::string BLACK_QUEEN_FEN_POSITION = "5k2/8/2P2p2/2q5/2p5/5q2/8/4KP2 b - - 0 1";
    Position position;
    position.set_FEN(BLACK_QUEEN_FEN_POSITION);
    MoveGen::MoveList moveList;

    MoveGen::generate_all_moves(position,moveList);
    EXPECT_EQ(moveList.size, 45);

}