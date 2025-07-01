#pragma once

#include "../arbitrator.hpp"

#include <glog/logging.h>
#include "verification.hpp"


namespace arbitration_graphs {

template <typename EnvironmentModelT, typename CommandT, typename SubCommandT>
typename Arbitrator<EnvironmentModelT, CommandT, SubCommandT>::Options Arbitrator<
    EnvironmentModelT,
    CommandT,
    SubCommandT>::applicableOptions(const Time& time, const EnvironmentModelT& environmentModel) const {

    Options options;
    std::copy_if(behaviorOptions_.begin(),
                 behaviorOptions_.end(),
                 std::back_inserter(options),
                 std::bind(&Arbitrator::isApplicable, this, std::placeholders::_1, time, environmentModel));
    return options;
};

template <typename EnvironmentModelT, typename CommandT, typename SubCommandT>
bool Arbitrator<EnvironmentModelT, CommandT, SubCommandT>::isActive(const typename Option::Ptr& option) const {
    return activeBehavior_ && option == activeBehavior_;
}

template <typename EnvironmentModelT, typename CommandT, typename SubCommandT>
bool Arbitrator<EnvironmentModelT, CommandT, SubCommandT>::isApplicable(
    const typename Option::Ptr& option, const Time& time, const EnvironmentModelT& environmentModel) const {
    const bool isActiveAndCanBeContinued =
        isActive(option) && option->behavior_->checkCommitmentCondition(time, environmentModel);
    return isActiveAndCanBeContinued || option->behavior_->checkInvocationCondition(time, environmentModel);
}

template <typename EnvironmentModelT, typename CommandT, typename SubCommandT>
std::size_t Arbitrator<EnvironmentModelT, CommandT, SubCommandT>::getOptionIndex(
    const typename Option::ConstPtr& behaviorOption) const {
    const auto it = std::find(behaviorOptions_.begin(), behaviorOptions_.end(), behaviorOption);

    if (it != behaviorOptions_.end()) {
        return std::distance(behaviorOptions_.begin(), it);
    }
    throw InvalidArgumentsError(
        "Invalid call of getOptionIndex(): Given option not found in list of behavior options!");
}

template <typename EnvironmentModelT, typename CommandT, typename SubCommandT>
std::optional<SubCommandT> Arbitrator<EnvironmentModelT, CommandT, SubCommandT>::getAndVerifyCommand(
    const typename Option::Ptr& option, const Time& time, const EnvironmentModelT& environmentModel) const {
    try {
        const SubCommandT command = option->getCommand(time, environmentModel);

        const verification::AbstractResult::ConstPtr verificationResult =
            verifier_->analyze(time, environmentModel, command);
        option->verificationResult_.cache(time, verificationResult);

        // options explicitly flagged as fallback do not need to pass verification
        if (verificationResult->isOk() || option->hasFlag(Option::Flags::FALLBACK)) {
            return command;
        }
        // given option is applicable, but not safe
        VLOG(1) << "Given option " << option->behavior_->name_ << " is applicable, but not safe";
        VLOG(2) << "verification result: " << verificationResult;
    } catch (VerificationError& e) {
        // given option is arbitrator without safe applicable option
        option->verificationResult_.reset();

        VLOG(1) << "Given option " << option->behavior_->name_ << " is an arbitrator without safe applicable option";
    } catch (const std::exception& e) {
        // Catch all other exceptions and cache failed verification result
        option->verificationResult_.cache(time, std::make_shared<verification::PlaceboResult>(false));
        VLOG(1) << "Given option " << option->behavior_->name_
                << " threw an exception during getAndVerifyCommand(): " << e.what();
    }
    return std::nullopt;
}

template <typename EnvironmentModelT, typename CommandT, typename SubCommandT>
std::optional<SubCommandT> Arbitrator<EnvironmentModelT, CommandT, SubCommandT>::getAndVerifyCommandFromActive(
    const Time& time, const EnvironmentModelT& environmentModel) {
    bool activeBehaviorCanBeContinued =
        activeBehavior_ && activeBehavior_->behavior_->checkCommitmentCondition(time, environmentModel);

    if (activeBehavior_ && !activeBehaviorCanBeContinued) {
        activeBehavior_->behavior_->loseControl(time, environmentModel);
        activeBehavior_.reset();
    }

    bool activeBehaviorInterruptable = activeBehavior_ && (activeBehavior_->hasFlag(Option::Flags::INTERRUPTABLE));

    // continue with active behavior, if one exists, it is committed, not interruptable and passes verification
    if (activeBehaviorCanBeContinued && !activeBehaviorInterruptable) {
        const std::optional<SubCommandT> command = getAndVerifyCommand(activeBehavior_, time, environmentModel);
        if (command) {
            return command.value();
        }

        activeBehavior_->behavior_->loseControl(time, environmentModel);
        activeBehavior_ = nullptr;
    }

    return std::nullopt;
}

template <typename EnvironmentModelT, typename CommandT, typename SubCommandT>
SubCommandT Arbitrator<EnvironmentModelT, CommandT, SubCommandT>::getAndVerifyCommandFromApplicable(
    const Options& options, const Time& time, const EnvironmentModelT& environmentModel) {
    for (const auto& bestOption : options) {
        if (!activeBehavior_ || bestOption != activeBehavior_) {
            // we allow bestOption and activeBehavior_ to gain control simultaneuosly until we figure out
            // if bestOption passes verification
            bestOption->behavior_->gainControl(time, environmentModel);
        }
        // otherwise we have bestOption == activeBehavior_ which already gained control

        // an arbitrator as option might not return a command,
        // if its applicable options fail verification or throw an exception:
        const std::optional<SubCommandT> command = getAndVerifyCommand(bestOption, time, environmentModel);
        if (command) {
            if (activeBehavior_ && bestOption != activeBehavior_) {
                // finally, prevent two behaviors from having control
                activeBehavior_->behavior_->loseControl(time, environmentModel);
            }
            activeBehavior_ = bestOption;
            return command.value();
        }
        bestOption->behavior_->loseControl(time, environmentModel);
    }

    throw NoApplicableOptionPassedVerificationError("None of the " + std::to_string(options.size()) +
                                                    " applicable options passed the verification step!");
}

} // namespace arbitration_graphs
