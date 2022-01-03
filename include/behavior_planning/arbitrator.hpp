#pragma once

#include <algorithm>
#include <functional>
#include <iterator>
#include <memory>
#include <optional>
#include <vector>

#include <yaml-cpp/yaml.h>

#include "behavior.hpp"
#include "exceptions.hpp"
#include "verification.hpp"


namespace behavior_planning {


/*!
 * \brief The Arbitrator class
 *
 * \note If CommandT != SubCommandT either
 *       - override getCommand() in your specialized Arbitrator or
 *       - provide a CommandT(const SubCommandT&) constructor
 *
 * \note As long as VerifierT::analyze() is static the VerificationResultT type can be deduced by the compiler,
 *       otherwise you have to pass it as template argument
 */
template <typename CommandT,
          typename SubCommandT = CommandT,
          typename VerifierT = verification::PlaceboVerifier<SubCommandT>,
          typename VerificationResultT = typename decltype(std::function{VerifierT::analyze})::result_type>
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
        mutable std::optional<VerificationResultT> verificationResult_;

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
            if (verificationResult_ && !verificationResult_->isOk()) {
                // ANSI backspace: \010
                // ANSI strikethrough on: \033[9m
                output << "×××\010\010\010\033[9m";
                behavior_->to_stream(output, time, prefix, suffix);
                // ANSI strikethrough off: \033[29m
                // ANSI hide on: \033[8m
                // ANSI hide off: \033[28m
                output << "\033[29m\033[8m×××\033[28m";
            } else {
                behavior_->to_stream(output, time, prefix, suffix);
            }

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
    using Options = std::vector<typename Option::Ptr>;
    using ConstOptions = std::vector<typename Option::ConstPtr>;


    Arbitrator(const std::string& name = "Arbitrator", const VerifierT& verifier = VerifierT())
            : Behavior<CommandT>(name), verifier_{verifier} {};


    virtual void addOption(const typename Behavior<SubCommandT>::Ptr& behavior, const typename Option::Flags& flags) {
        typename Option::Ptr option = std::make_shared<Option>(behavior, flags);
        this->behaviorOptions_.push_back(option);
    }

    CommandT getCommand(const Time& time) override {
        // first try to continue an active option, if one exists
        std::optional<SubCommandT> command = getAndVerifyCommandFromActive(time);

        if (command) {
            return command.value();
        }

        // otherwise take all options equally into account, including the active option (if it exists)
        const auto applicableOptions = this->applicableOptions(time);

        if (!applicableOptions.empty()) {
            const auto bestApplicableOptions = sortOptionsByGivenPolicy(applicableOptions, time);
            return getAndVerifyCommandFromApplicable(bestApplicableOptions, time);
        }

        throw InvocationConditionIsFalseError(
            "No behavior with true invocation condition found! Only call getCommand() if "
            "checkInvocationCondition() or checkCommitmentCondition() is true!");
    }

