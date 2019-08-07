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

    std::string str() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    friend std::ostream& operator<<(std::ostream& output, const Behavior& behavior) {
        if (behavior.checkInvocationCondition()) {
            output << "\033[32mINVOCATION\033[0m ";
        } else {
            output << "\033[31mInvocation\033[0m ";
        }
        if (behavior.checkCommitmentCondition()) {
            output << "\033[32mCOMMITMENT\033[0m ";
        } else {
            output << "\033[31mCommitment\033[0m ";
        }

        output << behavior.name_;
        return output;
    }

    const std::string name_;
};
} // namespace behavior_planning
