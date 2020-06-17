#pragma once

#include <memory>
#include <optional>

#include <yaml-cpp/yaml.h>

#include "arbitrator.hpp"


namespace behavior_planning {

template <typename CommandT, typename SubCommandT = CommandT>
class PriorityArbitrator : public Arbitrator<CommandT, SubCommandT> {
public:
    using Ptr = std::shared_ptr<PriorityArbitrator>;
    using ConstPtr = std::shared_ptr<const PriorityArbitrator>;

    struct Option : public Arbitrator<CommandT, SubCommandT>::Option {
    public:
        using Ptr = std::shared_ptr<Option>;
        using FlagsT = typename Arbitrator<CommandT, SubCommandT>::Option::FlagsT;
        using ConstPtr = std::shared_ptr<const Option>;

        enum Flags { NO_FLAGS = 0b0, INTERRUPTABLE = 0b1 };

        Option(const typename Behavior<SubCommandT>::Ptr& behavior, const FlagsT& flags)
                : Arbitrator<CommandT, SubCommandT>::Option(behavior, flags) {
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
            output << option_index + 1 << ". ";
            Arbitrator<CommandT, SubCommandT>::Option::to_stream(output, time, option_index, prefix, suffix);
            return output;
        }
    };

    PriorityArbitrator(const std::string& name = "PriorityArbitrator") : Arbitrator<CommandT, SubCommandT>(name){};

    void addOption(const typename Behavior<SubCommandT>::Ptr& behavior, const typename Option::Flags& flags) {
        typename Option::Ptr option = std::make_shared<Option>(behavior, flags);
        this->behaviorOptions_.push_back(option);
    }

    /*!
     * \brief Returns a yaml representation of the arbitrator object with its current state
     *
     * \param time  Expected execution time point of this behaviors command
     * \return      Yaml representation of this behavior
     */
    virtual YAML::Node toYaml(const Time& time) const override {
        YAML::Node node = Arbitrator<CommandT, SubCommandT>::toYaml(time);
        node["type"] = "PriorityArbitrator";
        return node;
    }

protected:
    /*!
     * Find behavior option with highest priority and true invocation condition
     *
     * @return  Applicable option with highest priority (can also be the currently active option)
     */
    std::optional<int> findBestOption(const Time& time) const {
        for (int i = 0; i < (int)this->behaviorOptions_.size(); ++i) {
            typename Option::Ptr option = std::dynamic_pointer_cast<Option>(this->behaviorOptions_.at(i));

            bool isActive = this->activeBehavior_ && (i == *this->activeBehavior_);
            bool isActiveAndCanBeContinued =
                isActive &&
                this->behaviorOptions_.at(*this->activeBehavior_)->behavior_->checkCommitmentCondition(time);
            if (option->behavior_->checkInvocationCondition(time) || isActiveAndCanBeContinued) {
                return i;
            }
        }
        return std::nullopt;
    }
};
} // namespace behavior_planning
