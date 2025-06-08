#include <gtest/gtest.h>
#include "move.h"

using namespace Xake;

/*Move:
0000 0000 0000 0000 0000 0000 0000 0011 1111 -> From 0x7F
0000 0000 0000 0000 0000 0000 1111 1100 0000 -> To 0x7F
0000 0000 0000 0000 0000 0011 0000 0000 0000 -> Special move flag
0000 0000 0000 0000 0001 1100 0000 0000 0000 -> Promotion Piece
0000 0000 0000 0001 1110 0000 0000 0000 0000 -> Captured piece
1111 1111 1111 1110 0000 0000 0000 0000 0000 -> Score
*/

TEST(MoveTest, PromotionMove){
                                                                
    Square64 from = Square64::SQ64_A6;                       
    Square64 to = Square64::SQ64_A7;                         
    SpecialMove specialMove = SpecialMove::PROMOTION_QUEEN;  
    Piece attackerPiece = Piece::W_PAWN;
    Piece capturedPiece = Piece::B_BISHOP; 

    Move moveQueenPromotion = make_capture_move(from, to, specialMove, attackerPiece, capturedPiece);
    Move expected = 0b1110100111101110100110000101000;

    EXPECT_EQ(moveQueenPromotion, expected);

}

TEST(MoveTest, CastlingMove){

    Square64 from = Square64::SQ64_A6;                           
    Square64 to = Square64::SQ64_A7;                             
    SpecialMove specialMove = SpecialMove::CASTLE;  
    Piece attackerPiece = Piece::W_PAWN;                 
    Piece capturedPiece = Piece::B_BISHOP;
    
    Move moveCasling = make_capture_move(from, to, specialMove, attackerPiece, capturedPiece);
    Move expected = 0b1110100111101100011110000101000;

    EXPECT_EQ(moveCasling, expected);

}

TEST(MoveTest, MoveScoreFunction){

    Move move = 0b1111111111111000000000000000000000;
    MoveScore expectedScore = 8191;
    MoveScore score = move_score(move);

    EXPECT_EQ(score, expectedScore);

}
