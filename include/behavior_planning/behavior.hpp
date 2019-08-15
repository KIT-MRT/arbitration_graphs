#pragma once

#include <iostream>
#include <memory>
#include <sstream>


namespace behavior_planning {

template <typename CommandT>
class Behavior {
public:
    using Ptr = std::shared_ptr<Behavior>;

    Behavior(const std::string& name = "Behavior") : name_{name} {
    }

    virtual CommandT getCommand() = 0;
    virtual bool checkInvocationCondition() const {
        return false;
    }
    virtual bool checkCommitmentCondition() const {
        return false;
    }
    virtual void gainControl() {
    }
    virtual void loseControl() {
    }

    virtual std::string to_str() const {
        std::stringstream ss;
        to_stream(ss);
        return ss.str();
    }

    virtual std::ostream& to_stream(std::ostream& output) const {
        if (checkInvocationCondition()) {
            output << "\033[32mINVOCATION\033[0m ";
        } else {
            output << "\033[31mInvocation\033[0m ";
        }
        if (checkCommitmentCondition()) {
            output << "\033[32mCOMMITMENT\033[0m ";
        } else {
            output << "\033[31mCommitment\033[0m ";
        }

        output << name_;
        return output;
    }

    const std::string name_;
};


template <typename CommandT>
std::ostream& operator<<(std::ostream& output, const Behavior<CommandT>& behavior) {
    behavior.to_stream(output);
    return output;
}
} // namespace behavior_planning
