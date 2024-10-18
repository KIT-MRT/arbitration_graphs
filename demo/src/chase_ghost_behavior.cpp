#include "demo/chase_ghost_behavior.hpp"
#include "demo/types.hpp"

namespace demo {

Command ChaseGhostBehavior::getCommand(const Time& time) {
    auto pacmanPosition = environmentModel_->pacmanPosition();

    auto closestScaredGhost = environmentModel_->closestScaredGhost(time);
    if (!closestScaredGhost) {
        throw std::runtime_error("Can not compute command to chase ghost because there are no scared ghosts.");
    }

    auto ghostPosition = closestScaredGhost->ghost.position;

    std::optional<Direction> direction;

    ///@todo Implement logic to get the direction to chase the closest ghost

    if (!direction) {
        throw std::runtime_error("Failed to compute direction to chase the closest ghost.");
    }

    return Command{direction.value()};
}

bool ChaseGhostBehavior::checkInvocationCondition(const Time& time) const {
    ///@todo Also check if ghost is close by
    return environmentModel_->closestScaredGhost(time).has_value() &&
           environmentModel_->closestScaredGhost(time)->ghost.scaredCountdown > parameters_.minScaredTicksLeft;
}

bool ChaseGhostBehavior::checkCommitmentCondition(const Time& time) const {
    return checkInvocationCondition(time);
}

} // namespace demo
