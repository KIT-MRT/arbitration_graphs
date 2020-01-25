#pragma once

#include <iomanip>
#include <memory>
#include <optional>

#include "arbitrator.hpp"


namespace behavior_planning {

template <typename SubCommandT>
struct CostEstimator {
    using Ptr = std::shared_ptr<CostEstimator>;

    virtual double estimateCost(const SubCommandT& command, const bool isActive) = 0;
};

template <typename CommandT, typename SubCommandT = CommandT>
class CostArbitrator : public Arbitrator<CommandT, SubCommandT> {
public:
    using Ptr = std::shared_ptr<CostArbitrator>;

    struct Option : Arbitrator<CommandT, SubCommandT>::Option {
        using Ptr = std::shared_ptr<Option>;
        using FlagsT = typename Arbitrator<CommandT, SubCommandT>::Option::FlagsT;

        enum Flags { NO_FLAGS = 0b0, INTERRUPTABLE = 0b1 };

        Option(const typename Behavior<SubCommandT>::Ptr& behavior,
               const FlagsT& flags,
               const typename CostEstimator<SubCommandT>::Ptr& costEstimator)
                : Arbitrator<CommandT, SubCommandT>::Option(behavior, flags), costEstimator_{costEstimator} {
        }

        /*!
         * \brief Writes a string representation of the behavior option and its current state to the output stream.
         *
         * \param output        Output stream to write into, will be returned also
         * \param time          Expected execution time point of this behaviors command
         * \param option_index  Position index of this option within behaviorOptions_
         * \param prefix        A string that should be prepended to each line that is written to the output stream
         * \param suffix        A string that should be appended to each line that is written to the output stream
         * \return              The same given input stream (signature similar to std::ostream& operator<<())
         *
         * \see Arbitrator::to_stream()
         */
        virtual std::ostream& to_stream(std::ostream& output,
                                        const Time& time,
                                        const int& option_index,
                                        const std::string& prefix = "",
                                        const std::string& suffix = "") const {
            if (last_estimated_cost_) {
                output << std::fixed << std::setprecision(3) << "- (cost: " << *last_estimated_cost_ << ") ";
            } else {
                output << "- (cost:  n.a.) ";
            }

            Arbitrator<CommandT, SubCommandT>::Option::to_stream(output, time, option_index, prefix, suffix);
            return output;
        }

        typename CostEstimator<SubCommandT>::Ptr costEstimator_;
        mutable std::optional<double> last_estimated_cost_;
    };


    CostArbitrator(const std::string& name = "CostArbitrator") : Arbitrator<CommandT, SubCommandT>(name){};


    void addOption(const typename Behavior<SubCommandT>::Ptr& behavior,
                   const typename Option::Flags& flags,
                   const typename CostEstimator<SubCommandT>::Ptr& costEstimator) {
        typename Option::Ptr option = std::make_shared<Option>(behavior, flags, costEstimator);
        this->behaviorOptions_.push_back(option);
    }

private:
    /*!
     * Find behavior option with lowest cost and true invocation condition
     *
     * @return  Applicable option with lowest costs (can also be the currently active option)
     */
    std::optional<int> findBestOption(const Time& time) const {
        double costOfBestOption = std::numeric_limits<double>::max();
        std::optional<int> bestOption;

        for (int i = 0; i < (int)this->behaviorOptions_.size(); ++i) {
            typename Option::Ptr option = std::dynamic_pointer_cast<Option>(this->behaviorOptions_.at(i));

            bool isActive = this->activeBehavior_ && (i == *this->activeBehavior_);
            bool isActiveAndCanBeContinued =
                isActive &&
                this->behaviorOptions_.at(*this->activeBehavior_)->behavior_->checkCommitmentCondition(time);
            if (option->behavior_->checkInvocationCondition(time) || isActiveAndCanBeContinued) {
                double cost = option->costEstimator_->estimateCost(option->behavior_->getCommand(time), isActive);
                option->last_estimated_cost_ = cost;

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
