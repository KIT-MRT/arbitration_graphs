#include "demo/move_randomly_behavior.hpp"

#include <gtest/gtest.h>

namespace demo {

TEST(MoveRandomlyTest, checkInvocationConditionTrue) {
    MoveRandomlyBehavior moveRandomlyBehavior(MoveRandomlyBehavior::Parameters{});
    ASSERT_TRUE(moveRandomlyBehavior.checkInvocationCondition(Clock::now()));
}

TEST(MoveRandomlyTest, checkCommitmentConditionFalse) {
    MoveRandomlyBehavior moveRandomlyBehavior(MoveRandomlyBehavior::Parameters{});
    ASSERT_FALSE(moveRandomlyBehavior.checkCommitmentCondition(Clock::now()));
}

TEST(MoveRandomlyTest, getCachedCommand) {
    Duration selectionValidFor{std::chrono::seconds(1)};
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
    MoveRandomlyBehavior moveRandomlyBehavior(MoveRandomlyBehavior::Parameters{selectionValidFor});

    Time time = Clock::now();

    int sampleSize = 1000;
    std::map<Direction, int> directionCounter{
        {Direction::UP, 0}, {Direction::DOWN, 0}, {Direction::LEFT, 0}, {Direction::RIGHT, 0}};

    for (int i = 0; i < sampleSize; i++) {
        Direction direction = moveRandomlyBehavior.getCommand(time, environmentModel).nextDirection();
        directionCounter[direction]++;

        // We need to progress time to force a re-selection of a new direction
        time += selectionValidFor;
    }

    // Every possible direction should be chosen roughly the same amount of time
    EXPECT_NEAR(0.25, directionCounter[Direction::UP] / static_cast<double>(sampleSize), 0.1);
    EXPECT_NEAR(0.25, directionCounter[Direction::DOWN] / static_cast<double>(sampleSize), 0.1);
    EXPECT_NEAR(0.25, directionCounter[Direction::LEFT] / static_cast<double>(sampleSize), 0.1);
    EXPECT_NEAR(0.25, directionCounter[Direction::RIGHT] / static_cast<double>(sampleSize), 0.1);
}

} // namespace demo
