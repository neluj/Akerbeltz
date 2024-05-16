#include <gtest/gtest.h>
#include "uci.h"

//TODO usar mock
#include "position.h"

using namespace Xake;

/* TEST(UCITest, MakeMove){

    const std::string START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    Position pos;
    pos.set_FEN(START_FEN);

    std::string happyMove{"d2d4"};
    Move expectedHappyMove = 0b1001101100100010;
    EXPECT_EQ(UCI::make_move(pos, happyMove), expectedHappyMove);

    std::string wrongSizeL{"aaaaaa"};
    EXPECT_EQ(UCI::make_move(pos, wrongSizeL), 0);

    std::string wrongSizeS{"aaa"};
    EXPECT_EQ(UCI::make_move(pos, wrongSizeS), 0);

    std::string wrongValue{"aaaa"};
    EXPECT_EQ(UCI::make_move(pos, wrongValue), 0);

    //std::string happyMove{"d3d4"};
    //EXPECT_EQ(UCI::make_move(pos, happyMove), 0b01101100100010);

} */

// DELETEME
TEST(UCITest, DISABLED_UCIDeleteme){
//TEST(UCITest, UCIDeleteme){

    UCI::run();
  
}
