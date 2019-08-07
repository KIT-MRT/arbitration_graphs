#pragma once

#include <memory>
#include <boost/optional.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include "behavior.hpp"


namespace behavior_planning {

template <typename CommandT>
class PriorityArbitrator : public Behavior<CommandT> {
public:
    PriorityArbitrator(const std::string& name = "PriorityArbitrator") : Behavior<CommandT>(name){};

    void addOption(const typename Behavior<CommandT>::Ptr& behavior, const bool interruptable) {
        behaviorOptions_.push_back({behavior, interruptable});
    }

    CommandT getCommand() override {
        bool activeBehaviorCanBeContinued =
            activeBehavior_ && behaviorOptions_.at(*activeBehavior_).behavior->checkCommitmentCondition();

        if (activeBehavior_ && !activeBehaviorCanBeContinued) {
            behaviorOptions_.at(*activeBehavior_).behavior->loseControl();
            activeBehavior_ = boost::none;
        }

        bool activeBehaviorInterruptable = activeBehavior_ && behaviorOptions_.at(*activeBehavior_).interruptable;

        if (!activeBehavior_ || !activeBehaviorCanBeContinued || activeBehaviorInterruptable) {
            boost::optional<int> bestOption = findBestOption();

            if (bestOption) {
                if (!activeBehavior_) {
                    activeBehavior_ = bestOption;
                    behaviorOptions_.at(*activeBehavior_).behavior->gainControl();
                } else if (*bestOption != *activeBehavior_) {
                    behaviorOptions_.at(*activeBehavior_).behavior->loseControl();

                    activeBehavior_ = bestOption;
                    behaviorOptions_.at(*activeBehavior_).behavior->gainControl();
                }
            } else {
                throw std::runtime_error("No behavior with true invocation condition found! Only call getCommand() if "
                                         "checkInvocationCondition() or checkCommitmentCondition() is true!");
            }
        }
        return behaviorOptions_.at(*activeBehavior_).behavior->getCommand();
    }

    bool checkInvocationCondition() const override {
        for (auto& option : behaviorOptions_) {
            if (option.behavior->checkInvocationCondition()) {
                return true;
            }
        }
        return false;
    }
    bool checkCommitmentCondition() const override {
        if (activeBehavior_) {
            if (behaviorOptions_.at(*activeBehavior_).behavior->checkCommitmentCondition()) {
                return true;
            } else {
                return checkInvocationCondition();
            }
        }
        return false;
    }

    virtual void gainControl() override {
    }

    virtual void loseControl() override {
        if (activeBehavior_) {
            behaviorOptions_.at(*activeBehavior_).behavior->loseControl();
        }
        activeBehavior_ = boost::none;
    }

    friend std::ostream& operator<<(std::ostream& output, const PriorityArbitrator& priority_arbitrator) {
        output << priority_arbitrator.name_;

        for (int i = 0; i < (int)priority_arbitrator.behaviorOptions_.size(); ++i) {
            const Option& option = priority_arbitrator.behaviorOptions_.at(i);
            bool isActive = priority_arbitrator.activeBehavior_ && (i == *(priority_arbitrator.activeBehavior_));

            //            int n_max_digits = (int) log10 ((double) priority_arbitrator.behaviorOptions_.size()) + 1;
            output << std::endl << "  " << i + 1 << ". ";

            if (isActive) {
                output << boost::to_upper_copy<std::string>(option.behavior->str());
            } else {
                output << *option.behavior;
            }
        }
        return output;
    }


private:
    struct Option {
        typename Behavior<CommandT>::Ptr behavior;
        bool interruptable;
    };

    /*!
     * Find behavior option with highest priority and true invocation condition
     *
     * @return  Applicable option with highest priority (can also be the currently active option)
     */
    boost::optional<int> findBestOption() const {
        for (int i = 0; i < (int)behaviorOptions_.size(); ++i) {
            const Option& option = behaviorOptions_.at(i);

            bool isActive = activeBehavior_ && (i == *activeBehavior_);
            bool isActiveAndCanBeContinued =
                isActive && behaviorOptions_.at(*activeBehavior_).behavior->checkCommitmentCondition();
            if (option.behavior->checkInvocationCondition() || isActiveAndCanBeContinued) {
                return i;
            }
        }
        return boost::none;
    }

    std::vector<Option> behaviorOptions_;
    boost::optional<int> activeBehavior_;
};
} // namespace behavior_planning
