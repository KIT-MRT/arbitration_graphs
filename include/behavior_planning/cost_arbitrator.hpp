#pragma once

#include <iomanip>
#include <memory>
#include <boost/optional.hpp>

#include "arbitrator.hpp"


namespace behavior_planning {

template <typename CommandT>
class CostArbitrator : public Arbitrator<CommandT> {
public:
    using Ptr = std::shared_ptr<CostArbitrator>;

    struct CostEstimator {
        using Ptr = std::shared_ptr<CostEstimator>;

        virtual double estimateCost(const CommandT& command, const bool isActive) = 0;
    };

    struct Option : Arbitrator<CommandT>::Option {
        using Ptr = std::shared_ptr<Option>;
        using FlagsT = typename Arbitrator<CommandT>::Option::FlagsT;

        enum Flags { NO_FLAGS = 0b0, INTERRUPTABLE = 0b1 };

        Option(const typename Behavior<CommandT>::Ptr& behavior,
               const FlagsT& flags,
               const typename CostEstimator::Ptr& costEstimator)
                : Arbitrator<CommandT>::Option(behavior, flags), costEstimator_{costEstimator} {
        }

        typename CostEstimator::Ptr costEstimator_;
        mutable boost::optional<double> last_estimated_cost_;
    };


    CostArbitrator(const std::string& name = "CostArbitrator") : Arbitrator<CommandT>(name){};


    void addOption(const typename Behavior<CommandT>::Ptr& behavior,
                   const typename Option::Flags& flags,
                   const typename CostEstimator::Ptr& costEstimator) {
        typename Option::Ptr option = std::make_shared<Option>(behavior, flags, costEstimator);
        this->behaviorOptions_.push_back(option);
    }

    /*!
     * \brief Writes a string representation of the arbitrator object with its current state to the given output stream.
     *
     * \param output    Output stream to write into, will be returned also
     * \param prefix    A string that should be prepended to each line that is written to the output stream
     * \param suffix    A string that should be appended to each line that is written to the output stream
     * \return          The same given input stream (signature similar to std::ostream& operator<<())
     *
     * \see Arbitrator::to_stream()
     */
    virtual std::ostream& to_stream(std::ostream& output,
                                    const std::string& prefix = "",
                                    const std::string& suffix = "") const override {
        Behavior<CommandT>::to_stream(output, prefix, suffix);

        for (int i = 0; i < (int)this->behaviorOptions_.size(); ++i) {
            typename Option::Ptr option = std::dynamic_pointer_cast<Option>(this->behaviorOptions_.at(i));
            bool isActive = this->activeBehavior_ && (i == *(this->activeBehavior_));

            std::stringstream cost_string;
            if (option->last_estimated_cost_) {
                cost_string << std::fixed << std::setprecision(3) << *option->last_estimated_cost_;
            } else {
                cost_string << " n.a.";
            }

            if (isActive) {
                output << suffix << std::endl << prefix << " -> - (cost: " << cost_string.str() << ") ";
            } else {
                output << suffix << std::endl << prefix << "    - (cost: " << cost_string.str() << ") ";
            }
            option->behavior_->to_stream(output, "    " + prefix, suffix);
        }
        return output;
    }


private:
    /*!
     * Find behavior option with lowest cost and true invocation condition
     *
     * @return  Applicable option with lowest costs (can also be the currently active option)
     */
    boost::optional<int> findBestOption() const {
        double costOfBestOption = std::numeric_limits<double>::max();
        boost::optional<int> bestOption;

        for (int i = 0; i < (int)this->behaviorOptions_.size(); ++i) {
            typename Option::Ptr option = std::dynamic_pointer_cast<Option>(this->behaviorOptions_.at(i));

            bool isActive = this->activeBehavior_ && (i == *this->activeBehavior_);
            bool isActiveAndCanBeContinued =
                isActive && this->behaviorOptions_.at(*this->activeBehavior_)->behavior_->checkCommitmentCondition();
            if (option->behavior_->checkInvocationCondition() || isActiveAndCanBeContinued) {
                double cost = option->costEstimator_->estimateCost(option->behavior_->getCommand(), isActive);
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
