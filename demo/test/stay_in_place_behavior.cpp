#include "demo/stay_in_place_behavior.hpp"

#include <gtest/gtest.h>

#include "mock_environment_model.hpp"

namespace demo {


class StayInPlaceBehaviorTest : public ::testing::Test {
protected:
    MockEnvironmentModel environmentModel_{};
    StayInPlaceBehavior stayInPlaceBehavior_{};
};

TEST_F(StayInPlaceBehaviorTest, checkInvocationConditionTrue) {
    ASSERT_TRUE(stayInPlaceBehavior_.checkInvocationCondition(Clock::now(), environmentModel_));
}

TEST_F(StayInPlaceBehaviorTest, checkCommitmentConditionFalse) {
    ASSERT_FALSE(stayInPlaceBehavior_.checkCommitmentCondition(Clock::now(), environmentModel_));
}

TEST_F(StayInPlaceBehaviorTest, getCommand) {
    Time time = Clock::now();

    environmentModel_.setPacmanDirection(Direction::Left);
    Command command = stayInPlaceBehavior_.getCommand(time, environmentModel_);
    ASSERT_EQ(command.nextDirection(), Direction::Right);

    environmentModel_.setPacmanDirection(Direction::Right);
    command = stayInPlaceBehavior_.getCommand(time, environmentModel_);
    ASSERT_EQ(command.nextDirection(), Direction::Left);

    environmentModel_.setPacmanDirection(Direction::Up);
    command = stayInPlaceBehavior_.getCommand(time, environmentModel_);
    ASSERT_EQ(command.nextDirection(), Direction::Down);

    environmentModel_.setPacmanDirection(Direction::Down);
    command = stayInPlaceBehavior_.getCommand(time, environmentModel_);
    ASSERT_EQ(command.nextDirection(), Direction::Up);
}

} // namespace demo
