#include "demo/stay_in_place_behavior.hpp"

#include <gtest/gtest.h>

#include "mock_environment_model.hpp"

namespace demo {


class StayInPlaceBehaviorTest : public ::testing::Test {
protected:
    StayInPlaceBehaviorTest()
            : environmentModel_(std::make_shared<MockEnvironmentModel>()), stayInPlaceBehavior_{environmentModel_} {
    }

    MockEnvironmentModel::Ptr environmentModel_;

    StayInPlaceBehavior stayInPlaceBehavior_;
};

TEST_F(StayInPlaceBehaviorTest, checkInvocationConditionTrue) {
    ASSERT_TRUE(stayInPlaceBehavior_.checkInvocationCondition(Clock::now()));
}

TEST_F(StayInPlaceBehaviorTest, checkCommitmentConditionFalse) {
    ASSERT_FALSE(stayInPlaceBehavior_.checkCommitmentCondition(Clock::now()));
}

TEST_F(StayInPlaceBehaviorTest, getCommand) {
    Time time = Clock::now();

    environmentModel_->setPacmanDirection(Direction::LEFT);
    Command command = stayInPlaceBehavior_.getCommand(time);
    ASSERT_EQ(command.nextDirection(), Direction::RIGHT);

    environmentModel_->setPacmanDirection(Direction::RIGHT);
    command = stayInPlaceBehavior_.getCommand(time);
    ASSERT_EQ(command.nextDirection(), Direction::LEFT);

    environmentModel_->setPacmanDirection(Direction::UP);
    command = stayInPlaceBehavior_.getCommand(time);
    ASSERT_EQ(command.nextDirection(), Direction::DOWN);

    environmentModel_->setPacmanDirection(Direction::DOWN);
    command = stayInPlaceBehavior_.getCommand(time);
    ASSERT_EQ(command.nextDirection(), Direction::UP);
}

} // namespace demo
