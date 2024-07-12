#include <gtest/gtest.h>
#include "types.h"

using namespace Xake;

TEST(TypesTest, Squarest){
    
    File filea1 = square_file(SQ120_A1);
    Rank ranka1 = square_rank(SQ120_A1);

    EXPECT_EQ(FILE_A, filea1);
    EXPECT_EQ(RANK_1, ranka1);

    File fileh1 = square_file(SQ120_H1);
    Rank rankh1 = square_rank(SQ120_H1);

    EXPECT_EQ(FILE_H, fileh1);
    EXPECT_EQ(RANK_1, rankh1);

    File filea8 = square_file(SQ120_A8);
    Rank ranka8 = square_rank(SQ120_A8);

    EXPECT_EQ(FILE_A, filea8);
    EXPECT_EQ(RANK_8, ranka8);

    File fileh8 = square_file(SQ120_H8);
    Rank rankh8 = square_rank(SQ120_H8);

    EXPECT_EQ(FILE_H, fileh8);
    EXPECT_EQ(RANK_8, rankh8);

    File filed1 = square_file(SQ120_D1);
    Rank rankd1 = square_rank(SQ120_D1);

    EXPECT_EQ(FILE_D, filed1);
    EXPECT_EQ(RANK_1, rankd1);

    File filee4 = square_file(SQ120_E4);
    Rank ranke4 = square_rank(SQ120_E4);

    EXPECT_EQ(FILE_E, filee4);
    EXPECT_EQ(RANK_4, ranke4);

    File filef8 = square_file(SQ120_F8);
    Rank rankf8 = square_rank(SQ120_F8);

    EXPECT_EQ(FILE_F, filef8);
    EXPECT_EQ(RANK_8, rankf8);

    File fileg1 = square_file(SQ120_G1);
    Rank rankg1 = square_rank(SQ120_G1);

    EXPECT_EQ(FILE_G, fileg1);
    EXPECT_EQ(RANK_1, rankg1);

    File fileb8 = square_file(SQ120_B8);
    Rank rankb8 = square_rank(SQ120_B8);

    EXPECT_EQ(FILE_B, fileb8);
    EXPECT_EQ(RANK_8, rankb8);

    File fileb1 = square_file(SQ120_B1);
    Rank rankb1 = square_rank(SQ120_B1);

    EXPECT_EQ(FILE_B, fileb1);
    EXPECT_EQ(RANK_1, rankb1);

    File fileg2 = square_file(SQ120_G2);
    Rank rankg2 = square_rank(SQ120_G2);

    EXPECT_EQ(FILE_G, fileg2);
    EXPECT_EQ(RANK_2, rankg2);

    File fileb2 = square_file(SQ120_B2);
    Rank rankb2 = square_rank(SQ120_B2);

    EXPECT_EQ(FILE_B, fileb2);
    EXPECT_EQ(RANK_2, rankb2);

    File fileg7 = square_file(SQ120_G7);
    Rank rankg7 = square_rank(SQ120_G7);

    EXPECT_EQ(FILE_G, fileg7);
    EXPECT_EQ(RANK_7, rankg7);
}