#include <gtest/gtest.h>
#include "move.h"

using namespace Xake;

/*Move:
0000 0000 0000 0000 0000 0000 0000 0111 1111 -> From 0x7F
0000 0000 0000 0000 0000 0011 1111 1000 0000 -> To 0x7F
0000 0000 0000 0000 0000 1100 0000 0000 0000 -> Special move flag
0000 0000 0000 0000 0111 0000 0000 0000 0000 -> Promotion Piece
0000 0000 0000 0011 1000 0000 0000 0000 0000 -> Captured piece type
1111 1111 1111 1100 0000 0000 0000 0000 0000 -> Score
*/

TEST(MoveTest, PromotionMove){
                                                                
    Square120 from = Square120::SQ120_A6;                       //1000111
    Square120 to = Square120::SQ120_A7;                         //1010001
    SpecialMove specialMove = SpecialMove::PROMOTION_QUEEN;  //1 0100
    Piece capturedPiece = Piece::B_BISHOP;          //1001

    Move moveQueenPromotion = make_move(from, to, specialMove, capturedPiece);
    Move expected = 0b10011010010100011000111;

    EXPECT_EQ(moveQueenPromotion, expected);

}

TEST(MoveTest, CastlingMove){

    Square120 from = Square120::SQ120_A6;                           //1000111
    Square120 to = Square120::SQ120_A7;                             //1010001
    SpecialMove specialMove = SpecialMove::CASTLE;      //0 0011
    Piece capturedPiece = Piece::B_BISHOP;              //1001
    
    Move moveCasling = make_move(from, to, specialMove, capturedPiece);
    Move expected = 0b10010001110100011000111;

    EXPECT_EQ(moveCasling, expected);

}