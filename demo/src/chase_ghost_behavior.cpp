#include "demo/chase_ghost_behavior.hpp"
#include "demo/types.hpp"

namespace demo {

Command ChaseGhostBehavior::getCommand(const Time& time, const EnvironmentModel& environmentModel) {
    auto pacmanPosition = environmentModel_->pacmanPosition();

    auto closestScaredGhost = environmentModel_->closestScaredGhost(time);
    if (!closestScaredGhost) {
        throw std::runtime_error("Can not compute command to chase ghost because there are no scared ghosts.");
    }

    auto ghostPosition = closestScaredGhost->ghost.position;

    std::optional<Direction> direction;
    double minDistance = std::numeric_limits<double>::max();
    for (const auto& move : Move::possibleMoves()) {
        auto nextPosition = environmentModel_->positionConsideringTunnel(pacmanPosition + move.deltaPosition);

        if (environmentModel_->isWall(nextPosition)) {
            continue;
        }

        // Chose the direction moving pacman towards the closest scared ghost (considering ghost movement)
        auto nextDistance = environmentModel_->mazeDistance(nextPosition, ghostPosition);
        if (nextDistance < minDistance) {
            direction = move.direction;
            minDistance = nextDistance;
        }
    }

    if (!direction) {
        throw std::runtime_error("Failed to compute direction to chase the closest ghost.");
    }

    return Command{direction.value()};
}

bool ChaseGhostBehavior::checkInvocationCondition(const Time& time, const EnvironmentModel& environmentModel) const {
    return environmentModel_->closestScaredGhost(time).has_value() &&
           environmentModel_->closestScaredGhost(time)->ghost.scaredCountdown > parameters_.minScaredTicksLeft &&
           environmentModel_->closestScaredGhost(time)->distance < parameters_.invocationMinDistance;
}

bool ChaseGhostBehavior::checkCommitmentCondition(const Time& time, const EnvironmentModelT& environmentModel) const {
    return checkInvocationCondition(time);
}

} // namespace demo
