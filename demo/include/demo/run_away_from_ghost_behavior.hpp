#pragma once

#include <arbitration_graphs/behavior.hpp>

#include "environment_model.hpp"
#include "types.hpp"

namespace demo {

/**
 * @brief The RunAwayFromGhostBehavior makes Pacman run away from the ghost.
 *
 * The behavior returns the command which increases the distance between Pacman and the ghost that's currently closest
 * to him. It is applicable once a ghost is within a certain distance to Pacman. To prevent oscillating behavior
 * switches, the commitment condition should remain active for longer than the invocation condition.
 */
class RunAwayFromGhostBehavior : public arbitration_graphs::Behavior<Command> {
public:
    using Ptr = std::shared_ptr<RunAwayFromGhostBehavior>;
    using ConstPtr = std::shared_ptr<const RunAwayFromGhostBehavior>;

    struct Parameters {
        double invocationMinDistance{5};
        double commitmentMinDistance{7};
    };

    RunAwayFromGhostBehavior(EnvironmentModel::Ptr environmentModel,
                             const Parameters& parameters,
                             const std::string& name = "RunAwayFromGhostBehavior")
            : Behavior(name), environmentModel_{std::move(environmentModel)}, parameters_{parameters} {
    }

    Command getCommand(const Time& time) override;

    bool checkInvocationCondition(const Time& time) const override;
    bool checkCommitmentCondition(const Time& time) const override;

    void gainControl(const Time& time) override;
    void loseControl(const Time& time) override;

private:
    EnvironmentModel::Ptr environmentModel_;
    Parameters parameters_;


    bool isActive_{false};
};

} // namespace demo
