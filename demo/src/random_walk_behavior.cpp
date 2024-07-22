#include "demo/random_walk_behavior.hpp"

namespace demo {

Command RandomWalkBehavior::getCommand(const Time& time) {
    util_caching::policies::ApproximateTime<Time, std::chrono::seconds> approximateTimePolicy{
        parameters_.selectionFixedFor.count()};
    if (directionCache_.cached(time, approximateTimePolicy)) {
        return directionCache_.cached(time, approximateTimePolicy).value();
    }

    Direction randomDirection = selectRandomDirection();

    directionCache_.cache(time, randomDirection);
    return randomDirection;
}

Direction RandomWalkBehavior::selectRandomDirection() {
    int randomIndex = discreteRandomDistribution_(randomGenerator_);
    Move randomMove = Move::possibleMoves().at(randomIndex);

    return randomMove.direction;
}

} // namespace demo
