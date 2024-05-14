#include "demo/run_away_from_ghost_behavior.hpp"

namespace demo {

Command RunAwayFromGhostBehavior::getCommand(const Time& time) {
    auto pacmanPosition = environmentModel_->pacmanPosition();
    auto ghostPosition = environmentModel_->closestGhostPosition();
    auto direction = Direction::NONE;

    if (pacmanPosition.x < ghostPosition.x) {
        direction = Direction::LEFT;
    } else if (pacmanPosition.x > ghostPosition.x) {
        direction = Direction::RIGHT;
    } else if (pacmanPosition.y < ghostPosition.y) {
        direction = Direction::DOWN;
    } else if (pacmanPosition.y > ghostPosition.y) {
        direction = Direction::UP;
    }

    return Command{direction};
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