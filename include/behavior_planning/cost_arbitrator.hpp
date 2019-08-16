#pragma once

#include <iomanip>
#include <memory>
#include <boost/optional.hpp>

#include "behavior.hpp"


namespace behavior_planning {

template <typename CommandT>
class CostArbitrator : public Behavior<CommandT> {
public:
    using Ptr = std::shared_ptr<CostArbitrator>;

    struct CostEstimator {
        using Ptr = std::shared_ptr<CostEstimator>;

        virtual double estimateCost(const CommandT& command, const bool isActive) = 0;
    };

    CostArbitrator(const std::string& name = "CostArbitrator") : Behavior<CommandT>(name){};

    void addOption(const typename Behavior<CommandT>::Ptr& behavior,
                   const bool interruptable,
                   const typename CostEstimator::Ptr& costEstimator) {
        behaviorOptions_.push_back({behavior, interruptable, costEstimator, boost::none});
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

            std::stringstream cost_string;
            if (option.last_estimated_cost) {
                cost_string << std::fixed << std::setprecision(3) << *option.last_estimated_cost;
            } else {
                cost_string << " n.a.";
            }

            if (isActive) {
                output << suffix << std::endl << prefix << " -> - (cost: " << cost_string.str() << ") ";
            } else {
                output << suffix << std::endl << prefix << "    - (cost: " << cost_string.str() << ") ";
            }
            option.behavior->to_stream(output, "    " + prefix, suffix);
        }
        return output;
    }


private:
    struct Option {
        typename Behavior<CommandT>::Ptr behavior;
        bool interruptable;
        typename CostEstimator::Ptr costEstimator;
        mutable boost::optional<double> last_estimated_cost;
    };

    std::vector<Option> behaviorOptions_;
    boost::optional<int> activeBehavior_;

    /*!
     * Find behavior option with lowest cost and true invocation condition
     *
     * @return  Applicable option with lowest costs (can also be the currently active option)
     */
    boost::optional<int> findBestOption() const {
        double costOfBestOption = std::numeric_limits<double>::max();
        boost::optional<int> bestOption;

        for (int i = 0; i < (int)behaviorOptions_.size(); ++i) {
            const Option& option = behaviorOptions_.at(i);

            bool isActive = activeBehavior_ && (i == *activeBehavior_);
            bool isActiveAndCanBeContinued =
                isActive && behaviorOptions_.at(*activeBehavior_).behavior->checkCommitmentCondition();
            if (option.behavior->checkInvocationCondition() || isActiveAndCanBeContinued) {
                double cost = option.costEstimator->estimateCost(option.behavior->getCommand(), isActive);
                option.last_estimated_cost = cost;

                if (cost < costOfBestOption) {
                    costOfBestOption = cost;
                    bestOption = i;
                }
            }
        }
        return bestOption;
    }
};
} // namespace behavior_planning
