#pragma once

#include <arbitration_graphs/behavior.hpp>

#include "environment_model.hpp"
#include "types.hpp"

namespace demo {

/**
 * @brief The AvoidGhostBehavior makes Pacman run away from the closest ghost.
 *
 * The behavior returns the command which increases the distance between Pacman and the ghost that's currently closest
 * to him. It is applicable once a ghost is within a certain distance to Pacman. To prevent oscillating behavior
 * switches, the commitment condition should remain active for longer than the invocation condition.
 */
class AvoidGhostBehavior : public arbitration_graphs::Behavior<Command> {
public:
    using Ptr = std::shared_ptr<AvoidGhostBehavior>;
    using ConstPtr = std::shared_ptr<const AvoidGhostBehavior>;

    struct Parameters {
        double invocationMinDistance{5};
        double commitmentMinDistance{7};
    };

    AvoidGhostBehavior(EnvironmentModel::Ptr environmentModel,
                       const Parameters& parameters,
                       const std::string& name = "AvoidGhost")
            : Behavior(name), environmentModel_{std::move(environmentModel)}, parameters_{parameters} {
    }

    Command getCommand(const Time& time) override;

    bool checkInvocationCondition(const Time& time) const override;
    bool checkCommitmentCondition(const Time& time) const override;

private:
    EnvironmentModel::Ptr environmentModel_;
    Parameters parameters_;
};

} // namespace demo
