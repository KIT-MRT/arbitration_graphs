#include "demo/avoid_ghost_behavior.hpp"

#include <gtest/gtest.h>

#include "mock_environment_model.hpp"

namespace demo {

class AvoidGhostBehaviorTest : public ::testing::Test {
protected:
    AvoidGhostBehaviorTest() : avoidGhostBehavior{parameters} {
    }

    // NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes)
    AvoidGhostBehavior::Parameters parameters{};
    MockEnvironmentModel environmentModel;

    AvoidGhostBehavior avoidGhostBehavior;
    // NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes)
};

TEST_F(AvoidGhostBehaviorTest, checkInvocationConditionTrue) {
    environmentModel.setPacmanPosition({1, 1});
    environmentModel.setGhostPositions({2, 3});

    Time time = Clock::now();
    ASSERT_TRUE(avoidGhostBehavior.checkInvocationCondition(time, environmentModel));
}

TEST_F(AvoidGhostBehaviorTest, checkInvocationConditionFalse) {
    environmentModel.setPacmanPosition({1, 1});
    environmentModel.setGhostPositions({8, 8});

    Time time = Clock::now();
    ASSERT_FALSE(avoidGhostBehavior.checkInvocationCondition(time, environmentModel));
}

TEST_F(AvoidGhostBehaviorTest, checkCommitmentConditionTrue) {
    environmentModel.setPacmanPosition({1, 1});
    environmentModel.setGhostPositions({2, 3});

    Time time = Clock::now();
    ASSERT_TRUE(avoidGhostBehavior.checkCommitmentCondition(time, environmentModel));
}

TEST_F(AvoidGhostBehaviorTest, checkCommitmentConditionFalse) {
    environmentModel.setPacmanPosition({1, 1});
    environmentModel.setGhostPositions({8, 8});

    Time time = Clock::now();
    ASSERT_FALSE(avoidGhostBehavior.checkCommitmentCondition(time, environmentModel));
}

TEST_F(AvoidGhostBehaviorTest, getCommandLeft) {
    environmentModel.setMaze({3, 3},
                             "###"
                             "   "
                             "###");

    environmentModel.setPacmanPosition({1, 1});
    environmentModel.setGhostPositions({2, 1});

    Time time = Clock::now();
    Command command = avoidGhostBehavior.getCommand(time, environmentModel);
    ASSERT_EQ(command.nextDirection(), Direction::Left);
}

TEST_F(AvoidGhostBehaviorTest, getCommandRight) {
    environmentModel.setMaze({3, 3},
                             "###"
                             "   "
                             "###");

    environmentModel.setPacmanPosition({1, 1});
    environmentModel.setGhostPositions({0, 1});

    Time time = Clock::now();
    Command command = avoidGhostBehavior.getCommand(time, environmentModel);
    ASSERT_EQ(command.nextDirection(), Direction::Right);
}

TEST_F(AvoidGhostBehaviorTest, getCommandDown) {
    environmentModel.setMaze({3, 3},
                             "# #"
                             "# #"
                             "# #");

    environmentModel.setPacmanPosition({1, 1});
    environmentModel.setGhostPositions({1, 0});

    Time time = Clock::now();
    Command command = avoidGhostBehavior.getCommand(time, environmentModel);
    ASSERT_EQ(command.nextDirection(), Direction::Down);
}

TEST_F(AvoidGhostBehaviorTest, getCommandUp) {
    environmentModel.setMaze({3, 3},
                             "# #"
                             "# #"
                             "# #");

    environmentModel.setPacmanPosition({1, 1});
    environmentModel.setGhostPositions({1, 2});

    Time time = Clock::now();
    Command command = avoidGhostBehavior.getCommand(time, environmentModel);
    ASSERT_EQ(command.nextDirection(), Direction::Up);
}

TEST_F(AvoidGhostBehaviorTest, getCommandAwayFromWall) {
    environmentModel.setMaze({3, 3},
                             "###"
                             "#  "
                             "###");

    environmentModel.setPacmanPosition({1, 1});
    environmentModel.setGhostPositions({2, 1});

    // Even though there is a ghost, pacman should not move towards a wall.
    Time time = Clock::now();
    Command command = avoidGhostBehavior.getCommand(time, environmentModel);
    ASSERT_EQ(command.nextDirection(), Direction::Right);
}

} // namespace demo
