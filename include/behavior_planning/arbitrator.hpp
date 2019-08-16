#pragma once

#include <memory>
#include <boost/optional.hpp>

#include "behavior.hpp"


namespace behavior_planning {

template <typename CommandT>
class Arbitrator : public Behavior<CommandT> {
public:
    using Ptr = std::shared_ptr<Arbitrator>;

    struct Option {
    public:
        using Ptr = std::shared_ptr<Option>;

        enum Flags { NO_FLAGS = 0b0, INTERRUPTABLE = 0b1 };
        using FlagsT = std::underlying_type_t<Flags>;

        Option(const typename Behavior<CommandT>::Ptr& behavior, const FlagsT& flags)
                : behavior_{behavior}, flags_{flags} {
        }
        virtual ~Option() = default;

        typename Behavior<CommandT>::Ptr behavior_;
        FlagsT flags_;

        bool hasFlag(const FlagsT& flag_to_check) const {
            return flags_ & flag_to_check;
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
            behavior_->to_stream(output, prefix, suffix);
            return output;
        }
    };

    Arbitrator(const std::string& name = "Arbitrator") : Behavior<CommandT>(name){};


    void addOption(const typename Behavior<CommandT>::Ptr& behavior, const typename Option::Flags& flags) {
        behaviorOptions_.push_back({behavior, flags});
    }

    CommandT getCommand() override {
        bool activeBehaviorCanBeContinued =
            activeBehavior_ && behaviorOptions_.at(*activeBehavior_)->behavior_->checkCommitmentCondition();

        if (activeBehavior_ && !activeBehaviorCanBeContinued) {
            behaviorOptions_.at(*activeBehavior_)->behavior_->loseControl();
            activeBehavior_ = boost::none;
        }

        bool activeBehaviorInterruptable =
            activeBehavior_ && (behaviorOptions_.at(*activeBehavior_)->hasFlag(Option::Flags::INTERRUPTABLE));

        if (!activeBehavior_ || !activeBehaviorCanBeContinued || activeBehaviorInterruptable) {
            boost::optional<int> bestOption = findBestOption();

            if (bestOption) {
                if (!activeBehavior_) {
                    activeBehavior_ = bestOption;
                    behaviorOptions_.at(*activeBehavior_)->behavior_->gainControl();
                } else if (*bestOption != *activeBehavior_) {
                    behaviorOptions_.at(*activeBehavior_)->behavior_->loseControl();

                    activeBehavior_ = bestOption;
                    behaviorOptions_.at(*activeBehavior_)->behavior_->gainControl();
                }
            } else {
                throw std::runtime_error("No behavior with true invocation condition found! Only call getCommand() if "
                                         "checkInvocationCondition() or checkCommitmentCondition() is true!");
            }
        }
        return behaviorOptions_.at(*activeBehavior_)->behavior_->getCommand();
    }

    bool checkInvocationCondition() const override {
        for (auto& option : behaviorOptions_) {
            if (option->behavior_->checkInvocationCondition()) {
                return true;
            }
        }
        return false;
    }
    bool checkCommitmentCondition() const override {
        if (activeBehavior_) {
            if (behaviorOptions_.at(*activeBehavior_)->behavior_->checkCommitmentCondition()) {
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
            behaviorOptions_.at(*activeBehavior_)->behavior_->loseControl();
        }
        activeBehavior_ = boost::none;
    }

    /*!
     * \brief Writes a string representation of the Arbitrator object with its current state to the output stream.
     *
     * \param output        Output stream to write into, will be returned also
     * \param prefix        A string that should be prepended to each line that is written to the output stream
     * \param suffix        A string that should be appended to each line that is written to the output stream
     * \return              The same given input stream (signature similar to std::ostream& operator<<())
     *
     * \see Behavior::to_stream()
     */
    virtual std::ostream& to_stream(std::ostream& output,
                                    const std::string& prefix = "",
                                    const std::string& suffix = "") const override {
        Behavior<CommandT>::to_stream(output, prefix, suffix);

        for (int i = 0; i < (int)behaviorOptions_.size(); ++i) {
            const typename Option::Ptr& option = behaviorOptions_.at(i);
            bool isActive = activeBehavior_ && (i == *(activeBehavior_));

            if (isActive) {
                output << suffix << std::endl << prefix << " -> ";
            } else {
                output << suffix << std::endl << prefix << "    ";
            }
            option->to_stream(output, i, "    " + prefix, suffix);
        }
        return output;
    }


protected:
    /*!
     * Override this function in a specialized Arbitrator in order to
     * find a behavior option that is the best option according to your policy and has true invocation condition
     *
     * @return  Best applicable option according to your policy (can also be the currently active option)
     */
    virtual boost::optional<int> findBestOption() const = 0;

    std::vector<typename Option::Ptr> behaviorOptions_;
    boost::optional<int> activeBehavior_;
};
} // namespace behavior_planning
