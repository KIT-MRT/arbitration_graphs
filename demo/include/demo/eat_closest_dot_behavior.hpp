#pragma once

#include <arbitration_graphs/behavior.hpp>

#include "environment_model.hpp"
#include "types.hpp"

namespace demo {

/**
 * @brief The EatClosestDotBehavior makes Pac-Man move towards the closest dot.
 */
class EatClosestDotBehavior : public arbitration_graphs::Behavior<EnvironmentModel, Command> {
public:
    using Ptr = std::shared_ptr<EatClosestDotBehavior>;
    using ConstPtr = std::shared_ptr<const EatClosestDotBehavior>;

    explicit EatClosestDotBehavior(const std::string& name = "EatClosestDot") : Behavior(name) {
    }

    Command getCommand(const Time& /*time*/, const EnvironmentModel& environmentModel) override;

    bool checkInvocationCondition(const Time& /*time*/, const EnvironmentModel& /*environmentModel*/) const override;
    bool checkCommitmentCondition(const Time& /*time*/, const EnvironmentModel& /*environmentModel*/) const override;
};

} // namespace demo
