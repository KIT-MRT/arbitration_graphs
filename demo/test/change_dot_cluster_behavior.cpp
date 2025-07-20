#include "demo/change_dot_cluster_behavior.hpp"

#include <gtest/gtest.h>

#include "mock_environment_model.hpp"

namespace demo {

class ChangeDotClusterBehaviorTest : public ::testing::Test {
protected:
    ChangeDotClusterBehaviorTest() {
        setMazeWithTwoClusters();
        environmentModel.setGhostPositions({1, 1});
    }

    void setMazeWithoutClusters() {
        const char str[] = {"#####"
                            "#   #"
                            "#   #"
                            "#   #"
                            "#   #"
                            "#####"};
        environmentModel.setMaze({5, 6}, str);
    }
    void setMazeWithOneCluster() {
        const char str[] = {"#####"
                            "#o..#"
                            "#   #"
                            "#   #"
                            "#   #"
                            "#####"};
        environmentModel.setMaze({5, 6}, str);
    }
    void setMazeWithTwoClusters() {
        const char str[] = {"#####"
                            "#o..#"
                            "#   #"
                            "#   #"
                            "#.. #"
                            "#####"};
        environmentModel.setMaze({5, 6}, str);
    }

    MockEnvironmentModel environmentModel;

    ChangeDotClusterBehavior changeDotClusterBehavior;
};

TEST_F(ChangeDotClusterBehaviorTest, checkInvocationConditionTrue) {
    Time time = Clock::now();

    // The invocation condition should be true if...

    // ...we are outside the only cluster that's left
    setMazeWithOneCluster();
    environmentModel.setPacmanPosition({1, 3});

    ASSERT_TRUE(changeDotClusterBehavior.checkInvocationCondition(time, environmentModel));

    // ...or there are multiple clusters left. Doesn't matter if we are outside...
    setMazeWithTwoClusters();
    environmentModel.setPacmanPosition({1, 3});
    ASSERT_TRUE(changeDotClusterBehavior.checkInvocationCondition(time, environmentModel));

    // .. or inside a cluster in that case
    environmentModel.setPacmanPosition({1, 1});
    ASSERT_TRUE(changeDotClusterBehavior.checkInvocationCondition(time, environmentModel));
}

TEST_F(ChangeDotClusterBehaviorTest, checkInvocationConditionFalse) {
    Time time = Clock::now();

    // The invocation condition should be false if...

    // ..there are no clusters left
    setMazeWithoutClusters();
    environmentModel.setPacmanPosition({1, 3});

    ASSERT_FALSE(changeDotClusterBehavior.checkInvocationCondition(time, environmentModel));

    // ...we are inside the only cluster that's left
    setMazeWithOneCluster();
    environmentModel.setPacmanPosition({1, 1});

    ASSERT_FALSE(changeDotClusterBehavior.checkInvocationCondition(time, environmentModel));
}

TEST_F(ChangeDotClusterBehaviorTest, checkCommitmentConditionTrue) {
    Time time = Clock::now();
    setMazeWithTwoClusters();
    environmentModel.setPacmanPosition({1, 2});
    ASSERT_TRUE(changeDotClusterBehavior.checkInvocationCondition(time, environmentModel));

    // Once we gained control, we commit to reaching the target dot cluster
    changeDotClusterBehavior.gainControl(time, environmentModel);
    ASSERT_TRUE(changeDotClusterBehavior.checkCommitmentCondition(time, environmentModel));
}

TEST_F(ChangeDotClusterBehaviorTest, checkCommitmentConditionFalse) {
    Time time = Clock::now();
    setMazeWithTwoClusters();
    environmentModel.setPacmanPosition({1, 2});
    ASSERT_TRUE(changeDotClusterBehavior.checkInvocationCondition(time, environmentModel));
    changeDotClusterBehavior.gainControl(time, environmentModel);

    // Once we reached the target cluster, we finished our intended behavior and give up the commitment
    environmentModel.setPacmanPosition({2, 1});
    ASSERT_FALSE(changeDotClusterBehavior.checkCommitmentCondition(time, environmentModel));

    // We reached our goal no matter if we reached the cluster center or just any of the cluster dots
    environmentModel.setPacmanPosition({1, 1});
    ASSERT_FALSE(changeDotClusterBehavior.checkCommitmentCondition(time, environmentModel));
}

TEST_F(ChangeDotClusterBehaviorTest, getCommand) {
    Time time = Clock::now();
    setMazeWithTwoClusters();
    environmentModel.setPacmanPosition({2, 2});
    ASSERT_TRUE(changeDotClusterBehavior.checkInvocationCondition(time, environmentModel));

    changeDotClusterBehavior.gainControl(time, environmentModel);

    // The resulting command should navigate us towards the closest cluster center
    Command command = changeDotClusterBehavior.getCommand(time, environmentModel);
    ASSERT_EQ(command.nextDirection(), Direction::Up);
}

} // namespace demo
