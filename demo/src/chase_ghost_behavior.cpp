#include "demo/chase_ghost_behavior.hpp"
#include "demo/types.hpp"

namespace demo {

Command ChaseGhostBehavior::getCommand(const Time& time) {
    auto pacmanPosition = environmentModel_->pacmanPosition();
    ///@todo Make sure the closest ghost has not been eaten
    auto ghostPosition = environmentModel_->closestGhost(time).position;
    auto direction = Direction::LAST;

    double minDistance = std::numeric_limits<double>::max();
    for (const auto& move : Move::possibleMoves()) {
        auto nextPosition = pacmanPosition + move.deltaPosition;

        if (environmentModel_->isWall(nextPosition)) {
            continue;
        }

        auto nextDistance = environmentModel_->distance(nextPosition, ghostPosition);
        if (nextDistance < minDistance) {
            direction = move.direction;
            minDistance = nextDistance;
        }
    }

    return {direction};
}

bool ChaseGhostBehavior::checkInvocationCondition(const Time& time) const {
    return environmentModel_->ghostsScared();
}

bool ChaseGhostBehavior::checkCommitmentCondition(const Time& time) const {
    ///@todo Commit until timer starts running out → Timer needs to be extracted first
    return checkInvocationCondition(time);
}

} // namespace demo
