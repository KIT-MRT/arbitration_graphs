#pragma once

#include <arbitration_graphs/behavior.hpp>

#include "types.hpp"

namespace demo {

/**
 * @brief The DoNothingBehavior does just that. Nothing.
 *
 * The behavior can be called any time and will always return a command with direction NONE.
 * It is intended to be used as a fallback behavior when no other behavior is available.
 * Due to the fact that Pacman will never stop moving, this means that when this behavior is called, Pacman will keep
 * moving in the same direction as before.
 */
class DoNothingBehavior : public arbitration_graphs::Behavior<Command> {
public:
    using Ptr = std::shared_ptr<DoNothingBehavior>;
    using ConstPtr = std::shared_ptr<const DoNothingBehavior>;

    explicit DoNothingBehavior(const std::string& name = "DoNothingBehavior") : Behavior(name) {
    }

    Command getCommand(const Time& time) override {
        return Command{Direction::LAST};
    }

    bool checkInvocationCondition(const Time& time) const override {
        return true;
    }
    bool checkCommitmentCondition(const Time& time) const override {
        return false;
    }

    void gainControl(const Time& time) override {
    }
    void loseControl(const Time& time) override {
    }
};

} // namespace demo
