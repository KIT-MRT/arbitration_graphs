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
    environmentModel_->setPacmanPosition({1, 1});
    environmentModel_->setGhostPositions({2, 3});

    Time time = Clock::now();
    ASSERT_TRUE(runAwayFromGhostBehavior_.checkInvocationCondition(time));
}

TEST_F(RunAwayFromGhostBehaviorTest, checkInvocationConditionFalse) {
    environmentModel_->setPacmanPosition({1, 1});
    environmentModel_->setGhostPositions({8, 8});

    Time time = Clock::now();
    ASSERT_FALSE(runAwayFromGhostBehavior_.checkInvocationCondition(time));
}

TEST_F(RunAwayFromGhostBehaviorTest, checkCommitmentConditionTrue) {
    environmentModel_->setPacmanPosition({1, 1});
    environmentModel_->setGhostPositions({2, 3});

    Time time = Clock::now();
    ASSERT_TRUE(runAwayFromGhostBehavior_.checkCommitmentCondition(time));
}

TEST_F(RunAwayFromGhostBehaviorTest, checkCommitmentConditionFalse) {
    environmentModel_->setPacmanPosition({1, 1});
    environmentModel_->setGhostPositions({8, 8});

    Time time = Clock::now();
    ASSERT_FALSE(runAwayFromGhostBehavior_.checkCommitmentCondition(time));
}

TEST_F(RunAwayFromGhostBehaviorTest, getCommandLeft) {
    const char str[] = {"###"
                        "   "
                        "###"};
    environmentModel_->setMaze({3, 3}, str);

    environmentModel_->setPacmanPosition({1, 1});
    environmentModel_->setGhostPositions({2, 1});

    Time time = Clock::now();
    Command command = runAwayFromGhostBehavior_.getCommand(time);
    ASSERT_EQ(command.direction, Direction::LEFT);
}

TEST_F(RunAwayFromGhostBehaviorTest, getCommandRight) {
    const char str[] = {"###"
                        "   "
                        "###"};
    environmentModel_->setMaze({3, 3}, str);

    environmentModel_->setPacmanPosition({1, 1});
    environmentModel_->setGhostPositions({0, 1});

    Time time = Clock::now();
    Command command = runAwayFromGhostBehavior_.getCommand(time);
    ASSERT_EQ(command.direction, Direction::RIGHT);
}

TEST_F(RunAwayFromGhostBehaviorTest, getCommandDown) {
    const char str[] = {"# #"
                        "# #"
                        "# #"};
    environmentModel_->setMaze({3, 3}, str);

    environmentModel_->setPacmanPosition({1, 1});
    environmentModel_->setGhostPositions({1, 0});

    Time time = Clock::now();
    Command command = runAwayFromGhostBehavior_.getCommand(time);
    ASSERT_EQ(command.direction, Direction::DOWN);
}

TEST_F(RunAwayFromGhostBehaviorTest, getCommandUp) {
    const char str[] = {"# #"
                        "# #"
                        "# #"};
    environmentModel_->setMaze({3, 3}, str);

    environmentModel_->setPacmanPosition({1, 1});
    environmentModel_->setGhostPositions({1, 2});

    Time time = Clock::now();
    Command command = runAwayFromGhostBehavior_.getCommand(time);
    ASSERT_EQ(command.direction, Direction::UP);
}

TEST_F(RunAwayFromGhostBehaviorTest, getCommandAwayFromWall) {
    const char str[] = {"###"
                        "#  "
                        "###"};
    environmentModel_->setMaze({3, 3}, str);

    environmentModel_->setPacmanPosition({1, 1});
    environmentModel_->setGhostPositions({2, 1});

    // Even though there is a ghost, pacman should not move towards a wall.
    Time time = Clock::now();
    Command command = runAwayFromGhostBehavior_.getCommand(time);
    ASSERT_EQ(command.direction, Direction::RIGHT);
}

} // namespace demo
