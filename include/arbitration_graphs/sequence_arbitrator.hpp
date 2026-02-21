#pragma once

#include <memory>

#include <yaml-cpp/yaml.h>

#include "arbitrator.hpp"
#include "exceptions.hpp"


namespace arbitration_graphs {

/*!
 * \brief A sequence arbitrator that executes its sub-behaviors in a fixed order.
 *
 * Based on the definition by Lauer et al. (2010):
 * - Invocation condition: The invocation condition of the first sub-behavior is true.
 * - Commitment condition: The sequence has started and has not yet completed.
 * - Algorithm: Executes sub-behaviors in the order they were added. Advances to the next
 *              sub-behavior once the current sub-behavior's commitment condition becomes false.
 *              The sequence is complete when the last sub-behavior's commitment condition
 *              becomes false.
 *
 * \see Martin Lauer, Roland Hafner, Sascha Lange, and Martin Riedmiller,
 *      "Cognitive concepts in autonomous soccer playing robots,"
 *      Cognitive Systems Research, vol. 11, no. 3, pp. 287–309, 2010,
 *      doi: https://doi.org/10.1016/j.cogsys.2009.12.003
 */
template <typename EnvironmentModelT, typename CommandT, typename SubCommandT = CommandT>
class SequenceArbitrator : public Arbitrator<EnvironmentModelT, CommandT, SubCommandT> {
public:
    using ArbitratorBase = Arbitrator<EnvironmentModelT, CommandT, SubCommandT>;

    using Ptr = std::shared_ptr<SequenceArbitrator>;
    using ConstPtr = std::shared_ptr<const SequenceArbitrator>;

    using PlaceboVerifierT = verification::PlaceboVerifier<EnvironmentModelT, SubCommandT>;
    using VerifierT = verification::Verifier<EnvironmentModelT, SubCommandT>;

    class Option : public ArbitratorBase::Option {
    public:
        using Ptr = std::shared_ptr<Option>;
        using FlagsT = typename ArbitratorBase::Option::FlagsT;
        using ConstPtr = std::shared_ptr<const Option>;

        enum Flags { NoFlags = 0b0 };

        Option(const typename Behavior<EnvironmentModelT, SubCommandT>::Ptr& behavior, const FlagsT& flags)
                : ArbitratorBase::Option(behavior, flags) {
        }

        /*!
         * \brief Writes a string representation of the behavior option and its current state to the output stream.
         *
         * \param output            Output stream to write into, will be returned also
         * \param time              Expected execution time point of this behaviors command
         * \param environmentModel  A read-only object containing the current state of the environment
         * \param optionIndex       Position index of this option within options()
         * \param prefix            A string that should be prepended to each line that is written to the output stream
         * \param suffix            A string that should be appended to each line that is written to the output stream
         * \return                  The same given input stream (signature similar to std::ostream& operator<<())
         *
         * \see Arbitrator::toStream()
         */
        std::ostream& toStream(std::ostream& output,
                               const Time& time,
                               const EnvironmentModelT& environmentModel,
                               const int& optionIndex,
                               const std::string& prefix = "",
                               const std::string& suffix = "") const override;
    };

    explicit SequenceArbitrator(const std::string& name = "SequenceArbitrator",
                                typename VerifierT::Ptr verifier = std::make_shared<PlaceboVerifierT>())
            : ArbitratorBase(name, verifier) {};

    void addOption(const typename Behavior<EnvironmentModelT, SubCommandT>::Ptr& behavior,
                   const typename Option::FlagsT& flags) override {
        typename Option::Ptr option = std::make_shared<Option>(behavior, flags);
        this->addOptionImpl(option);
        sequenceOptions_.push_back(option);
    }

    /*!
     * \brief The invocation condition is true if the first sub-behavior's invocation condition is true.
     */
    bool checkInvocationCondition(const Time& time, const EnvironmentModelT& environmentModel) const override {
        if (sequenceOptions_.empty()) {
            return false;
        }
        return sequenceOptions_.front()->behavior()->checkInvocationCondition(time, environmentModel);
    }