    ConstOptions options() const {
        return ConstOptions(behaviorOptions_.begin(), behaviorOptions_.end());
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
     * @brief   Override this function in a specialized Arbitrator in order to
     *          sort given behavior options according to your policy in descending order (first is best)
     *
     * @param options  Applicable behavior options in the order given in behaviorOptions_
     * @param time  Expected execution time point of this behaviors command
     * @return  Behavior options sorted according to your policy
     */
    virtual Options sortOptionsByGivenPolicy(const Options& options, const Time& time) const = 0;

    /*!
     * @brief   Returns all behavior options with true invocation condition or
     *          true commitment condition for the active option
     *
     * @param time  Expected execution time point of this behaviors command
     * @return  Vector of applicable behavior options
     */
    Options applicableOptions(const Time& time) const {
        using namespace std::placeholders;

        Options options;
        std::copy_if(behaviorOptions_.begin(),
                     behaviorOptions_.end(),
                     std::back_inserter(options),
                     std::bind(&Arbitrator::isApplicable, this, _1, time));
        return options;
    };

    bool isActive(const typename Option::Ptr& option) const {
        return activeBehavior_ && option == activeBehavior_;
    }
    bool isApplicable(const typename Option::Ptr& option, const Time& time) const {
        const bool isActiveAndCanBeContinued = isActive(option) && option->behavior_->checkCommitmentCondition(time);
        return isActiveAndCanBeContinued || option->behavior_->checkInvocationCondition(time);
    }

    std::size_t getOptionIndex(const typename Option::ConstPtr& behaviorOption) const {
        const auto it = std::find(behaviorOptions_.begin(), behaviorOptions_.end(), behaviorOption);

        if (it != behaviorOptions_.end()) {
            return std::distance(behaviorOptions_.begin(), it);
        }
        throw InvalidArgumentsError(
            "Invalid call of getOptionIndex(): Given option not found in list of behavior options!");
    }

    /*!
     * @brief Call getCommand on the given option and verify its returned command
     *
     * @param option    Behavior option to call and verify
     * @param time      Expected execution time point of this behaviors command
     * @return Command of the given option, if it passed verification, otherwise nullopt
     */
    std::optional<SubCommandT> getAndVerifyCommand(const typename Option::Ptr& option, const Time& time) const {
        try {
            const SubCommandT command = option->behavior_->getCommand(time);
            const VerificationResultT verificationResult = verifier_.analyze(time, command);
            option->verificationResult_ = verificationResult;
            if (verificationResult.isOk()) {
                return command;
            }
            // given option is applicable, but not safe
            /// \todo log this somewhere?
        } catch (VerificationError& e) {
            // given option is arbitrator without safe applicable option
            option->verificationResult_.reset();

            /// \todo log this somewhere?
        }
        return std::nullopt;
    }

    /*!
     * @brief Get and verify the command from the active behavior, if there is an active one
     *
     * @param time  Expected execution time point of this behaviors command
     * @return Command of the active option, if it exists, can be continued and it passed verification,
     *         otherwise nullopt
     */
    std::optional<SubCommandT> getAndVerifyCommandFromActive(const Time& time) {
        bool activeBehaviorCanBeContinued =
            activeBehavior_ && activeBehavior_->behavior_->checkCommitmentCondition(time);

        if (activeBehavior_ && !activeBehaviorCanBeContinued) {
            activeBehavior_->behavior_->loseControl(time);
            activeBehavior_.reset();
        }

        bool activeBehaviorInterruptable = activeBehavior_ && (activeBehavior_->hasFlag(Option::Flags::INTERRUPTABLE));

        // continue with active behavior, if one exists, it is committed, not interruptable and passes verification
        if (activeBehaviorCanBeContinued && !activeBehaviorInterruptable) {
            const std::optional<SubCommandT> command = getAndVerifyCommand(activeBehavior_, time);
            if (command) {
                return command.value();
            }

            activeBehavior_->behavior_->loseControl(time);
            activeBehavior_ = nullptr;
        }

        return std::nullopt;
    }

    /*!
     * @brief Get and verify the command from the best option that passes verification
     *
     * @param options   Applicable behavior options, sorted by custom policy (descending: first is best)
     * @param time      Expected execution time point of this behaviors command
     * @return Command of best option passing verification, throws if none passes
     */
    SubCommandT getAndVerifyCommandFromApplicable(const Options& options, const Time& time) {
        for (const auto& bestOption : options) {
            if (!activeBehavior_ || bestOption != activeBehavior_) {
                // we allow bestOption and activeBehavior_ to gain control simultaneuosly until we figure out
                // if bestOption passes verification
                bestOption->behavior_->gainControl(time);
            }
            // otherwise we have bestOption == activeBehavior_ which already gained control

            // an arbitrator as option might not return a command, if its applicable options fail verification:
            const std::optional<SubCommandT> command = getAndVerifyCommand(bestOption, time);

            if (command) {
                if (activeBehavior_ && bestOption != activeBehavior_) {
                    // finally, prevent two behaviors from having control
                    activeBehavior_->behavior_->loseControl(time);
                }
                activeBehavior_ = bestOption;
                return command.value();
            }
            bestOption->behavior_->loseControl(time);
        }

        throw NoApplicableOptionPassedVerificationError("None of the " + std::to_string(options.size()) +
                                                        " applicable options passed the verification step!");
    }

    Options behaviorOptions_;
    typename Option::Ptr activeBehavior_;

    VerifierT verifier_;
};
} // namespace behavior_planning
