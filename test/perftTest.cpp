#include <gtest/gtest.h>
#include "search.h"
#include "position.h"

using namespace Xake;
//Pseudo-legal basic unit test, using random position 

TEST(PerftTest, CaseOne){

    Position position;



    //const std::string FEN_INIT_POSITION = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    //const std::string FEN_INIT_POSITION = "rnbqkbnr/pppppppp/8/8/1P6/8/PQ1PP2P/R3K2R w KQkq - 0 1";
    //const std::string FEN_INIT_POSITION = "r3k2r/p1ppqpb1/bn2pnN1/3P4/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    //const std::string FEN_INIT_POSITION = "r3k2r/2P5/8/8/1p6/8/P1P5/R3K2R w KQkq - 0 1";
    //const std::string FEN_INIT_POSITION = "4k3/8/8/8/1p6/P7/8/4K3 w - - 0 1";
    //const std::string FEN_INIT_POSITION = "4k3/8/8/8/1p6/8/P7/4K3 w - - 0 1";
    // perft 2 const std::string FEN_INIT_POSITION = "4k3/P7/8/8/8/8/4K3/2q5 b - - 0 1";
    //const std::string FEN_INIT_POSITION = "4k3/8/8/8/8/8/6K1/6r1 w - - 0 1";
    const std::string FEN_INIT_POSITION = "8/1n4N1/2k5/8/8/5K2/1N4n1/8 b - - 0 1";
    
    position.set_FEN(FEN_INIT_POSITION);

    Search::perftTest(6, position);

}