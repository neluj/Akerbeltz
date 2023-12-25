#include <gtest/gtest.h>
#include "move.h"

/*Move:
0000 0000 0000 0000 0000 0000 0000 0111 1111 -> From 0x7F
0000 0000 0000 0000 0000 0011 1111 1000 0000 -> To 0x7F
0000 0000 0000 0000 0000 1100 0000 0000 0000 -> Special move flag
0000 0000 0000 0000 0111 0000 0000 0000 0000 -> Promotion Piece
0000 0000 0000 0011 1000 0000 0000 0000 0000 -> Captured piece type
1111 1111 1111 1100 0000 0000 0000 0000 0000 -> Score
*/

TEST(MoveTest, PromotionMove){
                                                                
    Xake::Square from = Xake::Square::A6;                       //1000111
    Xake::Square to = Xake::Square::A7;                         //1010001
    Xake::SpecialMove specialMove = Xake::SpecialMove::PROMOTION_QUEEN;  //1 0100
    Xake::Piece capturedPiece = Xake::Piece::B_BISHOP;          //1001

    Xake::Move moveQueenPromotion = Xake::make_move(from, to, specialMove, capturedPiece);
    Xake::Move expected = 0b10011010010100011000111;

    EXPECT_EQ(moveQueenPromotion, expected);

}

TEST(MoveTest, CastlingMove){

    Xake::Square from = Xake::Square::A6;                           //1000111
    Xake::Square to = Xake::Square::A7;                             //1010001
    Xake::SpecialMove specialMove = Xake::SpecialMove::CASTLE;      //0 0011
    Xake::Piece capturedPiece = Xake::Piece::B_BISHOP;              //1001
    
    Xake::Move moveCasling = Xake::make_move(from, to, specialMove, capturedPiece);
    Xake::Move expected = 0b10010001110100011000111;

    EXPECT_EQ(moveCasling, expected);

}