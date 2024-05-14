#include "demo/run_away_from_ghost_behavior.hpp"
#include <memory>

#include <gtest/gtest.h>

#include "demo/environment_model.hpp"

namespace demo {

class MockEnvironmentModel : public EnvironmentModel {
public:
    using Ptr = std::shared_ptr<MockEnvironmentModel>;
    using ConstPtr = std::shared_ptr<const MockEnvironmentModel>;

    MockEnvironmentModel() : EnvironmentModel() {
        entityPositions_.pacman = {1, 1};
        entityPositions_.blinky = {2, 2};
        entityPositions_.pinky = {3, 3};
        entityPositions_.inky = {4, 4};
        entityPositions_.clyde = {5, 5};
    }

    PositionStore positions() const {
        return entityPositions_;
    }
    void setPositions(const PositionStore& positions) {
        entityPositions_ = positions;
    }
};


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
    Time time = Clock::now();
    ASSERT_TRUE(runAwayFromGhostBehavior_.checkInvocationCondition(time));
}

TEST_F(RunAwayFromGhostBehaviorTest, checkInvocationConditionFalse) {
    auto positions = environmentModel_->positions();
    positions.pacman = {100, 100};
    environmentModel_->setPositions(positions);

    Time time = Clock::now();
    ASSERT_FALSE(runAwayFromGhostBehavior_.checkInvocationCondition(time));
}

TEST_F(RunAwayFromGhostBehaviorTest, checkCommitmentConditionTrue) {
    Time time = Clock::now();
    ASSERT_TRUE(runAwayFromGhostBehavior_.checkCommitmentCondition(time));
}

TEST_F(RunAwayFromGhostBehaviorTest, checkCommitmentConditionFalse) {
    auto positions = environmentModel_->positions();
    positions.pacman = {100, 100};
    environmentModel_->setPositions(positions);

    Time time = Clock::now();
    ASSERT_FALSE(runAwayFromGhostBehavior_.checkCommitmentCondition(time));
}

TEST_F(RunAwayFromGhostBehaviorTest, getCommandLeft) {
    Time time = Clock::now();
    Command command = runAwayFromGhostBehavior_.getCommand(time);
    ASSERT_EQ(command.direction, Direction::LEFT);
}

TEST_F(RunAwayFromGhostBehaviorTest, getCommandRight) {
    auto positions = environmentModel_->positions();
    positions.pacman = {6, 6};
    environmentModel_->setPositions(positions);

    Time time = Clock::now();
    Command command = runAwayFromGhostBehavior_.getCommand(time);
    ASSERT_EQ(command.direction, Direction::RIGHT);
}

TEST_F(RunAwayFromGhostBehaviorTest, getCommandDown) {
    auto positions = environmentModel_->positions();
    positions.pacman = {2, 0};
    environmentModel_->setPositions(positions);

    Time time = Clock::now();
    Command command = runAwayFromGhostBehavior_.getCommand(time);
    ASSERT_EQ(command.direction, Direction::DOWN);
}

TEST_F(RunAwayFromGhostBehaviorTest, getCommandUp) {
    auto positions = environmentModel_->positions();
    positions.pacman = {5, 6};
    environmentModel_->setPositions(positions);

    Time time = Clock::now();
    Command command = runAwayFromGhostBehavior_.getCommand(time);
    ASSERT_EQ(command.direction, Direction::UP);
}

} // namespace demo
