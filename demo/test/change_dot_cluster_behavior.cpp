#include "demo/change_dot_cluster_behavior.hpp"

#include <memory>

#include <gtest/gtest.h>

#include "mock_environment_model.hpp"

namespace demo {

class ChangeDotClusterBehaviorTest : public ::testing::Test {
protected:
    ChangeDotClusterBehaviorTest() {
        setMazeWithTwoClusters();
        environmentModel_.setGhostPositions({1, 1});
    }

    void setMazeWithoutClusters() {
        const char str[] = {"#####"
                            "#   #"
                            "#   #"
                            "#   #"
                            "#   #"
                            "#####"};
        environmentModel_.setMaze({5, 6}, str);
    }
    void setMazeWithOneCluster() {
        const char str[] = {"#####"
                            "#o..#"
                            "#   #"
                            "#   #"
                            "#   #"
                            "#####"};
        environmentModel_.setMaze({5, 6}, str);
    }
    void setMazeWithTwoClusters() {
        const char str[] = {"#####"
                            "#o..#"
                            "#   #"
                            "#   #"
                            "#.. #"
                            "#####"};
        environmentModel_.setMaze({5, 6}, str);
    }

    MockEnvironmentModel environmentModel_{};

    ChangeDotClusterBehavior changeDotClusterBehavior_{};
};

TEST_F(ChangeDotClusterBehaviorTest, checkInvocationConditionTrue) {
    Time time = Clock::now();

    // The invocation condition should be true if...

    // ...we are outside the only cluster that's left
    setMazeWithOneCluster();
    environmentModel_.setPacmanPosition({1, 3});

    ASSERT_TRUE(changeDotClusterBehavior_.checkInvocationCondition(time, environmentModel_));

    // ...or there are multiple clusters left. Doesn't matter if we are outside...
    setMazeWithTwoClusters();
    environmentModel_.setPacmanPosition({1, 3});
    ASSERT_TRUE(changeDotClusterBehavior_.checkInvocationCondition(time, environmentModel_));

    // .. or inside a cluster in that case
    environmentModel_.setPacmanPosition({1, 1});
    ASSERT_TRUE(changeDotClusterBehavior_.checkInvocationCondition(time, environmentModel_));
}

TEST_F(ChangeDotClusterBehaviorTest, checkInvocationConditionFalse) {
    Time time = Clock::now();

    // The invocation condition should be false if...

    // ..there are no clusters left
    setMazeWithoutClusters();
    environmentModel_.setPacmanPosition({1, 3});

    ASSERT_FALSE(changeDotClusterBehavior_.checkInvocationCondition(time, environmentModel_));

    // ...we are inside the only cluster that's left
    setMazeWithOneCluster();
    environmentModel_.setPacmanPosition({1, 1});

    ASSERT_FALSE(changeDotClusterBehavior_.checkInvocationCondition(time, environmentModel_));
}

TEST_F(ChangeDotClusterBehaviorTest, checkCommitmentConditionTrue) {
    Time time = Clock::now();
    setMazeWithTwoClusters();
    environmentModel_.setPacmanPosition({1, 2});
    ASSERT_TRUE(changeDotClusterBehavior_.checkInvocationCondition(time, environmentModel_));

    // Once we gained control, we commit to reaching the target dot cluster
    changeDotClusterBehavior_.gainControl(time, environmentModel_);
    ASSERT_TRUE(changeDotClusterBehavior_.checkCommitmentCondition(time, environmentModel_));
}

TEST_F(ChangeDotClusterBehaviorTest, checkCommitmentConditionFalse) {
    Time time = Clock::now();
    setMazeWithTwoClusters();
    environmentModel_.setPacmanPosition({1, 2});
    ASSERT_TRUE(changeDotClusterBehavior_.checkInvocationCondition(time, environmentModel_));
    changeDotClusterBehavior_.gainControl(time, environmentModel_);

    // Once we reached the target cluster, we finished our intended behavior and give up the commitment
    environmentModel_.setPacmanPosition({2, 1});
    ASSERT_FALSE(changeDotClusterBehavior_.checkCommitmentCondition(time, environmentModel_));

    // We reached our goal no matter if we reached the cluster center or just any of the cluster dots
    environmentModel_.setPacmanPosition({1, 1});
    ASSERT_FALSE(changeDotClusterBehavior_.checkCommitmentCondition(time, environmentModel_));
}

TEST_F(ChangeDotClusterBehaviorTest, getCommand) {
    Time time = Clock::now();
    setMazeWithTwoClusters();
    environmentModel_.setPacmanPosition({2, 2});
    ASSERT_TRUE(changeDotClusterBehavior_.checkInvocationCondition(time, environmentModel_));

    changeDotClusterBehavior_.gainControl(time, environmentModel_);

    // The resulting command should navigate us towards the closest cluster center
    Command command = changeDotClusterBehavior_.getCommand(time, environmentModel_);
    ASSERT_EQ(command.nextDirection(), Direction::Up);
}

} // namespace demo
