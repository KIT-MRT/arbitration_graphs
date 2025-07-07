#pragma once

#include <memory>
#include <optional>
#include <vector>

#include <util_caching/cache.hpp>
#include <yaml-cpp/yaml.h>

#include "behavior.hpp"
#include "exceptions.hpp"
#include "verification.hpp"


namespace arbitration_graphs {


/*!
 * \brief The Arbitrator class
 *
 * \note If CommandT != SubCommandT either
 *       - override getCommand() in your specialized Arbitrator or
 *       - provide a CommandT(const SubCommandT&) constructor
 */
template <typename EnvironmentModelT, typename CommandT, typename SubCommandT = CommandT>
class Arbitrator : public Behavior<EnvironmentModelT, CommandT> {
public:
    using Ptr = std::shared_ptr<Arbitrator>;
    using ConstPtr = std::shared_ptr<const Arbitrator>;

    using PlaceboVerifierT = verification::PlaceboVerifier<EnvironmentModelT, SubCommandT>;
    using VerifierT = verification::Verifier<EnvironmentModelT, SubCommandT>;

    /*!
     * \brief The Option struct holds a behavior option of the arbitrator and corresponding flags
     *
     * This is a subclass of arbitrator, as some of the Flags could be arbitrator specific.
     * For each arbitration class we have to make sure though, to derive an Option class from this Option base.
     *
     * \note When using addOption(), make sure to use the Flags of the correct Arbitrator.
     *       The compiler wouldn't notice a confusion unfortunately.
     */
    class Option {
    public:
        using Ptr = std::shared_ptr<Option>;
        using ConstPtr = std::shared_ptr<const Option>;

        enum Flags { NoFlags = 0b0, Interruptable = 0b1, Fallback = 0b10 };
        using FlagsT = std::underlying_type_t<Flags>;

        using BehaviorT = Behavior<EnvironmentModelT, SubCommandT>;

        Option(const typename BehaviorT::Ptr& behavior, const FlagsT& flags) : behavior_{behavior}, flags_{flags} {
        }
        virtual ~Option() = default;

        SubCommandT getCommand(const Time& time, const EnvironmentModelT& environmentModel) const {
            if (!command_.cached(time)) {
                command_.cache(time, behavior_->getCommand(time, environmentModel));
            }
            return command_.cached(time).value();
        }

        const typename BehaviorT::Ptr& behavior() const {
            return behavior_;
        }

        std::optional<verification::AbstractResult::ConstPtr> verificationResult(const Time& time) const {
            return verificationResult_.cached(time);
        }
        void cacheVerificationResult(const Time& time, const verification::AbstractResult::ConstPtr& result) const {
            verificationResult_.cache(time, result);
        }
        void resetVerificationResult() const {
            verificationResult_.reset();
        }

        bool hasFlag(const FlagsT& flagToCheck) const {
            return flags_ & flagToCheck;
        }

        /*!
         * \brief Writes a string representation of the behavior option and its current state to the output stream.
         *
         * \param output            Output stream to write into, will be returned also
         * \param time              Expected execution time point of this behaviors command
         * \param environmentModel  A read-only object containing the current state of the environment
         * \param optionIndex       Position index of this option within behaviorOptions_
         * \param prefix            A string that should be prepended to each line that is written to the output stream
         * \param suffix            A string that should be appended to each line that is written to the output stream
         * \return                  The same given input stream (signature similar to std::ostream& operator<<())
         *
         * \see Arbitrator::to_stream()
         */
        virtual std::ostream& to_stream(std::ostream& output,
                                        const Time& time,
                                        const EnvironmentModelT& environmentModel,
                                        const int& optionIndex,
                                        const std::string& prefix = "",
                                        const std::string& suffix = "") const;

        /*!
         * \brief Returns a yaml representation of this option with its current state
         *
         * \param time  Expected execution time point of this behaviors command
         * \return      Yaml representation of this behavior
         */
        virtual YAML::Node toYaml(const Time& time, const EnvironmentModelT& environmentModel) const;

    private:
        typename Behavior<EnvironmentModelT, SubCommandT>::Ptr behavior_;
        FlagsT flags_;
        mutable util_caching::Cache<Time, SubCommandT> command_;
        mutable util_caching::Cache<Time, verification::Result::ConstPtr> verificationResult_;
    };
    using Options = std::vector<typename Option::Ptr>;
    using ConstOptions = std::vector<typename Option::ConstPtr>;


    explicit Arbitrator(const std::string& name = "Arbitrator",
                        typename VerifierT::Ptr verifier = std::make_shared<PlaceboVerifierT>())
            : Behavior<EnvironmentModelT, CommandT>(name), verifier_(verifier) {};


    virtual void addOption(const typename Behavior<EnvironmentModelT, SubCommandT>::Ptr& behavior,
                           const typename Option::FlagsT& flags) {
        typename Option::Ptr option = std::make_shared<Option>(behavior, flags);
        this->behaviorOptions_.push_back(option);
    }

