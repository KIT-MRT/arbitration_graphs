#pragma once

#include <algorithm>
#include <iterator>
#include <memory>
#include <optional>
#include <vector>

#include <yaml-cpp/yaml.h>

#include "behavior.hpp"
#include "exceptions.hpp"


namespace behavior_planning {

/*!
 * \brief The Arbitrator class
 *
 * \note If CommandT != SubCommandT either
 *       - override getCommand() in your specialized Arbitrator or
 *       - provide a CommandT(const SubCommandT&) constructor
 */
template <typename CommandT, typename SubCommandT = CommandT>
class Arbitrator : public Behavior<CommandT> {
public:
    using Ptr = std::shared_ptr<Arbitrator>;
    using ConstPtr = std::shared_ptr<const Arbitrator>;

    /*!
     * \brief The Option struct
     *
     * \todo explain why it is a subclass of arbitrator
     * \todo explain flags implementation and usage
     *       - MyArbitrator::Option::Flags != Arbitrator::Option::Flags (no inheritance)
     *       - addOption() checks type, but hasFlag() not anymore (otherwise each inherited Option would have to
     *         implement a new, not overriding (because signature changed) hasFlag() -> error prone)
     */
    struct Option {
    public:
        using Ptr = std::shared_ptr<Option>;
        using ConstPtr = std::shared_ptr<const Option>;

        enum Flags { NO_FLAGS = 0b0, INTERRUPTABLE = 0b1 };
        using FlagsT = std::underlying_type_t<Flags>;

        Option(const typename Behavior<SubCommandT>::Ptr& behavior, const FlagsT& flags)
                : behavior_{behavior}, flags_{flags} {
        }
        //! \todo document why we need this
        virtual ~Option() = default;

        typename Behavior<SubCommandT>::Ptr behavior_;
        FlagsT flags_;

        bool hasFlag(const FlagsT& flag_to_check) const {
            return flags_ & flag_to_check;
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
            behavior_->to_stream(output, time, prefix, suffix);
            return output;
        }

        /*!
         * \brief Returns a yaml representation of this option with its current state
         *
         * \param time  Expected execution time point of this behaviors command
         * \return      Yaml representation of this behavior
         */
        virtual YAML::Node toYaml(const Time& time) const {
            YAML::Node node;
            node["type"] = "Option";
            node["behavior"] = behavior_->toYaml(time);

            if (hasFlag(Option::Flags::INTERRUPTABLE)) {
                node["flags"].push_back("INTERRUPTABLE");
            }

            return node;
        }
    };

    Arbitrator(const std::string& name = "Arbitrator") : Behavior<CommandT>(name){};


    virtual void addOption(const typename Behavior<SubCommandT>::Ptr& behavior, const typename Option::Flags& flags) {
        typename Option::Ptr option = std::make_shared<Option>(behavior, flags);
        this->behaviorOptions_.push_back(option);
    }

    CommandT getCommand(const Time& time) override {
        bool activeBehaviorCanBeContinued =
            activeBehavior_ && activeBehavior_->behavior_->checkCommitmentCondition(time);

        if (activeBehavior_ && !activeBehaviorCanBeContinued) {
            activeBehavior_->behavior_->loseControl(time);
            activeBehavior_.reset();
        }

        bool activeBehaviorInterruptable = activeBehavior_ && (activeBehavior_->hasFlag(Option::Flags::INTERRUPTABLE));

        if (!activeBehavior_ || !activeBehaviorCanBeContinued || activeBehaviorInterruptable) {
            typename Option::Ptr bestOption = findBestOption(time);

            if (bestOption) {
                if (!activeBehavior_) {
                    activeBehavior_ = bestOption;
                    activeBehavior_->behavior_->gainControl(time);
                } else if (bestOption != activeBehavior_) {
                    activeBehavior_->behavior_->loseControl(time);

                    activeBehavior_ = bestOption;
                    activeBehavior_->behavior_->gainControl(time);
                }
            } else {
                throw InvocationConditionIsFalseError(
                    "No behavior with true invocation condition found! Only call getCommand() if "
                    "checkInvocationCondition() or checkCommitmentCondition() is true!");
            }
        }
        return activeBehavior_->behavior_->getCommand(time);
    }

