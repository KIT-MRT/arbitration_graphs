#include "demo/stay_in_place_behavior.hpp"

#include <gtest/gtest.h>

#include "mock_environment_model.hpp"

namespace demo {


class StayInPlaceBehaviorTest : public ::testing::Test {
protected:
    MockEnvironmentModel environmentModel;
    StayInPlaceBehavior stayInPlaceBehavior;
};

TEST_F(StayInPlaceBehaviorTest, checkInvocationConditionTrue) {
    ASSERT_TRUE(stayInPlaceBehavior.checkInvocationCondition(Clock::now(), environmentModel));
}

TEST_F(StayInPlaceBehaviorTest, checkCommitmentConditionFalse) {
    ASSERT_FALSE(stayInPlaceBehavior.checkCommitmentCondition(Clock::now(), environmentModel));
}

TEST_F(StayInPlaceBehaviorTest, getCommand) {
    Time time = Clock::now();

    environmentModel.setPacmanDirection(Direction::Left);
    Command command = stayInPlaceBehavior.getCommand(time, environmentModel);
    ASSERT_EQ(command.nextDirection(), Direction::Right);

    environmentModel.setPacmanDirection(Direction::Right);
    command = stayInPlaceBehavior.getCommand(time, environmentModel);
    ASSERT_EQ(command.nextDirection(), Direction::Left);

    environmentModel.setPacmanDirection(Direction::Up);
    command = stayInPlaceBehavior.getCommand(time, environmentModel);
    ASSERT_EQ(command.nextDirection(), Direction::Down);

    environmentModel.setPacmanDirection(Direction::Down);
    command = stayInPlaceBehavior.getCommand(time, environmentModel);
    ASSERT_EQ(command.nextDirection(), Direction::Up);
}

} // namespace demo
