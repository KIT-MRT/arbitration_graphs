#include "demo/move_randomly_behavior.hpp"

#include <gtest/gtest.h>

#include "mock_environment_model.hpp"

namespace demo {

TEST(MoveRandomlyTest, checkInvocationConditionTrue) {
    MoveRandomlyBehavior moveRandomlyBehavior(MoveRandomlyBehavior::Parameters{});
    ASSERT_TRUE(moveRandomlyBehavior.checkInvocationCondition(Clock::now(), MockEnvironmentModel()));
}

TEST(MoveRandomlyTest, checkCommitmentConditionFalse) {
    MoveRandomlyBehavior moveRandomlyBehavior(MoveRandomlyBehavior::Parameters{});
    ASSERT_FALSE(moveRandomlyBehavior.checkCommitmentCondition(Clock::now(), MockEnvironmentModel()));
}

TEST(MoveRandomlyTest, getCachedCommand) {
    Duration selectionValidFor{std::chrono::seconds(1)};
    MockEnvironmentModel environmentModel;
    MoveRandomlyBehavior moveRandomlyBehavior(MoveRandomlyBehavior::Parameters{selectionValidFor});

    Time time = Clock::now();
    Command firstCommand = moveRandomlyBehavior.getCommand(time, environmentModel);

    time += 0.5 * selectionValidFor;
    Command secondCommand = moveRandomlyBehavior.getCommand(time, environmentModel);

    // We return the first selection until after selectionValidFor so the commands should be identical
    ASSERT_EQ(firstCommand.nextDirection(), secondCommand.nextDirection());
}

TEST(MoveRandomlyTest, getRandomCommand) {
    Duration selectionValidFor{std::chrono::seconds(1)};
    MockEnvironmentModel environmentModel{};
    MoveRandomlyBehavior moveRandomlyBehavior(MoveRandomlyBehavior::Parameters{selectionValidFor});

    Time time = Clock::now();

    int sampleSize = 1000;
    std::map<Direction, int> directionCounter{
        {Direction::Up, 0}, {Direction::Down, 0}, {Direction::Left, 0}, {Direction::Right, 0}};

    for (int i = 0; i < sampleSize; i++) {
        Direction direction = moveRandomlyBehavior.getCommand(time, environmentModel).nextDirection();
        directionCounter[direction]++;

        // We need to progress time to force a re-selection of a new direction
        time += selectionValidFor;
    }

    // Every possible direction should be chosen roughly the same amount of time
    EXPECT_NEAR(0.25, directionCounter[Direction::Up] / static_cast<double>(sampleSize), 0.1);
    EXPECT_NEAR(0.25, directionCounter[Direction::Down] / static_cast<double>(sampleSize), 0.1);
    EXPECT_NEAR(0.25, directionCounter[Direction::Left] / static_cast<double>(sampleSize), 0.1);
    EXPECT_NEAR(0.25, directionCounter[Direction::Right] / static_cast<double>(sampleSize), 0.1);
}

} // namespace demo
