#include <gtest/gtest.h>
#include "move.h"

using namespace Xake;

/*Move:
0000 0000 0000 0000 0000 0000 0000 0111 1111 -> From 0x7F
0000 0000 0000 0000 0000 0011 1111 1000 0000 -> To 0x7F
0000 0000 0000 0000 0000 1100 0000 0000 0000 -> Special move flag
0000 0000 0000 0000 0111 0000 0000 0000 0000 -> Promotion Piece
0000 0000 0000 0111 1000 0000 0000 0000 0000 -> Captured piece
1111 1111 1111 1000 0000 0000 0000 0000 0000 -> Score
*/

TEST(MoveTest, PromotionMove){
                                                                
    Square120 from = Square120::SQ120_A6;                       
    Square120 to = Square120::SQ120_A7;                         
    SpecialMove specialMove = SpecialMove::PROMOTION_QUEEN;  
    Piece attackerPiece = Piece::W_PAWN;
    Piece capturedPiece = Piece::B_BISHOP; 

    Move moveQueenPromotion = make_capture_move(from, to, specialMove, attackerPiece, capturedPiece);
    Move expected = 0b1000011110111010010100011000111;

    EXPECT_EQ(moveQueenPromotion, expected);

}

TEST(MoveTest, CastlingMove){

    Square120 from = Square120::SQ120_A6;                           
    Square120 to = Square120::SQ120_A7;                             
    SpecialMove specialMove = SpecialMove::CASTLE;  
    Piece attackerPiece = Piece::W_PAWN;                 
    Piece capturedPiece = Piece::B_BISHOP;
    
    Move moveCasling = make_capture_move(from, to, specialMove, attackerPiece, capturedPiece);
    Move expected = 0b1000011110110001110100011000111;

    EXPECT_EQ(moveCasling, expected);

}

TEST(MoveTest, MoveScoreFunction){

    Move move = 0b111111111111100000000000000000000000;
    MoveScore expectedScore = 8191;
    MoveScore score = move_score(move);

    EXPECT_EQ(score, expectedScore);

}
