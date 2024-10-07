#pragma once

#include "../arbitrator.hpp"

#include <glog/logging.h>


namespace arbitration_graphs {

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
        const SubCommandT command = option->getCommand(time);

        const VerificationResultT verificationResult = verifier_.analyze(time, command);
        option->verificationResult_.cache(time, verificationResult);

        // options explicitly flagged as fallback do not need to pass verification
        if (verificationResult.isOk() || option->hasFlag(Option::Flags::FALLBACK)) {
            return command;
        }
        // given option is applicable, but not safe
        VLOG(1) << "Given option " << option->behavior_->name_ << " is applicable, but not safe";
        VLOG(2) << "verification result: " << verificationResult;
    } catch (VerificationError& e) {
        // given option is arbitrator without safe applicable option
        option->verificationResult_.reset();

        VLOG(1) << "Given option " << option->behavior_->name_ << " is an arbitrator without safe applicable option";
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
        std::optional<SubCommandT> command;
        try {
            command = getAndVerifyCommand(bestOption, time);
        } catch (const std::exception& e) {
            VLOG(1) << bestOption->behavior_->name_ << " threw an exception during getAndVerifyCommand(): " << e.what();
            bestOption->verificationResult_.reset();
            bestOption->behavior_->loseControl(time);
            continue;
        }

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

} // namespace arbitration_graphs
