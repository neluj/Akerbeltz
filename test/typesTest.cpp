#include <gtest/gtest.h>
#include "types.h"

TEST(TypesTest, Squarest){
    
    Xake::File filea1 = square_file(Xake::SQ120_A1);
    Xake::Rank ranka1 = square_rank(Xake::SQ120_A1);

    EXPECT_EQ(Xake::FILE_A, filea1);
    EXPECT_EQ(Xake::RANK_1, ranka1);

    Xake::File fileh1 = square_file(Xake::SQ120_H1);
    Xake::Rank rankh1 = square_rank(Xake::SQ120_H1);

    EXPECT_EQ(Xake::FILE_H, fileh1);
    EXPECT_EQ(Xake::RANK_1, rankh1);

    Xake::File filea8 = square_file(Xake::SQ120_A8);
    Xake::Rank ranka8 = square_rank(Xake::SQ120_A8);

    EXPECT_EQ(Xake::FILE_A, filea8);
    EXPECT_EQ(Xake::RANK_8, ranka8);

    Xake::File fileh8 = square_file(Xake::SQ120_H8);
    Xake::Rank rankh8 = square_rank(Xake::SQ120_H8);

    EXPECT_EQ(Xake::FILE_H, fileh8);
    EXPECT_EQ(Xake::RANK_8, rankh8);

    Xake::File filed1 = square_file(Xake::SQ120_D1);
    Xake::Rank rankd1 = square_rank(Xake::SQ120_D1);

    EXPECT_EQ(Xake::FILE_D, filed1);
    EXPECT_EQ(Xake::RANK_1, rankd1);

    Xake::File filee4 = square_file(Xake::SQ120_E4);
    Xake::Rank ranke4 = square_rank(Xake::SQ120_E4);

    EXPECT_EQ(Xake::FILE_E, filee4);
    EXPECT_EQ(Xake::RANK_4, ranke4);

    Xake::File filef8 = square_file(Xake::SQ120_F8);
    Xake::Rank rankf8 = square_rank(Xake::SQ120_F8);

    EXPECT_EQ(Xake::FILE_F, filef8);
    EXPECT_EQ(Xake::RANK_8, rankf8);

    Xake::File fileg1 = square_file(Xake::SQ120_G1);
    Xake::Rank rankg1 = square_rank(Xake::SQ120_G1);

    EXPECT_EQ(Xake::FILE_G, fileg1);
    EXPECT_EQ(Xake::RANK_1, rankg1);

    Xake::File fileb8 = square_file(Xake::SQ120_B8);
    Xake::Rank rankb8 = square_rank(Xake::SQ120_B8);

    EXPECT_EQ(Xake::FILE_B, fileb8);
    EXPECT_EQ(Xake::RANK_8, rankb8);

    Xake::File fileb1 = square_file(Xake::SQ120_B1);
    Xake::Rank rankb1 = square_rank(Xake::SQ120_B1);

    EXPECT_EQ(Xake::FILE_B, fileb1);
    EXPECT_EQ(Xake::RANK_1, rankb1);

    Xake::File fileg2 = square_file(Xake::SQ120_G2);
    Xake::Rank rankg2 = square_rank(Xake::SQ120_G2);

    EXPECT_EQ(Xake::FILE_G, fileg2);
    EXPECT_EQ(Xake::RANK_2, rankg2);

    Xake::File fileb2 = square_file(Xake::SQ120_B2);
    Xake::Rank rankb2 = square_rank(Xake::SQ120_B2);

    EXPECT_EQ(Xake::FILE_B, fileb2);
    EXPECT_EQ(Xake::RANK_2, rankb2);

    Xake::File fileg7 = square_file(Xake::SQ120_G7);
    Xake::Rank rankg7 = square_rank(Xake::SQ120_G7);

    EXPECT_EQ(Xake::FILE_G, fileg7);
    EXPECT_EQ(Xake::RANK_7, rankg7);
}