    CommandT getCommand(const Time& time, const EnvironmentModelT& environmentModel) override {
        // first try to continue an active option, if one exists
        std::optional<SubCommandT> command = getAndVerifyCommandFromActive(time, environmentModel);

        if (command) {
            return command.value();
        }

        // otherwise take all options equally into account, including the active option (if it exists)
        const auto applicableOptions = this->applicableOptions(time, environmentModel);

        if (!applicableOptions.empty()) {
            const auto bestApplicableOptions = sortOptionsByGivenPolicy(applicableOptions, time, environmentModel);
            return getAndVerifyCommandFromApplicable(bestApplicableOptions, time, environmentModel);
        }

        throw InvocationConditionIsFalseError(
            "No behavior with true invocation condition found! Only call getCommand() if "
            "checkInvocationCondition() or checkCommitmentCondition() is true!");
    }

    ConstOptions options() const {
        return ConstOptions(behaviorOptions_.begin(), behaviorOptions_.end());
    }

    bool checkInvocationCondition(const Time& time, const EnvironmentModelT& environmentModel) const override {
        for (auto& option : behaviorOptions_) {
            if (option->behavior()->checkInvocationCondition(time, environmentModel)) {
                return true;
            }
        }
        return false;
    }
    bool checkCommitmentCondition(const Time& time, const EnvironmentModelT& environmentModel) const override {
        if (activeBehavior_) {
            if (activeBehavior_->behavior()->checkCommitmentCondition(time, environmentModel)) {
                return true;
            }
            return checkInvocationCondition(time, environmentModel);
        }
        return false;
    }

    void gainControl(const Time& time, const EnvironmentModelT& environmentModel) override {
    }

    void loseControl(const Time& time, const EnvironmentModelT& environmentModel) override {
        if (activeBehavior_) {
            activeBehavior_->behavior()->loseControl(time, environmentModel);
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
     * \param environmentModel  A read-only object containing the current state of the environment
     * \param prefix    A string that should be prepended to each line that is written to the output stream
     * \param suffix    A string that should be appended to each line that is written to the output stream
     * \return          The same given input stream (signature similar to std::ostream& operator<<())
     *
     * \see Behavior::to_stream()
     */
    std::ostream& to_stream(std::ostream& output,
                            const Time& time,
                            const EnvironmentModelT& environmentModel,
                            const std::string& prefix = "",
                            const std::string& suffix = "") const override;

    /*!
     * \brief Returns a yaml representation of the arbitrator object with its current state
     *
     * \param time  Expected execution time point of this behaviors command
     * \param environmentModel  A read-only object containing the current state of the environment
     * \return      Yaml representation of this behavior
     */
    YAML::Node toYaml(const Time& time, const EnvironmentModelT& environmentModel) const override;


protected:
    /*!
     * @brief   Override this function in a specialized Arbitrator in order to
     *          sort given behavior options according to your policy in descending order (first is best)
     *
     * @param options  Applicable behavior options in the order given in behaviorOptions_
     * @param time  Expected execution time point of this behaviors command
     * @return  Behavior options sorted according to your policy
     */
    virtual Options sortOptionsByGivenPolicy(const Options& options,
                                             const Time& time,
                                             const EnvironmentModelT& environmentModel) const = 0;

    /*!
     * @brief   Returns all behavior options with true invocation condition or
     *          true commitment condition for the active option
     *
     * @param time  Expected execution time point of this behaviors command
     * @return  Vector of applicable behavior options
     */
    Options applicableOptions(const Time& time, const EnvironmentModelT& environmentModel) const;

    bool isActive(const typename Option::Ptr& option) const;
    bool isApplicable(const typename Option::Ptr& option,
                      const Time& time,
                      const EnvironmentModelT& environmentModel) const;

    std::size_t getOptionIndex(const typename Option::ConstPtr& behaviorOption) const;

    /*!
     * @brief Call getCommand on the given option and verify its returned command
     *
     * @param option    Behavior option to call and verify
     * @param time      Expected execution time point of this behaviors command
     * @param environmentModel  A read-only object containing the current state of the environment
     * @return Command of the given option, if it passed verification, otherwise nullopt
     */
    std::optional<SubCommandT> getAndVerifyCommand(const typename Option::Ptr& option,
                                                   const Time& time,
                                                   const EnvironmentModelT& environmentModel) const;

    /*!
     * @brief Get and verify the command from the active behavior, if there is an active one
     *
     * @param time  Expected execution time point of this behaviors command
     * @param environmentModel  A read-only object containing the current state of the environment
     * @return Command of the active option, if it exists, can be continued and it passed verification,
     *         otherwise nullopt
     */
    std::optional<SubCommandT> getAndVerifyCommandFromActive(const Time& time,
                                                             const EnvironmentModelT& environmentModel);

    /*!
     * @brief Get and verify the command from the best option that passes verification
     *
     * @param options   Applicable behavior options, sorted by custom policy (descending: first is best)
     * @param time      Expected execution time point of this behaviors command
     * @param environmentModel  A read-only object containing the current state of the environment
     * @return Command of best option passing verification, throws if none passes
     */
    SubCommandT getAndVerifyCommandFromApplicable(const Options& options,
                                                  const Time& time,
                                                  const EnvironmentModelT& environmentModel);

    Options behaviorOptions_;
    typename Option::Ptr activeBehavior_;

    typename VerifierT::Ptr verifier_;
};
} // namespace arbitration_graphs

#include "internal/arbitrator_impl.hpp" // IWYU pragma: keep
#include "internal/arbitrator_io.hpp"   // IWYU pragma: keep
