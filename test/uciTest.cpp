#include <gtest/gtest.h>
#include "uci.h"

//TODO usar mock
#include "position.h"

using namespace Xake;

TEST(UCITest, MakeMove){

    const std::string START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    Position pos;
    pos.set_FEN(START_FEN);

    std::string happyMove{"d2d4"};
    Move expectedHappyMove = make_quiet_move(SQ64_D2, SQ64_D4, SpecialMove::PAWN_START);
    EXPECT_EQ(UCI::make_move(pos, happyMove), expectedHappyMove);

    std::string wrongSizeL{"aaaaaa"};
    EXPECT_EQ(UCI::make_move(pos, wrongSizeL), 0);

    std::string wrongSizeS{"aaa"};
    EXPECT_EQ(UCI::make_move(pos, wrongSizeS), 0);

    std::string wrongValue{"aaaa"};
    EXPECT_EQ(UCI::make_move(pos, wrongValue), 0);

    const std::string START_B_FEN = "rnbqkbnr/pppppppp/8/8/3P4/8/PPP1PPPP/RNBQKBNR b KQkq d3 0 1";
    pos.set_FEN(START_B_FEN);

    std::string bpStart{"d7d5"};
    Move expectedbpStart = make_quiet_move(SQ64_D7, SQ64_D5, SpecialMove::PAWN_START);
    EXPECT_EQ(UCI::make_move(pos, bpStart), expectedbpStart);

    const std::string CASTLING_W = "4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1";
    pos.set_FEN(CASTLING_W);
    
    std::string wCastlq{"e1c1"};
    Move expectedwCastlq = make_quiet_move(SQ64_E1, SQ64_C1, SpecialMove::CASTLE);
    EXPECT_EQ(UCI::make_move(pos, wCastlq), expectedwCastlq);


    const std::string PROMOTION = "8/Pk6/8/8/8/8/6Kp/8 w - - 0 1";
    pos.set_FEN(PROMOTION);
    
    std::string wPromq{"a7a8q"};
    Move expectedwPromq = make_quiet_move(SQ64_A7, SQ64_A8, SpecialMove::PROMOTION_QUEEN);
    EXPECT_EQ(UCI::make_move(pos, wPromq), expectedwPromq);

    const std::string CAPTURE = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    pos.set_FEN(CAPTURE);
    
    std::string capMove{"d5e6"};
    Move expectedcapMove = make_capture_move(SQ64_D5, SQ64_E6, SpecialMove::NO_SPECIAL, W_PAWN, B_PAWN);
    EXPECT_EQ(UCI::make_move(pos, capMove), expectedcapMove);
} 

