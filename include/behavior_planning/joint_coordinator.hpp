#pragma once

#include <memory>
#include <optional>

#include <yaml-cpp/yaml.h>

#include "arbitrator.hpp"
#include "exceptions.hpp"


namespace behavior_planning {

/*!
 * \brief The JointCoordinator class combines all sub-commands by conjunction, using the operator&()
 *  Difference to ConjunctiveCoordinator is that it is invocated if any of the sub-behaviors has true
 *  invocationCondition
 */
template <typename CommandT,
          typename SubCommandT,
          typename VerifierT = verification::PlaceboVerifier<SubCommandT>,
          typename VerificationResultT = typename decltype(std::function{VerifierT::analyze})::result_type>
class JointCoordinator : public Arbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT> {
public:
    using ArbitratorBase = Arbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>;

    using Ptr = std::shared_ptr<JointCoordinator>;
    using ConstPtr = std::shared_ptr<const JointCoordinator>;

    struct Option : public ArbitratorBase::Option {
    public:
        using Ptr = std::shared_ptr<Option>;
        using FlagsT = typename ArbitratorBase::Option::FlagsT;
        using ConstPtr = std::shared_ptr<const Option>;

        enum Flags { NO_FLAGS = 0b0 };

        Option(const typename Behavior<SubCommandT>::Ptr& behavior, const FlagsT& flags)
                : ArbitratorBase::Option(behavior, flags) {
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
            output << "- ";
            ArbitratorBase::Option::to_stream(output, time, option_index, prefix, suffix);
            return output;
        }
    };

    explicit JointCoordinator(const std::string& name = "JointCoordinator", const VerifierT& verifier = VerifierT())
            : ArbitratorBase(name, verifier) {
    }

    void addOption(const typename Behavior<SubCommandT>::Ptr& behavior, const typename Option::Flags& flags) {
        for (auto option : this->behaviorOptions_) {
            if (option->behavior_ == behavior) {
                throw MultipleReferencesToSameInstanceError(
                    "Coordinators cannot hold the same behavior instantiation multiple times!");
            }
        }

        typename Option::Ptr option = std::make_shared<Option>(behavior, flags);
        this->behaviorOptions_.push_back(option);
    }

    // Combine all the subcommands if any of the sub-behaviors is invocated or is active right now and can be committed
    CommandT getCommand(const Time& time) override {
        SubCommandT subcommand_conjunction;
        bool isAnyApplicableOptionSafe = false;

        for (int i = 0; i < (int)this->behaviorOptions_.size(); ++i) {
            typename Option::Ptr option = std::dynamic_pointer_cast<Option>(this->behaviorOptions_.at(i));

            // Not exacly what isActiveAndCanBeContinued represents in other arbitrators:
            // Normally, isActiveAndCanBeContinued says that the _one_ active behavior option has true commitment
            // condition.
            // Here, isActiveAndCanBeContinued says that the JointCoordinator is active (meaning many of its
            // options are active) and this particular option has true commitment condition (though itself might yet not
            // be active).
            // In order to be consistent with the normal interpretation of isActiveAndCanBeContinued, we would need to
            // track which of the options are currently active and which not.
            bool isActiveAndCanBeContinued = isActive_ && option->behavior_->checkCommitmentCondition(time);

            if (option->behavior_->checkInvocationCondition(time) || isActiveAndCanBeContinued) {
                const std::optional<SubCommandT> command = this->getAndVerifyCommand(option, time);
                if (command) {
                    isAnyApplicableOptionSafe = true;
                    subcommand_conjunction &= command.value();
                }
            }
        }

        if (!isAnyApplicableOptionSafe) {
            throw NoApplicableOptionPassedVerificationError("None of the " +
                                                            std::to_string(this->behaviorOptions_.size()) +
                                                            " applicable options passed the verification step!");
        }

        return CommandT(subcommand_conjunction);
    }

    bool checkInvocationCondition(const Time& time) const override {
        if (this->behaviorOptions_.size() == 0) {
            return false;
        }
        for (auto& option : this->behaviorOptions_) {
            if (option->behavior_->checkInvocationCondition(time)) {
                return true;
            }
        }
        return false;
    }
    bool checkCommitmentCondition(const Time& time) const override {
        if (!isActive_) {
            return false;
        }
        for (auto& option : this->behaviorOptions_) {
            if (option->behavior_->checkCommitmentCondition(time)) {
                return true;
            }
        }
        return false;
    }

    void gainControl(const Time& time) override {
        for (auto& option : this->behaviorOptions_) {
            option->behavior_->gainControl(time);
        }
        isActive_ = true;
    }

    void loseControl(const Time& time) override {
        isActive_ = false;
        for (auto& option : this->behaviorOptions_) {
            option->behavior_->loseControl(time);
        }
    }

    /*!
     * \brief Writes a string representation of the Coordinator object with its current state to the output stream.
     *
     * \param output    Output stream to write into, will be returned also
     * \param time      Expected execution time point of this behaviors command
     * \param prefix    A string that should be prepended to each line that is written to the output stream
     * \param suffix    A string that should be appended to each line that is written to the output stream
     * \return          The same given input stream (signature similar to std::ostream& operator<<())
     *
     * \see Behavior::to_stream()
     */
    std::ostream& to_stream(std::ostream& output,
                            const Time& time,
                            const std::string& prefix = "",
                            const std::string& suffix = "") const override {
        Behavior<CommandT>::to_stream(output, time, prefix, suffix);

        for (int i = 0; i < (int)this->behaviorOptions_.size(); ++i) {
            typename Option::Ptr option = std::dynamic_pointer_cast<Option>(this->behaviorOptions_.at(i));

            /// \todo Put arrow only in front of active options, not to all if JointCoordinator is active
            if (isActive_) {
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
    YAML::Node toYaml(const Time& time) const override {
        YAML::Node node = ArbitratorBase::toYaml(time);
        node["type"] = "JointCoordinator";
        return node;
    }

protected:
    typename ArbitratorBase::Options sortOptionsByGivenPolicy(const typename ArbitratorBase::Options& options,
                                                              const Time& time) const override {
        return {};
    }

    bool isActive_{false};
};
} // namespace behavior_planning
