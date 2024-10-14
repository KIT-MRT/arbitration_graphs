#pragma once

#include <utility>

#include "behavior.hpp"
#include "verification.hpp"


namespace arbitration_graphs_tests {

using namespace arbitration_graphs;

const std::string strikeThroughOn = "×××\010\010\010\033[9m";
const std::string strikeThroughOff = "\033[29m\033[8m×××\033[28m";

const std::string invocationTrueString = "\033[32mINVOCATION\033[39m ";
const std::string invocationFalseString = "\033[31mInvocation\033[39m ";
const std::string commitmentTrueString = "\033[32mCOMMITMENT\033[39m ";
const std::string commitmentFalseString = "\033[31mCommitment\033[39m ";


using DummyCommand = std::string;

class DummyCommandInt {
public:
    DummyCommandInt(const int command) : command_{command} {
    }
    DummyCommandInt(const DummyCommand& command) {
        command_ = static_cast<std::string>(command).length();
    }

    bool operator==(const int other_command) const {
        return command_ == other_command;
    }

    int command_;
};
bool operator==(const int command_int, const DummyCommandInt& command_object) {
    return command_int == command_object.command_;
}

class DummyBehavior : public Behavior<DummyCommand> {
public:
    using Ptr = std::shared_ptr<DummyBehavior>;

    DummyBehavior(const bool invocation, const bool commitment, const std::string& name = "DummyBehavior")
            : Behavior(name), invocationCondition_{invocation}, commitmentCondition_{commitment} {};

    DummyCommand getCommand(const Time& time) override {
        getCommandCounter_++;
        return name_;
    }
    bool checkInvocationCondition(const Time& time) const override {
        return invocationCondition_;
    }
    bool checkCommitmentCondition(const Time& time) const override {
        return commitmentCondition_;
    }
    virtual void loseControl(const Time& time) override {
        loseControlCounter_++;
    }

    bool invocationCondition_;
    bool commitmentCondition_;
    int getCommandCounter_{0};
    int loseControlCounter_{0};
};

class BrokenDummyBehavior : public DummyBehavior {
public:
    BrokenDummyBehavior(const bool invocation, const bool commitment, const std::string& name = "BrokenDummyBehavior")
            : DummyBehavior(invocation, commitment, name) {};

    DummyCommand getCommand(const Time& time) override {
        throw std::runtime_error("BrokenDummyBehavior::getCommand() is broken");
    }
};

struct DummyResult : public verification::PlaceboResult {};

struct DummyVerifier {
    explicit DummyVerifier(std::string wrong) : wrong_{std::move(wrong)} {
    }
    DummyResult analyze(const Time& /*time*/, const DummyCommand& data) const {
        if (data == wrong_) {
            return DummyResult{false};
        }
        return DummyResult{true};
    };
    std::string wrong_;
};

} // namespace arbitration_graphs_tests


inline std::ostream& operator<<(std::ostream& out, const arbitration_graphs_tests::DummyResult& result) {
    out << (result.isOk() ? "is okay" : "is not okay");
    return out;
}
