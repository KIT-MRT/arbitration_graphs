#include "demo/chase_ghost_behavior.hpp"

namespace demo {

Command ChaseGhostBehavior::getCommand(const Time& time) {
    auto pacmanPosition = environmentModel_->pacmanPosition();

    auto closestScaredGost = environmentModel_->closestScaredGhost(time);
    if (!closestScaredGost) {
        throw std::runtime_error("Can not compute command to chase ghost because there are no scared ghosts.");
    }

    auto ghostPosition = closestScaredGost->ghost.position;
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
    return environmentModel_->closestScaredGhost(time).has_value() &&
           environmentModel_->closestScaredGhost(time)->ghost.scaredCountdown > parameters_.minScaredTicksLeft;
}

bool ChaseGhostBehavior::checkCommitmentCondition(const Time& time) const {
    return checkInvocationCondition(time);
}

} // namespace demo
