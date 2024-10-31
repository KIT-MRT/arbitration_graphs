#include "demo/avoid_ghost_behavior.hpp"
#include "demo/types.hpp"

namespace demo {

Command AvoidGhostBehavior::getCommand(const Time& time) {
    auto pacmanPosition = environmentModel_->pacmanPosition();
    auto ghostPosition = environmentModel_->closestGhost(time).ghost.position;

    std::optional<Direction> direction;
    double maxDistance = -1;
    for (const auto& move : Move::possibleMoves()) {
        auto nextPosition = environmentModel_->positionConsideringTunnel(pacmanPosition + move.deltaPosition);

        if (environmentModel_->isWall(nextPosition)) {
            continue;
        }

        auto nextDistance = environmentModel_->mazeDistance(nextPosition, ghostPosition);
        if (nextDistance > maxDistance) {
            direction = move.direction;
            maxDistance = nextDistance;
        }
    }

    if (!direction) {
        throw std::runtime_error("Failed to compute direction to chase the closest ghost.");
    }

    return Command{direction.value()};
}

bool AvoidGhostBehavior::checkInvocationCondition(const Time& time) const {
    return environmentModel_->closestGhost(time).distance < parameters_.invocationMinDistance;
}

bool AvoidGhostBehavior::checkCommitmentCondition(const Time& time) const {
    return environmentModel_->closestGhost(time).distance < parameters_.commitmentMinDistance;
}

} // namespace demo
