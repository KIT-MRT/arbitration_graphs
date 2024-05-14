#pragma once

#include <arbitration_graphs/behavior.hpp>

#include "types.hpp"

namespace demo {

class DoNothingBehavior : public arbitration_graphs::Behavior<Command> {
public:
    using Ptr = std::shared_ptr<DoNothingBehavior>;
    using ConstPtr = std::shared_ptr<const DoNothingBehavior>;

    DoNothingBehavior(const std::string& name = "DoNothingBehavior")
            : Behavior(name) {}

    Command getCommand(const Time& time) override {
        return {Direction::NONE};
    }

    bool checkInvocationCondition(const Time& time) const override {
        return true;
    }
    bool checkCommitmentCondition(const Time& time) const override {
        return false;
    }

    void gainControl(const Time& time) override {
        isActive_ = true;
    }
    void loseControl(const Time& time) override {
        isActive_ = false;
    }

private:
    bool isActive_{false};
};

} // namespace demo
