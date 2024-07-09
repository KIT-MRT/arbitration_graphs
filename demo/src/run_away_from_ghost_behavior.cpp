#include "demo/run_away_from_ghost_behavior.hpp"
#include "demo/types.hpp"

namespace demo {

Command RunAwayFromGhostBehavior::getCommand(const Time& time) {
    auto pacmanPosition = environmentModel_->pacmanPosition();
    auto ghostPosition = environmentModel_->closestGhost(time).ghost.position;
    auto direction = Direction::LAST;

    double maxDistance = -1;
    for (const auto& move : Move::possibleMoves()) {
        auto nextPosition = pacmanPosition + move.deltaPosition;

        if (environmentModel_->isWall(nextPosition)) {
            continue;
        }

        auto nextDistance = environmentModel_->distance(nextPosition, ghostPosition);
        if (nextDistance > maxDistance) {
            direction = move.direction;
            maxDistance = nextDistance;
        }
    }

    return {direction};
}

bool RunAwayFromGhostBehavior::checkInvocationCondition(const Time& time) const {
    return environmentModel_->closestGhost(time).distance < parameters_.invocationMinDistance;
}

bool RunAwayFromGhostBehavior::checkCommitmentCondition(const Time& time) const {
    return environmentModel_->closestGhost(time).distance < parameters_.commitmentMinDistance;
}

} // namespace demo
