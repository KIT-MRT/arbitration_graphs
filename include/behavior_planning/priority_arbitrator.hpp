#pragma once

#include <memory>
#include <boost/optional.hpp>

#include "arbitrator.hpp"


namespace behavior_planning {

template <typename CommandT>
class PriorityArbitrator : public Arbitrator<CommandT> {
public:
    using Ptr = std::shared_ptr<PriorityArbitrator>;

    struct Option : public Arbitrator<CommandT>::Option {
    public:
        using Ptr = std::shared_ptr<Option>;
        using FlagsT = typename Arbitrator<CommandT>::Option::FlagsT;

        enum Flags { NO_FLAGS = 0b0, INTERRUPTABLE = 0b1 };

        Option(const typename Behavior<CommandT>::Ptr& behavior, const FlagsT& flags)
                : Arbitrator<CommandT>::Option(behavior, flags) {
        }
    };

    PriorityArbitrator(const std::string& name = "PriorityArbitrator") : Arbitrator<CommandT>(name){};

    void addOption(const typename Behavior<CommandT>::Ptr& behavior, const typename Option::Flags& flags) {
        typename Option::Ptr option = std::make_shared<Option>(behavior, flags);
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

            if (isActive) {
                output << suffix << std::endl << prefix << " -> " << i + 1 << ". ";
            } else {
                output << suffix << std::endl << prefix << "    " << i + 1 << ". ";
            }
            option->behavior_->to_stream(output, "    " + prefix, suffix);
        }
        return output;
    }

protected:
    /*!
     * Find behavior option with highest priority and true invocation condition
     *
     * @return  Applicable option with highest priority (can also be the currently active option)
     */
    boost::optional<int> findBestOption() const {
        for (int i = 0; i < (int)this->behaviorOptions_.size(); ++i) {
            typename Option::Ptr option = std::dynamic_pointer_cast<Option>(this->behaviorOptions_.at(i));

            bool isActive = this->activeBehavior_ && (i == *this->activeBehavior_);
            bool isActiveAndCanBeContinued =
                isActive && this->behaviorOptions_.at(*this->activeBehavior_)->behavior_->checkCommitmentCondition();
            if (option->behavior_->checkInvocationCondition() || isActiveAndCanBeContinued) {
                return i;
            }
        }
        return boost::none;
    }
};
} // namespace behavior_planning
