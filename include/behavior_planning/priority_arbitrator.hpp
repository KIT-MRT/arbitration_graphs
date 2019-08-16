#pragma once

#include <memory>
#include <boost/optional.hpp>

#include "behavior.hpp"


namespace behavior_planning {

template <typename CommandT>
class PriorityArbitrator : public Behavior<CommandT> {
public:
    using Ptr = std::shared_ptr<PriorityArbitrator>;

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

    /*!
     * \brief Writes a string representation of the arbitrator object with its current state to the given output stream.
     *
     * \param output    Output stream to write into, will be returned also
     * \param prefix    A string that should be prepended to each line that is written to the output stream
     * \param suffix    A string that should be appended to each line that is written to the output stream
     * \return          The same given input stream (signature similar to std::ostream& operator<<())
     *
     * \see Behavior::to_stream()
     */
    virtual std::ostream& to_stream(std::ostream& output,
                                    const std::string& prefix = "",
                                    const std::string& suffix = "") const override {
        Behavior<CommandT>::to_stream(output, prefix, suffix);

        for (int i = 0; i < (int)behaviorOptions_.size(); ++i) {
            const Option& option = behaviorOptions_.at(i);
            bool isActive = activeBehavior_ && (i == *(activeBehavior_));

            //            int n_max_digits = (int) log10 ((double) priority_arbitrator.behaviorOptions_.size()) + 1;
            if (isActive) {
                output << suffix << std::endl << prefix << " -> " << i + 1 << ". ";
            } else {
                output << suffix << std::endl << prefix << "    " << i + 1 << ". ";
            }
            option.behavior->to_stream(output, "    " + prefix, suffix);
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
