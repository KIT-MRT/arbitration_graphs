#pragma once

#include <arbitration_graphs/behavior.hpp>

#include "demo/types.hpp"


namespace demo {

const std::string strikeThroughOn = "×××\010\010\010\033[9m";
const std::string strikeThroughOff = "\033[29m\033[8m×××\033[28m";

const std::string invocationTrueString = "\033[32mINVOCATION\033[39m ";
const std::string invocationFalseString = "\033[31mInvocation\033[39m ";
const std::string commitmentTrueString = "\033[32mCOMMITMENT\033[39m ";
const std::string commitmentFalseString = "\033[31mCommitment\033[39m ";


class DummyBehavior : public arbitration_graphs::Behavior<Command> {
public:
    using Ptr = std::shared_ptr<DummyBehavior>;

    DummyBehavior(const bool invocation,
                  const bool commitment,
                  const Command& command,
                  const std::string& name = "DummyBehavior")
            : Behavior(name), invocationCondition_{invocation}, commitmentCondition_{commitment}, command_{command} {};

    Command getCommand(const Time& /*time*/) override {
        return command_;
    }

    bool checkInvocationCondition(const Time& /*time*/) const override {
        return invocationCondition_;
    }
    bool checkCommitmentCondition(const Time& /*time*/) const override {
        return commitmentCondition_;
    }

    bool invocationCondition_;
    bool commitmentCondition_;
    Command command_;
};

} // namespace demo
