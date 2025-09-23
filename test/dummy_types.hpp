#pragma once

#include "behavior.hpp"
#include "verification.hpp"


namespace arbitration_graphs_tests {

using namespace arbitration_graphs;

const std::string StrikeThroughOn = "×××\010\010\010\033[9m";
const std::string StrikeThroughOff = "\033[29m\033[8m×××\033[28m";

const std::string InvocationTrueString = "\033[32mINVOCATION\033[39m ";
const std::string InvocationFalseString = "\033[31mInvocation\033[39m ";
const std::string CommitmentTrueString = "\033[32mCOMMITMENT\033[39m ";
const std::string CommitmentFalseString = "\033[31mCommitment\033[39m ";


using DummyCommand = std::string;

class DummyCommandInt {
public:
    explicit DummyCommandInt(const int command) : command{command} {
    }
    // NOLINTNEXTLINE(google-explicit-constructor)
    DummyCommandInt(const DummyCommand& strCommand)
            : command{static_cast<int>(static_cast<std::string>(strCommand).length())} {};

    bool operator==(const int otherCommand) const {
        return command == otherCommand;
    }

    int command;
};
inline bool operator==(const int commandInt, const DummyCommandInt& commandObject) {
    return commandInt == commandObject.command;
}

struct DummyEnvironmentModel {
    // A mock getter function that simply simulates accessing the environment model.
    void getObservation() const {
        accessCounter++;
    }

    mutable int accessCounter{0};
};

class DummyBehavior : public Behavior<DummyEnvironmentModel, DummyCommand> {
public:
    using Ptr = std::shared_ptr<DummyBehavior>;

    DummyBehavior(const bool invocation, const bool commitment, const std::string& name = "DummyBehavior")
            : Behavior(name), invocationCondition{invocation}, commitmentCondition{commitment} {};
    DummyBehavior(const DummyBehavior&) = default;
    DummyBehavior(DummyBehavior&&) = delete;
    DummyBehavior& operator=(const DummyBehavior&) = default;
    DummyBehavior& operator=(DummyBehavior&&) = delete;
    virtual ~DummyBehavior() = default;

    DummyCommand getCommand(const Time& /*time*/, const DummyEnvironmentModel& environmentModel) override {
        // While computing a command, we can read the environment model to get the required context. In this mock
        // implementation, we merely simulate this access to assure that this interaction is possible. You will find
        // analogous calls to the environment model in the other methods of this class.
        environmentModel.getObservation();
        getCommandCounter++;
        return {name()};
    }
    bool checkInvocationCondition(const Time& /*time*/, const DummyEnvironmentModel& environmentModel) const override {
        environmentModel.getObservation();
        return invocationCondition;
    }
    bool checkCommitmentCondition(const Time& /*time*/, const DummyEnvironmentModel& environmentModel) const override {
        environmentModel.getObservation();
        return commitmentCondition;
    }
    void gainControl(const Time& /*time*/, const DummyEnvironmentModel& environmentModel) override {
        environmentModel.getObservation();
    }
    void loseControl(const Time& /*time*/, const DummyEnvironmentModel& environmentModel) override {
        environmentModel.getObservation();
        loseControlCounter++;
    }

    bool invocationCondition;
    bool commitmentCondition;
    int getCommandCounter{0};
    int loseControlCounter{0};
};

class BrokenDummyBehavior : public DummyBehavior {
public:
    BrokenDummyBehavior(const bool invocation,
                        const bool commitment,
                        const std::string& name = "BrokenDummyBehavior",
                        int numGetCommandsUntilThrow = 0)
            : DummyBehavior(invocation, commitment, name), numGetCommandsUntilThrow_{numGetCommandsUntilThrow} {};

    DummyCommand getCommand(const Time& /*time*/, const DummyEnvironmentModel& environmentModel) override {
        if (getCommandCounter >= numGetCommandsUntilThrow_) {
            throw std::runtime_error("BrokenDummyBehavior::getCommand() is broken");
        }
        environmentModel.getObservation();

        getCommandCounter++;
        return name();
    }

private:
    int numGetCommandsUntilThrow_;
};

using DummyResult = verification::SimpleResult;

} // namespace arbitration_graphs_tests


inline std::ostream& operator<<(std::ostream& out, const arbitration_graphs_tests::DummyResult& result) {
    out << (result.isOk() ? "is okay" : "is not okay");
    return out;
}
