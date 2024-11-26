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

    explicit EatClosestDotBehavior(EnvironmentModel::Ptr environmentModel, const std::string& name = "EatClosestDot")
            : Behavior(name), environmentModel_{std::move(environmentModel)} {
    }

    Command getCommand(const Time& /*time*/) override;

    bool checkInvocationCondition(const Time& /*time*/) const override;
    bool checkCommitmentCondition(const Time& /*time*/) const override;

private:
    EnvironmentModel::Ptr environmentModel_;
};

} // namespace demo
