#include <gtest/gtest.h>
#include "pvtable.h"

#include "position.h"

using namespace Xake;


class PVTableTest : public ::testing::Test
{
    protected:
    virtual void SetUp()
    {  
       PVTable::init();
       Position::init();    
    }

    virtual void TearDown()
    {
    }
    public:
    Position position;
};

TEST_F(PVTableTest, LoadPVLine){

    //Inser new PV line

    Position position;
    const std::string FEN_INIT_POS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    position.set_FEN(FEN_INIT_POS);

    PVTable::PVLine pvLine;
                                                                
    PVTable::load_pv_line(pvLine, MAX_DEPTH, position);
                                    
    EXPECT_EQ(0, pvLine.depth);

    const std::string WAC1_POSITION = "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - - 0 1";
    position.set_FEN(WAC1_POSITION);

    Square120 fromg3 = Square120::SQ120_G3;
    Square120 tog6 = Square120::SQ120_G6;
    SpecialMove spNo = SpecialMove::NO_SPECIAL;
    Piece capNo = Piece::NO_PIECE;

    Move mg3g6 = make_move(fromg3, tog6, spNo, capNo);
    PVTable::insert_entry(position, mg3g6);

    PVTable::load_pv_line(pvLine, MAX_DEPTH, position);

    EXPECT_EQ(1, pvLine.depth);
    EXPECT_EQ(mg3g6, pvLine.moves[0]);

    position.do_move(mg3g6);

    Square120 fromd6 = Square120::SQ120_D6;
    Square120 toe5 = Square120::SQ120_E5;
    Piece capwk = Piece::W_KNIGHT;

    Move md6e5 = make_move(fromd6, toe5, spNo, capwk);
    PVTable::insert_entry(position, md6e5);

    PVTable::load_pv_line(pvLine, MAX_DEPTH, position);

    EXPECT_EQ(1, pvLine.depth);
    EXPECT_EQ(md6e5, pvLine.moves[0]);

    position.undo_move();

    PVTable::load_pv_line(pvLine, MAX_DEPTH, position);

    EXPECT_EQ(2, pvLine.depth);
    EXPECT_EQ(md6e5, pvLine.moves[1]);

    //Inser new PV line

    position.set_FEN(FEN_INIT_POS);

    PVTable::load_pv_line(pvLine, MAX_DEPTH, position);
                                    
    EXPECT_EQ(0, pvLine.depth);

    Square120 fromb1 = Square120::SQ120_B1;
    Square120 toc3 = Square120::SQ120_C3;

    Move mb1c3 = make_move(fromb1, toc3, spNo, capNo);
    PVTable::insert_entry(position, mb1c3);

    PVTable::load_pv_line(pvLine, MAX_DEPTH, position);

    EXPECT_EQ(1, pvLine.depth);
    EXPECT_EQ(mb1c3, pvLine.moves[0]);

    //Check previous line

    position.set_FEN(WAC1_POSITION);
    PVTable::load_pv_line(pvLine, MAX_DEPTH, position);

    EXPECT_EQ(2, pvLine.depth);
    EXPECT_EQ(mg3g6, pvLine.moves[0]);
    EXPECT_EQ(md6e5, pvLine.moves[1]);


}