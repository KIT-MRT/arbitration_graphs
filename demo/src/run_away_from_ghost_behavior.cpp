#include "demo/run_away_from_ghost_behavior.hpp"
#include "demo/types.hpp"

namespace demo {

Command RunAwayFromGhostBehavior::getCommand(const Time& time) {
    auto pacmanPosition = environmentModel_->pacmanPosition();
    auto ghostPosition = environmentModel_->closestGhostPosition();
    auto direction = Direction::NONE;

    double maxDistance = -1;
    for (const auto& nextMove : possibleNextMoves_) {
        auto nextPosition = pacmanPosition + nextMove.deltaPosition;

        if (environmentModel_->isWall(nextPosition)) {
            continue;
        }

        auto nextDistance = nextPosition.distance(ghostPosition);
        if (nextDistance > maxDistance) {
            direction = nextMove.direction;
            maxDistance = nextDistance;
        }
    }

    return {direction};
}

bool RunAwayFromGhostBehavior::checkInvocationCondition(const Time& time) const {
    return environmentModel_->closestGhostDistance() < parameters_.invocationMinDistance;
}

bool RunAwayFromGhostBehavior::checkCommitmentCondition(const Time& time) const {
    return environmentModel_->closestGhostDistance() < parameters_.commitmentMinDistance;
}

void RunAwayFromGhostBehavior::gainControl(const Time& time) {
    isActive_ = true;
}

void RunAwayFromGhostBehavior::loseControl(const Time& time) {
    isActive_ = false;
}

} // namespace demo