#include "demo/avoid_ghost_behavior.hpp"

#include <memory>

#include <gtest/gtest.h>

#include "mock_environment_model.hpp"

namespace demo {

class AvoidGhostBehaviorTest : public ::testing::Test {
protected:
    AvoidGhostBehaviorTest() : avoidGhostBehavior_{parameters_} {
    }

    AvoidGhostBehavior::Parameters parameters_;
    MockEnvironmentModel environmentModel_{};

    AvoidGhostBehavior avoidGhostBehavior_;
};

TEST_F(AvoidGhostBehaviorTest, checkInvocationConditionTrue) {
    environmentModel_.setPacmanPosition({1, 1});
    environmentModel_.setGhostPositions({2, 3});

    Time time = Clock::now();
    ASSERT_TRUE(avoidGhostBehavior_.checkInvocationCondition(time, environmentModel_));
}

TEST_F(AvoidGhostBehaviorTest, checkInvocationConditionFalse) {
    environmentModel_.setPacmanPosition({1, 1});
    environmentModel_.setGhostPositions({8, 8});

    Time time = Clock::now();
    ASSERT_FALSE(avoidGhostBehavior_.checkInvocationCondition(time, environmentModel_));
}

TEST_F(AvoidGhostBehaviorTest, checkCommitmentConditionTrue) {
    environmentModel_.setPacmanPosition({1, 1});
    environmentModel_.setGhostPositions({2, 3});

    Time time = Clock::now();
    ASSERT_TRUE(avoidGhostBehavior_.checkCommitmentCondition(time, environmentModel_));
}

TEST_F(AvoidGhostBehaviorTest, checkCommitmentConditionFalse) {
    environmentModel_.setPacmanPosition({1, 1});
    environmentModel_.setGhostPositions({8, 8});

    Time time = Clock::now();
    ASSERT_FALSE(avoidGhostBehavior_.checkCommitmentCondition(time, environmentModel_));
}

TEST_F(AvoidGhostBehaviorTest, getCommandLeft) {
    const char str[] = {"###"
                        "   "
                        "###"};
    environmentModel_.setMaze({3, 3}, str);

    environmentModel_.setPacmanPosition({1, 1});
    environmentModel_.setGhostPositions({2, 1});

    Time time = Clock::now();
    Command command = avoidGhostBehavior_.getCommand(time, environmentModel_);
    ASSERT_EQ(command.nextDirection(), Direction::LEFT);
}

TEST_F(AvoidGhostBehaviorTest, getCommandRight) {
    const char str[] = {"###"
                        "   "
                        "###"};
    environmentModel_.setMaze({3, 3}, str);

    environmentModel_.setPacmanPosition({1, 1});
    environmentModel_.setGhostPositions({0, 1});

    Time time = Clock::now();
    Command command = avoidGhostBehavior_.getCommand(time, environmentModel_);
    ASSERT_EQ(command.nextDirection(), Direction::RIGHT);
}

TEST_F(AvoidGhostBehaviorTest, getCommandDown) {
    const char str[] = {"# #"
                        "# #"
                        "# #"};
    environmentModel_.setMaze({3, 3}, str);

    environmentModel_.setPacmanPosition({1, 1});
    environmentModel_.setGhostPositions({1, 0});

    Time time = Clock::now();
    Command command = avoidGhostBehavior_.getCommand(time, environmentModel_);
    ASSERT_EQ(command.nextDirection(), Direction::DOWN);
}

TEST_F(AvoidGhostBehaviorTest, getCommandUp) {
    const char str[] = {"# #"
                        "# #"
                        "# #"};
    environmentModel_.setMaze({3, 3}, str);

    environmentModel_.setPacmanPosition({1, 1});
    environmentModel_.setGhostPositions({1, 2});

    Time time = Clock::now();
    Command command = avoidGhostBehavior_.getCommand(time, environmentModel_);
    ASSERT_EQ(command.nextDirection(), Direction::UP);
}

TEST_F(AvoidGhostBehaviorTest, getCommandAwayFromWall) {
    const char str[] = {"###"
                        "#  "
                        "###"};
    environmentModel_.setMaze({3, 3}, str);

    environmentModel_.setPacmanPosition({1, 1});
    environmentModel_.setGhostPositions({2, 1});

    // Even though there is a ghost, pacman should not move towards a wall.
    Time time = Clock::now();
    Command command = avoidGhostBehavior_.getCommand(time, environmentModel_);
    ASSERT_EQ(command.nextDirection(), Direction::RIGHT);
}

} // namespace demo
