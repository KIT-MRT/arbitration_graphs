#include "demo/stay_in_place_behavior.hpp"

#include <gtest/gtest.h>

namespace demo {

TEST(DoNothingBehaviorTest, checkInvocationConditionTrue) {
    StayInPlaceBehavior stayInPlaceBehavior;
    ASSERT_TRUE(stayInPlaceBehavior.checkInvocationCondition(Clock::now()));
}

TEST(DoNothingBehaviorTest, checkCommitmentConditionFalse) {
    StayInPlaceBehavior stayInPlaceBehavior;
    ASSERT_FALSE(stayInPlaceBehavior.checkCommitmentCondition(Clock::now()));
}

TEST(DoNothingBehaviorTest, getCommand) {
    StayInPlaceBehavior stayInPlaceBehavior;

    Time time = Clock::now();
    Command command = stayInPlaceBehavior.getCommand(time);
    ASSERT_EQ(command.nextDirection(), Direction::LAST);
}

} // namespace demo
