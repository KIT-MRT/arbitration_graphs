#pragma once

#include <arbitration_graphs/behavior.hpp>
#include <util_caching/cache.hpp>

#include "environment_model.hpp"
#include "types.hpp"

namespace demo {

/**
 * @brief The MoveRandomly moves Pac-Man in a random direction.
 *
 * This behavior will not consider ghosts or walls. Once a direction is selected, it will be returned for a defined
 * amount of time to avoid changing the direction too frequently.
 *
 */
class MoveRandomlyBehavior : public arbitration_graphs::Behavior<EnvironmentModel, Command> {
public:
    using Ptr = std::shared_ptr<MoveRandomlyBehavior>;
    using ConstPtr = std::shared_ptr<const MoveRandomlyBehavior>;

    struct Parameters {
        Duration selectionFixedFor{std::chrono::seconds(1)};
    };

    explicit MoveRandomlyBehavior(const Parameters& parameters, const std::string& name = "MoveRandomly")
            : Behavior{name}, parameters_{parameters} {
    }

    Command getCommand(const Time& time, const EnvironmentModel& /*environmentModel*/) override;

    bool checkInvocationCondition(const Time& /*time*/, const EnvironmentModel& /*environmentModel*/) const override;
    bool checkCommitmentCondition(const Time& /*time*/, const EnvironmentModel& /*environmentModel*/) const override;

private:
    Direction selectRandomDirection();

    util_caching::Cache<Time, Direction> directionCache_;
    Parameters parameters_;

    std::random_device randomDevice_;
    std::mt19937 randomGenerator_{randomDevice_()};
    std::uniform_int_distribution<> discreteRandomDistribution_{0, static_cast<int>(Move::possibleMoves().size()) - 1};
};

} // namespace demo