    bool checkInvocationCondition(const Time& time) const override {
        for (auto& option : behaviorOptions_) {
            if (option->behavior_->checkInvocationCondition(time)) {
                return true;
            }
        }
        return false;
    }
    bool checkCommitmentCondition(const Time& time) const override {
        if (activeBehavior_) {
            if (activeBehavior_->behavior_->checkCommitmentCondition(time)) {
                return true;
            } else {
                return checkInvocationCondition(time);
            }
        }
        return false;
    }

    virtual void gainControl(const Time& time) override {
    }

    virtual void loseControl(const Time& time) override {
        if (activeBehavior_) {
            activeBehavior_->behavior_->loseControl(time);
        }
        activeBehavior_.reset();
    }

    virtual bool isActive() const {
        return activeBehavior_ != nullptr;
    }

    /*!
     * \brief Writes a string representation of the Arbitrator object with its current state to the output stream.
     *
     * \param output    Output stream to write into, will be returned also
     * \param time      Expected execution time point of this behaviors command
     * \param prefix    A string that should be prepended to each line that is written to the output stream
     * \param suffix    A string that should be appended to each line that is written to the output stream
     * \return          The same given input stream (signature similar to std::ostream& operator<<())
     *
     * \see Behavior::to_stream()
     */
    virtual std::ostream& to_stream(std::ostream& output,
                                    const Time& time,
                                    const std::string& prefix = "",
                                    const std::string& suffix = "") const override {
        Behavior<CommandT>::to_stream(output, time, prefix, suffix);

        for (int i = 0; i < (int)behaviorOptions_.size(); ++i) {
            const typename Option::Ptr& option = behaviorOptions_.at(i);
            bool isActive = activeBehavior_ && (option == activeBehavior_);

            if (isActive) {
                output << suffix << std::endl << prefix << " -> ";
            } else {
                output << suffix << std::endl << prefix << "    ";
            }
            option->to_stream(output, time, i, "    " + prefix, suffix);
        }
        return output;
    }

    /*!
     * \brief Returns a yaml representation of the arbitrator object with its current state
     *
     * \param time  Expected execution time point of this behaviors command
     * \return      Yaml representation of this behavior
     */
    virtual YAML::Node toYaml(const Time& time) const override {
        YAML::Node node = Behavior<CommandT>::toYaml(time);

        node["type"] = "Arbitrator";
        for (const typename Option::Ptr& option : behaviorOptions_) {
            YAML::Node yaml = option->toYaml(time);
            node["options"].push_back(yaml);
        }
        if (activeBehavior_) {
            node["activeBehavior"] = getOptionIndex(activeBehavior_);
        }

        return node;
    }


protected:
    /*!
     * Override this function in a specialized Arbitrator in order to
     * find a behavior option that is the best option according to your policy and has true invocation condition
     *
     * @return  Best applicable option according to your policy (can also be the currently active option)
     */
    virtual typename Option::Ptr findBestOption(const Time& time) const = 0;

    std::size_t getOptionIndex(const typename Option::ConstPtr& behaviorOption) const {
        const auto it = std::find(behaviorOptions_.begin(), behaviorOptions_.end(), behaviorOption);

        if (it != behaviorOptions_.end()) {
            return std::distance(behaviorOptions_.begin(), it);
        }
        throw InvalidArgumentsError(
            "Invalid call of getOptionIndex(): Given option not found in list of behavior options!");
    }

    std::vector<typename Option::Ptr> behaviorOptions_;
    typename Option::Ptr activeBehavior_;
};
} // namespace behavior_planning
