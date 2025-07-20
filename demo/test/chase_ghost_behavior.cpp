#include "demo/chase_ghost_behavior.hpp"

#include <gtest/gtest.h>

#include "mock_environment_model.hpp"

namespace demo {

class ChaseGhostBehaviorTest : public ::testing::Test {
protected:
    ChaseGhostBehaviorTest() : chaseGhostBehavior{parameters} {
    }

    // NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes)
    ChaseGhostBehavior::Parameters parameters{};
    MockEnvironmentModel environmentModel;

    ChaseGhostBehavior chaseGhostBehavior;
    // NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes)
};

TEST_F(ChaseGhostBehaviorTest, checkInvocationConditionTrue) {
    environmentModel.setPacmanPosition({1, 1});
    environmentModel.setGhostPositions({2, 2});
    environmentModel.setGhostMode(GhostMode::Scared);
    environmentModel.setScaredCountdown(40);

    Time time = Clock::now();
    ASSERT_TRUE(chaseGhostBehavior.checkInvocationCondition(time, environmentModel));
}

TEST_F(ChaseGhostBehaviorTest, checkInvocationConditionFalse) {
    environmentModel.setGhostMode(GhostMode::Chasing);

    Time time = Clock::now();
    ASSERT_FALSE(chaseGhostBehavior.checkInvocationCondition(time, environmentModel));

    // We don't want to chase ghosts when they are far away
    environmentModel.setGhostMode(GhostMode::Scared);
    environmentModel.setScaredCountdown(40);
    environmentModel.setPacmanPosition({1, 1});
    environmentModel.setGhostPositions({8, 8});
    ASSERT_FALSE(chaseGhostBehavior.checkInvocationCondition(time, environmentModel));

    // We don't want to chase ghosts when they are about to chase us again
    environmentModel.setScaredCountdown(2);
    environmentModel.setGhostPositions({2, 2});
    ASSERT_FALSE(chaseGhostBehavior.checkInvocationCondition(time, environmentModel));
}

TEST_F(ChaseGhostBehaviorTest, checkCommitmentConditionTrue) {
    environmentModel.setPacmanPosition({1, 1});
    environmentModel.setGhostPositions({2, 2});
    environmentModel.setGhostMode(GhostMode::Scared);
    environmentModel.setScaredCountdown(40);

    Time time = Clock::now();
    ASSERT_TRUE(chaseGhostBehavior.checkCommitmentCondition(time, environmentModel));
}

TEST_F(ChaseGhostBehaviorTest, checkCommitmentConditionFalse) {
    environmentModel.setGhostMode(GhostMode::Chasing);

    Time time = Clock::now();
    ASSERT_FALSE(chaseGhostBehavior.checkInvocationCondition(time, environmentModel));

    // We don't want to chase ghosts when they are far away
    environmentModel.setGhostMode(GhostMode::Scared);
    environmentModel.setScaredCountdown(40);
    environmentModel.setPacmanPosition({1, 1});
    environmentModel.setGhostPositions({8, 8});
    ASSERT_FALSE(chaseGhostBehavior.checkInvocationCondition(time, environmentModel));

    // We don't want to chase ghosts when they are about to chase us again
    environmentModel.setScaredCountdown(2);
    environmentModel.setGhostPositions({2, 2});
    ASSERT_FALSE(chaseGhostBehavior.checkInvocationCondition(time, environmentModel));
}

TEST_F(ChaseGhostBehaviorTest, getCommandLeft) {
    environmentModel.setMaze({3, 3},
                             "###"
                             "   "
                             "###");

    environmentModel.setPacmanPosition({1, 1});
    environmentModel.setGhostPositions({0, 1});
    environmentModel.setGhostMode(GhostMode::Scared);
    environmentModel.setScaredCountdown(40);

    Time time = Clock::now();
    Command command = chaseGhostBehavior.getCommand(time, environmentModel);
    ASSERT_EQ(command.nextDirection(), Direction::Left);
}

TEST_F(ChaseGhostBehaviorTest, getCommandRight) {
    environmentModel.setMaze({3, 3},
                             "###"
                             "   "
                             "###");

    environmentModel.setPacmanPosition({1, 1});
    environmentModel.setGhostPositions({2, 1});
    environmentModel.setGhostMode(GhostMode::Scared);
    environmentModel.setScaredCountdown(40);

    Time time = Clock::now();
    Command command = chaseGhostBehavior.getCommand(time, environmentModel);
    ASSERT_EQ(command.nextDirection(), Direction::Right);
}

TEST_F(ChaseGhostBehaviorTest, getCommandDown) {
    environmentModel.setMaze({3, 3},
                             "# #"
                             "# #"
                             "# #");

    environmentModel.setPacmanPosition({1, 1});
    environmentModel.setGhostPositions({1, 2});
    environmentModel.setGhostMode(GhostMode::Scared);
    environmentModel.setScaredCountdown(40);

    Time time = Clock::now();
    Command command = chaseGhostBehavior.getCommand(time, environmentModel);
    ASSERT_EQ(command.nextDirection(), Direction::Down);
}

TEST_F(ChaseGhostBehaviorTest, getCommandUp) {
    environmentModel.setMaze({3, 3},
                             "# #"
                             "# #"
                             "# #");

    environmentModel.setPacmanPosition({1, 1});
    environmentModel.setGhostPositions({1, 0});
    environmentModel.setGhostMode(GhostMode::Scared);
    environmentModel.setScaredCountdown(40);

    Time time = Clock::now();
    Command command = chaseGhostBehavior.getCommand(time, environmentModel);
    ASSERT_EQ(command.nextDirection(), Direction::Up);
}

} // namespace demo
