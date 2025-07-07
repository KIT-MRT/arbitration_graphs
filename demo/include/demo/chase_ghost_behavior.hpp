#pragma once

#include <arbitration_graphs/behavior.hpp>

#include "environment_model.hpp"
#include "types.hpp"

namespace demo {

/**
 * @brief The ChaseGhostBehavior makes Pac-Man run away from the ghost.
 *
 * The behavior returns the command which decreases the distance between Pac-Man and the ghost that's currently closest
 * to him. It is applicable once Pac-Man ate a power pill and the ghosts are scared of him.
 */
class ChaseGhostBehavior : public arbitration_graphs::Behavior<EnvironmentModel, Command> {
public:
    using Ptr = std::shared_ptr<ChaseGhostBehavior>;
    using ConstPtr = std::shared_ptr<const ChaseGhostBehavior>;

    struct Parameters {
        double invocationMinDistance{5};

        ///@brief The minimum number of ticks the ghosts should be scared for for this behavior to be applicable
        int minScaredTicksLeft{5};
    };

    explicit ChaseGhostBehavior(const Parameters& parameters, const std::string& name = "ChaseGhost")
            : Behavior(name), parameters_{parameters} {
    }

    Command getCommand(const Time& time, const EnvironmentModel& environmentModel) override;

    bool checkInvocationCondition(const Time& time, const EnvironmentModel& environmentModel) const override;
    bool checkCommitmentCondition(const Time& time, const EnvironmentModel& environmentModel) const override;

private:
    Parameters parameters_;
};

} // namespace demo
