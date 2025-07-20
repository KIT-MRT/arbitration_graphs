#pragma once

#include <arbitration_graphs/behavior.hpp>

#include "demo/environment_model.hpp"
#include "demo/types.hpp"


namespace demo {

const std::string StrikeThroughOn = "×××\010\010\010\033[9m";
const std::string StrikeThroughOff = "\033[29m\033[8m×××\033[28m";

const std::string InvocationTrueString = "\033[32mINVOCATION\033[39m ";
const std::string InvocationFalseString = "\033[31mInvocation\033[39m ";
const std::string CommitmentTrueString = "\033[32mCOMMITMENT\033[39m ";
const std::string CommitmentFalseString = "\033[31mCommitment\033[39m ";


class DummyBehavior : public arbitration_graphs::Behavior<EnvironmentModel, Command> {
public:
    using Ptr = std::shared_ptr<DummyBehavior>;

    DummyBehavior(const bool invocation,
                  const bool commitment,
                  const Command& command,
                  const std::string& name = "DummyBehavior")
            : Behavior(name), invocationCondition{invocation}, commitmentCondition{commitment}, command{command} {};

    Command getCommand(const Time& /*time*/, const EnvironmentModel& /*environmentModel*/) override {
        return command;
    }

    bool checkInvocationCondition(const Time& /*time*/, const EnvironmentModel& /*environmentModel*/) const override {
        return invocationCondition;
    }
    bool checkCommitmentCondition(const Time& /*time*/, const EnvironmentModel& /*environmentModel*/) const override {
        return commitmentCondition;
    }

    bool invocationCondition;
    bool commitmentCondition;
    Command command;
};

} // namespace demo
