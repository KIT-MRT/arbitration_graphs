#pragma once

#include <algorithm>
#include <functional>
#include <iterator>
#include <memory>
#include <optional>
#include <vector>

#include <yaml-cpp/yaml.h>

#include <util_caching/cache.hpp>

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
        mutable util_caching::Cache<Time, VerificationResultT> verificationResult_;

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
                                        const std::string& suffix = "") const;

        /*!
         * \brief Returns a yaml representation of this option with its current state
         *
         * \param time  Expected execution time point of this behaviors command
         * \return      Yaml representation of this behavior
         */
        virtual YAML::Node toYaml(const Time& time) const;
    };
    using Options = std::vector<typename Option::Ptr>;
    using ConstOptions = std::vector<typename Option::ConstPtr>;


    Arbitrator(const std::string& name = "Arbitrator", const VerifierT& verifier = VerifierT())
            : Behavior<CommandT>(name), verifier_{verifier} {};


    virtual void addOption(const typename Behavior<SubCommandT>::Ptr& behavior, const typename Option::FlagsT& flags) {
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
                                    const std::string& suffix = "") const override;

    /*!
     * \brief Returns a yaml representation of the arbitrator object with its current state
     *
     * \param time  Expected execution time point of this behaviors command
     * \return      Yaml representation of this behavior
     */
    virtual YAML::Node toYaml(const Time& time) const override;


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
    Options applicableOptions(const Time& time) const;

    bool isActive(const typename Option::Ptr& option) const;
    bool isApplicable(const typename Option::Ptr& option, const Time& time) const;

    std::size_t getOptionIndex(const typename Option::ConstPtr& behaviorOption) const;

    /*!
     * @brief Call getCommand on the given option and verify its returned command
     *
     * @param option    Behavior option to call and verify
     * @param time      Expected execution time point of this behaviors command
     * @return Command of the given option, if it passed verification, otherwise nullopt
     */
    std::optional<SubCommandT> getAndVerifyCommand(const typename Option::Ptr& option, const Time& time) const;

    /*!
     * @brief Get and verify the command from the active behavior, if there is an active one
     *
     * @param time  Expected execution time point of this behaviors command
     * @return Command of the active option, if it exists, can be continued and it passed verification,
     *         otherwise nullopt
     */
    std::optional<SubCommandT> getAndVerifyCommandFromActive(const Time& time);

    /*!
     * @brief Get and verify the command from the best option that passes verification
     *
     * @param options   Applicable behavior options, sorted by custom policy (descending: first is best)
     * @param time      Expected execution time point of this behaviors command
     * @return Command of best option passing verification, throws if none passes
     */
    SubCommandT getAndVerifyCommandFromApplicable(const Options& options, const Time& time);

    Options behaviorOptions_;
    typename Option::Ptr activeBehavior_;

    VerifierT verifier_;
};
} // namespace behavior_planning

#include "internal/arbitrator_impl.hpp"
#include "internal/arbitrator_io.hpp"