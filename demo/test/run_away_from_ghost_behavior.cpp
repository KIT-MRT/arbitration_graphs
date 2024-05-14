#include "demo/run_away_from_ghost_behavior.hpp"

#include <memory>

#include <gtest/gtest.h>

#include "mock_environment_model.hpp"

namespace demo {

class RunAwayFromGhostBehaviorTest : public ::testing::Test {
protected:
    RunAwayFromGhostBehaviorTest()
            : parameters_{}, environmentModel_(std::make_shared<MockEnvironmentModel>()),
              runAwayFromGhostBehavior_{environmentModel_, parameters_} {
    }

    RunAwayFromGhostBehavior::Parameters parameters_;
    MockEnvironmentModel::Ptr environmentModel_;

    RunAwayFromGhostBehavior runAwayFromGhostBehavior_;
};

TEST_F(RunAwayFromGhostBehaviorTest, checkInvocationConditionTrue) {
    auto positions = environmentModel_->positions();
    positions.pacman = {0, 0};
    positions.blinky = {1, 2};
    environmentModel_->setPositions(positions);

    Time time = Clock::now();
    ASSERT_TRUE(runAwayFromGhostBehavior_.checkInvocationCondition(time));
}

TEST_F(RunAwayFromGhostBehaviorTest, checkInvocationConditionFalse) {
    auto positions = environmentModel_->positions();
    positions.pacman = {0, 0};
    environmentModel_->setPositions(positions);

    Time time = Clock::now();
    ASSERT_FALSE(runAwayFromGhostBehavior_.checkInvocationCondition(time));
}

TEST_F(RunAwayFromGhostBehaviorTest, checkCommitmentConditionTrue) {
    auto positions = environmentModel_->positions();
    positions.pacman = {0, 0};
    positions.blinky = {1, 2};
    environmentModel_->setPositions(positions);

    Time time = Clock::now();
    ASSERT_TRUE(runAwayFromGhostBehavior_.checkCommitmentCondition(time));
}

TEST_F(RunAwayFromGhostBehaviorTest, checkCommitmentConditionFalse) {
    auto positions = environmentModel_->positions();
    positions.pacman = {0,0};
    environmentModel_->setPositions(positions);

    Time time = Clock::now();
    ASSERT_FALSE(runAwayFromGhostBehavior_.checkCommitmentCondition(time));
}

TEST_F(RunAwayFromGhostBehaviorTest, getCommandLeft) {
    auto positions = environmentModel_->positions();
    positions.pacman = {4, 4};
    positions.blinky = {5, 4};
    environmentModel_->setPositions(positions);

    Time time = Clock::now();
    Command command = runAwayFromGhostBehavior_.getCommand(time);
    ASSERT_EQ(command.direction, Direction::LEFT);
}

TEST_F(RunAwayFromGhostBehaviorTest, getCommandRight) {
    auto positions = environmentModel_->positions();
    positions.pacman = {4, 4};
    positions.blinky = {3, 4};
    environmentModel_->setPositions(positions);

    Time time = Clock::now();
    Command command = runAwayFromGhostBehavior_.getCommand(time);
    ASSERT_EQ(command.direction, Direction::RIGHT);
}

TEST_F(RunAwayFromGhostBehaviorTest, getCommandDown) {
    auto positions = environmentModel_->positions();
    positions.pacman = {4, 4};
    positions.blinky = {4, 2};
    environmentModel_->setPositions(positions);

    Time time = Clock::now();
    Command command = runAwayFromGhostBehavior_.getCommand(time);
    ASSERT_EQ(command.direction, Direction::DOWN);
}

TEST_F(RunAwayFromGhostBehaviorTest, getCommandUp) {
    auto positions = environmentModel_->positions();
    positions.pacman = {4, 4};
    positions.blinky = {4, 5};
    environmentModel_->setPositions(positions);

    Time time = Clock::now();
    Command command = runAwayFromGhostBehavior_.getCommand(time);
    ASSERT_EQ(command.direction, Direction::UP);
}

} // namespace demo
