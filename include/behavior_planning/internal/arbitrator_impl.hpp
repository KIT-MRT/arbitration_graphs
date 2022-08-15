#pragma once

#include "../arbitrator.hpp"


namespace behavior_planning {

template <typename CommandT, typename SubCommandT, typename VerifierT, typename VerificationResultT>
typename Arbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>::Options Arbitrator<
    CommandT,
    SubCommandT,
    VerifierT,
    VerificationResultT>::applicableOptions(const Time& time) const {

    Options options;
    std::copy_if(behaviorOptions_.begin(),
                 behaviorOptions_.end(),
                 std::back_inserter(options),
                 std::bind(&Arbitrator::isApplicable, this, std::placeholders::_1, time));
    return options;
};

template <typename CommandT, typename SubCommandT, typename VerifierT, typename VerificationResultT>
bool Arbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>::isActive(
    const typename Option::Ptr& option) const {
    return activeBehavior_ && option == activeBehavior_;
}

template <typename CommandT, typename SubCommandT, typename VerifierT, typename VerificationResultT>
bool Arbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>::isApplicable(const typename Option::Ptr& option,
                                                                                     const Time& time) const {
    const bool isActiveAndCanBeContinued = isActive(option) && option->behavior_->checkCommitmentCondition(time);
    return isActiveAndCanBeContinued || option->behavior_->checkInvocationCondition(time);
}

template <typename CommandT, typename SubCommandT, typename VerifierT, typename VerificationResultT>
std::size_t Arbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>::getOptionIndex(
    const typename Option::ConstPtr& behaviorOption) const {
    const auto it = std::find(behaviorOptions_.begin(), behaviorOptions_.end(), behaviorOption);

    if (it != behaviorOptions_.end()) {
        return std::distance(behaviorOptions_.begin(), it);
    }
    throw InvalidArgumentsError(
        "Invalid call of getOptionIndex(): Given option not found in list of behavior options!");
}

template <typename CommandT, typename SubCommandT, typename VerifierT, typename VerificationResultT>
std::optional<SubCommandT> Arbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>::getAndVerifyCommand(
    const typename Option::Ptr& option, const Time& time) const {
    try {
        const SubCommandT command = option->behavior_->getCommand(time);
        const VerificationResultT verificationResult = verifier_.analyze(time, command);
        option->verificationResult_.cache(time, verificationResult);
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

template <typename CommandT, typename SubCommandT, typename VerifierT, typename VerificationResultT>
std::optional<SubCommandT> Arbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>::
    getAndVerifyCommandFromActive(const Time& time) {
    bool activeBehaviorCanBeContinued = activeBehavior_ && activeBehavior_->behavior_->checkCommitmentCondition(time);

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

template <typename CommandT, typename SubCommandT, typename VerifierT, typename VerificationResultT>
SubCommandT Arbitrator<CommandT, SubCommandT, VerifierT, VerificationResultT>::getAndVerifyCommandFromApplicable(
    const Options& options, const Time& time) {
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

} // namespace behavior_planning