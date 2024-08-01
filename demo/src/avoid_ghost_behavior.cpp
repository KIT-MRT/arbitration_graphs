#include "demo/avoid_ghost_behavior.hpp"
#include "demo/types.hpp"

namespace demo {

Command AvoidGhostBehavior::getCommand(const Time& time) {
    auto pacmanPosition = environmentModel_->pacmanPosition();
    auto ghostPosition = environmentModel_->closestGhost(time).ghost.position;
    auto direction = Direction::LAST;

    double maxDistance = -1;
    for (const auto& move : Move::possibleMoves()) {
        auto nextPosition = pacmanPosition + move.deltaPosition;

        if (environmentModel_->isWall(nextPosition)) {
            continue;
        }

        auto nextDistance = environmentModel_->manhattanDistance(nextPosition, ghostPosition);
        if (nextDistance > maxDistance) {
            direction = move.direction;
            maxDistance = nextDistance;
        }
    }

    return Command{direction};
}

bool AvoidGhostBehavior::checkInvocationCondition(const Time& time) const {
    return environmentModel_->closestGhost(time).distance < parameters_.invocationMinDistance;
}

bool AvoidGhostBehavior::checkCommitmentCondition(const Time& time) const {
    return environmentModel_->closestGhost(time).distance < parameters_.commitmentMinDistance;
}

} // namespace demo
