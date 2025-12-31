#include <chrono>

#include <gtest/gtest.h>

#include "timemanager.h"

using namespace Akerbeltz;

namespace {

using Ms = TimeManager::Ms;

TEST(TimeManagerTest, NoTimeControlsMeansInfiniteBudget) {
    TimeManager tm;
    TimeManager::BudgetParams params{};
    tm.allocate_budget(params);

    EXPECT_FALSE(tm.out_of_time());
    EXPECT_FALSE(tm.remaining_ms().has_value());
    EXPECT_TRUE(tm.enough_time_for_next_iteration(Ms{100}));
}

TEST(TimeManagerTest, MoveTimeEqualOverheadYieldsZeroRemaining) {
    TimeManager tm;
    TimeManager::BudgetParams params{};
    params.moveTimeMs = Ms{50};
    params.overhead = Ms{50};
    tm.allocate_budget(params);

    const auto rem = tm.remaining_ms();
    ASSERT_TRUE(rem.has_value());
    EXPECT_EQ(rem->count(), 0);
}

TEST(TimeManagerTest, MoveTimeBudgetIsFinite) {
    TimeManager tm;
    TimeManager::BudgetParams params{};
    params.moveTimeMs = Ms{1000};
    params.overhead = Ms{0};
    tm.allocate_budget(params);

    const auto rem = tm.remaining_ms();
    ASSERT_TRUE(rem.has_value());
    EXPECT_GE(rem->count(), 0);
    EXPECT_LE(rem->count(), 1000);
}

TEST(TimeManagerTest, ColorTimeBudgetUsesMovesToGo) {
    TimeManager tm;
    TimeManager::BudgetParams params{};
    params.colorTimeMs = Ms{10000};
    params.movesToGo = 20;
    params.overhead = Ms{0};
    tm.allocate_budget(params);

    const auto rem = tm.remaining_ms();
    ASSERT_TRUE(rem.has_value());
    EXPECT_GE(rem->count(), 0);
    EXPECT_LE(rem->count(), 500);
}

TEST(TimeManagerTest, EnoughTimeForNextIterationRejectsLargeNeedUnderSmallBudget) {
    TimeManager tm;
    TimeManager::BudgetParams params{};
    params.moveTimeMs = Ms{10};
    params.overhead = Ms{5};
    tm.allocate_budget(params);

    EXPECT_FALSE(tm.enough_time_for_next_iteration(Ms{100}));
}

TEST(TimeManagerTest, EnoughTimeForNextIterationAcceptsSmallNeedWithLargeBudget) {
    TimeManager tm;
    TimeManager::BudgetParams params{};
    params.moveTimeMs = Ms{5000};
    params.overhead = Ms{0};
    tm.allocate_budget(params);

    EXPECT_TRUE(tm.enough_time_for_next_iteration(Ms{1}));
}

}  // namespace
