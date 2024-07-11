#include "demo/do_nothing_behavior.hpp"

#include <gtest/gtest.h>

namespace demo {

TEST(DoNothingBehaviorTest, checkInvocationConditionTrue) {
    demo::DoNothingBehavior doNothingBehavior;
    ASSERT_TRUE(doNothingBehavior.checkInvocationCondition(Clock::now()));
}

TEST(DoNothingBehaviorTest, checkCommitmentConditionFalse) {
    demo::DoNothingBehavior doNothingBehavior;
    ASSERT_FALSE(doNothingBehavior.checkCommitmentCondition(Clock::now()));
}

TEST(DoNothingBehaviorTest, getCommand) {
    demo::DoNothingBehavior doNothingBehavior;

    Time time = Clock::now();
    Command command = doNothingBehavior.getCommand(time);
    ASSERT_EQ(command.nextDirection(), Direction::LAST);
}

} // namespace demo