    /*!
     * \brief The commitment condition is true as long as the sequence has started and has not yet completed.
     *
     * The sequence is committed if the current sub-behavior's commitment condition is true,
     * or if there are more sub-behaviors to execute after the current one.
     */
    bool checkCommitmentCondition(const Time& time, const EnvironmentModelT& environmentModel) const override {
        if (!sequenceStarted_ || sequenceOptions_.empty()) {
            return false;
        }
        return sequenceOptions_.at(currentIndex_)->behavior()->checkCommitmentCondition(time, environmentModel) ||
               currentIndex_ + 1 < static_cast<int>(sequenceOptions_.size());
    }

    /*!
     * \brief Starts execution of the sequence from the first sub-behavior.
     */
    void gainControl(const Time& time, const EnvironmentModelT& environmentModel) override {
        currentIndex_ = 0;
        sequenceStarted_ = true;
        if (!sequenceOptions_.empty()) {
            sequenceOptions_.front()->behavior()->gainControl(time, environmentModel);
        }
    }

    /*!
     * \brief Stops execution of the sequence and cleans up the current sub-behavior.
     */
    void loseControl(const Time& time, const EnvironmentModelT& environmentModel) override {
        if (sequenceStarted_ && !sequenceOptions_.empty() &&
            currentIndex_ < static_cast<int>(sequenceOptions_.size())) {
            sequenceOptions_.at(currentIndex_)->behavior()->loseControl(time, environmentModel);
        }
        currentIndex_ = 0;
        sequenceStarted_ = false;
    }

    /*!
     * \brief Executes sub-behaviors in sequence, advancing when the current one is done.
     *
     * Advances to the next sub-behavior when the current sub-behavior's commitment condition
     * becomes false. Returns the command of the current (active) sub-behavior.
     */
    CommandT getCommand(const Time& time, const EnvironmentModelT& environmentModel) override {
        if (!sequenceStarted_ || sequenceOptions_.empty()) {
            throw InvocationConditionIsFalseError(
                "SequenceArbitrator::getCommand() called without prior gainControl() or with no options!");
        }

        // Advance past sub-behaviors whose commitment condition has become false
        while (currentIndex_ + 1 < static_cast<int>(sequenceOptions_.size()) &&
               !sequenceOptions_.at(currentIndex_)->behavior()->checkCommitmentCondition(time, environmentModel)) {
            sequenceOptions_.at(currentIndex_)->behavior()->loseControl(time, environmentModel);
            currentIndex_++;
            sequenceOptions_.at(currentIndex_)->behavior()->gainControl(time, environmentModel);
        }

        // Get and verify command from the current sub-behavior
        typename ArbitratorBase::Option::Ptr currentOption = sequenceOptions_.at(currentIndex_);
        std::optional<SubCommandT> command = this->getAndVerifyCommand(currentOption, time, environmentModel);
        if (command) {
            return command.value();
        }

        throw NoApplicableOptionPassedVerificationError(
            "Current sub-behavior in sequence failed verification!");
    }

    bool isActive() const override {
        return sequenceStarted_;
    }

    /*!
     * \brief Writes a string representation of the SequenceArbitrator with its current state to the output stream.
     */
    std::ostream& toStream(std::ostream& output,
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
     * \brief Not used by SequenceArbitrator (getCommand is fully overridden), but required as pure virtual.
     */
    typename ArbitratorBase::Options sortOptionsByGivenPolicy(
        const typename ArbitratorBase::Options& options,
        const Time& /*time*/,
        const EnvironmentModelT& /*environmentModel*/) const override {
        return options;
    }

private:
    std::vector<typename Option::Ptr> sequenceOptions_;
    int currentIndex_{0};
    bool sequenceStarted_{false};
};

} // namespace arbitration_graphs

#include "internal/sequence_arbitrator_io.hpp" // IWYU pragma: keep
