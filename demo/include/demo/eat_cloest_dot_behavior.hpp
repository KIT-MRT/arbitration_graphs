#pragma once

#include <arbitration_graphs/behavior.hpp>

#include "environment_model.hpp"
#include "types.hpp"

namespace demo {

/**
 * @brief The EatClosestDotBehavior makes pacman move towards the closest dot.
 */
class EatClosestDotBehavior : public arbitration_graphs::Behavior<Command> {
public:
    using Ptr = std::shared_ptr<EatClosestDotBehavior>;
    using ConstPtr = std::shared_ptr<const EatClosestDotBehavior>;

    explicit EatClosestDotBehavior(EnvironmentModel::Ptr environmentModel,
                                   const std::string& name = "EatClosestDotBehavior")
            : Behavior(name), environmentModel_{std::move(environmentModel)} {
    }

    Command getCommand(const Time& /*time*/) override {
        auto pacmanPosition = environmentModel_->pacmanPosition();
        Path pathToClosestDot = environmentModel_->pathToClosestDot(pacmanPosition);

        if (pathToClosestDot.empty()) {
            throw std::runtime_error("Failed to compute path to closest dot. Can not provide a sensible command.");
        }

        return Command{pathToClosestDot};
    }

    bool checkInvocationCondition(const Time& /*time*/) const override {
        // This behavior is only applicable if there is at least one dot. We should check for the presence of a dot
        // here, but since the game is won when all dots are collected, we assume at least one dot exists when this
        // behavior is invoked.
        return true;
    }

    bool checkCommitmentCondition(const Time& /*time*/) const override {
        return false;
    }

private:
    EnvironmentModel::Ptr environmentModel_;
};

} // namespace demo
