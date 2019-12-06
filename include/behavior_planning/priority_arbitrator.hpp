#pragma once

#include <memory>
#include <optional>

#include "arbitrator.hpp"


namespace behavior_planning {

template <typename CommandT, typename SubCommandT = CommandT>
class PriorityArbitrator : public Arbitrator<CommandT, SubCommandT> {
public:
    using Ptr = std::shared_ptr<PriorityArbitrator>;

    struct Option : public Arbitrator<CommandT, SubCommandT>::Option {
    public:
        using Ptr = std::shared_ptr<Option>;
        using FlagsT = typename Arbitrator<CommandT, SubCommandT>::Option::FlagsT;

        enum Flags { NO_FLAGS = 0b0, INTERRUPTABLE = 0b1 };

        Option(const typename Behavior<SubCommandT>::Ptr& behavior, const FlagsT& flags)
                : Arbitrator<CommandT, SubCommandT>::Option(behavior, flags) {
        }

        /*!
         * \brief Writes a string representation of the behavior option and its current state to the output stream.
         *
         * \param output        Output stream to write into, will be returned also
         * \param option_index  Position index of this option within behaviorOptions_
         * \param prefix        A string that should be prepended to each line that is written to the output stream
         * \param suffix        A string that should be appended to each line that is written to the output stream
         * \return              The same given input stream (signature similar to std::ostream& operator<<())
         *
         * \see Arbitrator::to_stream()
         */
        virtual std::ostream& to_stream(std::ostream& output,
                                        const int& option_index,
                                        const std::string& prefix = "",
                                        const std::string& suffix = "") const {
            output << option_index + 1 << ". ";
            Arbitrator<CommandT, SubCommandT>::Option::to_stream(output, option_index, prefix, suffix);
            return output;
        }
    };

    PriorityArbitrator(const std::string& name = "PriorityArbitrator") : Arbitrator<CommandT, SubCommandT>(name){};

    void addOption(const typename Behavior<SubCommandT>::Ptr& behavior, const typename Option::Flags& flags) {
        typename Option::Ptr option = std::make_shared<Option>(behavior, flags);
        this->behaviorOptions_.push_back(option);
    }

protected:
    /*!
     * Find behavior option with highest priority and true invocation condition
     *
     * @return  Applicable option with highest priority (can also be the currently active option)
     */
    std::optional<int> findBestOption() const {
        for (int i = 0; i < (int)this->behaviorOptions_.size(); ++i) {
            typename Option::Ptr option = std::dynamic_pointer_cast<Option>(this->behaviorOptions_.at(i));

            bool isActive = this->activeBehavior_ && (i == *this->activeBehavior_);
            bool isActiveAndCanBeContinued =
                isActive && this->behaviorOptions_.at(*this->activeBehavior_)->behavior_->checkCommitmentCondition();
            if (option->behavior_->checkInvocationCondition() || isActiveAndCanBeContinued) {
                return i;
            }
        }
        return std::nullopt;
    }
};
} // namespace behavior_planning
