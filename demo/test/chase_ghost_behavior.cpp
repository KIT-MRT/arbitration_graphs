#include "demo/chase_ghost_behavior.hpp"

#include <gtest/gtest.h>

#include "mock_environment_model.hpp"

namespace demo {

class ChaseGhostBehaviorTest : public ::testing::Test {
protected:
    ChaseGhostBehaviorTest() : chaseGhostBehavior_{parameters_} {
    }

    ChaseGhostBehavior::Parameters parameters_{};
    MockEnvironmentModel environmentModel_{};

    ChaseGhostBehavior chaseGhostBehavior_;
};

TEST_F(ChaseGhostBehaviorTest, checkInvocationConditionTrue) {
    environmentModel_.setPacmanPosition({1, 1});
    environmentModel_.setGhostPositions({2, 2});
    environmentModel_.setGhostMode(GhostMode::SCARED);
    environmentModel_.setScaredCountdown(40);

    Time time = Clock::now();
    ASSERT_TRUE(chaseGhostBehavior_.checkInvocationCondition(time, environmentModel_));
}

TEST_F(ChaseGhostBehaviorTest, checkInvocationConditionFalse) {
    environmentModel_.setGhostMode(GhostMode::CHASING);

    Time time = Clock::now();
    ASSERT_FALSE(chaseGhostBehavior_.checkInvocationCondition(time, environmentModel_));

    // We don't want to chase ghosts when they are far away
    environmentModel_.setGhostMode(GhostMode::SCARED);
    environmentModel_.setScaredCountdown(40);
    environmentModel_.setPacmanPosition({1, 1});
    environmentModel_.setGhostPositions({8, 8});
    ASSERT_FALSE(chaseGhostBehavior_.checkInvocationCondition(time, environmentModel_));

    // We don't want to chase ghosts when they are about to chase us again
    environmentModel_.setScaredCountdown(2);
    environmentModel_.setGhostPositions({2, 2});
    ASSERT_FALSE(chaseGhostBehavior_.checkInvocationCondition(time, environmentModel_));
}

TEST_F(ChaseGhostBehaviorTest, checkCommitmentConditionTrue) {
    environmentModel_.setPacmanPosition({1, 1});
    environmentModel_.setGhostPositions({2, 2});
    environmentModel_.setGhostMode(GhostMode::SCARED);
    environmentModel_.setScaredCountdown(40);

    Time time = Clock::now();
    ASSERT_TRUE(chaseGhostBehavior_.checkCommitmentCondition(time, environmentModel_));
}

TEST_F(ChaseGhostBehaviorTest, checkCommitmentConditionFalse) {
    environmentModel_.setGhostMode(GhostMode::CHASING);

    Time time = Clock::now();
    ASSERT_FALSE(chaseGhostBehavior_.checkInvocationCondition(time, environmentModel_));

    // We don't want to chase ghosts when they are far away
    environmentModel_.setGhostMode(GhostMode::SCARED);
    environmentModel_.setScaredCountdown(40);
    environmentModel_.setPacmanPosition({1, 1});
    environmentModel_.setGhostPositions({8, 8});
    ASSERT_FALSE(chaseGhostBehavior_.checkInvocationCondition(time, environmentModel_));

    // We don't want to chase ghosts when they are about to chase us again
    environmentModel_.setScaredCountdown(2);
    environmentModel_.setGhostPositions({2, 2});
    ASSERT_FALSE(chaseGhostBehavior_.checkInvocationCondition(time, environmentModel_));
}

TEST_F(ChaseGhostBehaviorTest, getCommandLeft) {
    const char str[] = {"###"
                        "   "
                        "###"};
    environmentModel_.setMaze({3, 3}, str);

    environmentModel_.setPacmanPosition({1, 1});
    environmentModel_.setGhostPositions({0, 1});
    environmentModel_.setGhostMode(GhostMode::SCARED);
    environmentModel_.setScaredCountdown(40);

    Time time = Clock::now();
    Command command = chaseGhostBehavior_.getCommand(time, environmentModel_);
    ASSERT_EQ(command.nextDirection(), Direction::LEFT);
}

TEST_F(ChaseGhostBehaviorTest, getCommandRight) {
    const char str[] = {"###"
                        "   "
                        "###"};
    environmentModel_.setMaze({3, 3}, str);

    environmentModel_.setPacmanPosition({1, 1});
    environmentModel_.setGhostPositions({2, 1});
    environmentModel_.setGhostMode(GhostMode::SCARED);
    environmentModel_.setScaredCountdown(40);

    Time time = Clock::now();
    Command command = chaseGhostBehavior_.getCommand(time, environmentModel_);
    ASSERT_EQ(command.nextDirection(), Direction::RIGHT);
}

TEST_F(ChaseGhostBehaviorTest, getCommandDown) {
    const char str[] = {"# #"
                        "# #"
                        "# #"};
    environmentModel_.setMaze({3, 3}, str);

    environmentModel_.setPacmanPosition({1, 1});
    environmentModel_.setGhostPositions({1, 2});
    environmentModel_.setGhostMode(GhostMode::SCARED);
    environmentModel_.setScaredCountdown(40);

    Time time = Clock::now();
    Command command = chaseGhostBehavior_.getCommand(time, environmentModel_);
    ASSERT_EQ(command.nextDirection(), Direction::DOWN);
}

TEST_F(ChaseGhostBehaviorTest, getCommandUp) {
    const char str[] = {"# #"
                        "# #"
                        "# #"};
    environmentModel_.setMaze({3, 3}, str);

    environmentModel_.setPacmanPosition({1, 1});
    environmentModel_.setGhostPositions({1, 0});
    environmentModel_.setGhostMode(GhostMode::SCARED);
    environmentModel_.setScaredCountdown(40);

    Time time = Clock::now();
    Command command = chaseGhostBehavior_.getCommand(time, environmentModel_);
    ASSERT_EQ(command.nextDirection(), Direction::UP);
}

} // namespace demo
