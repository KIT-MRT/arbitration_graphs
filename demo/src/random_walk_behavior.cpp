#include "demo/random_walk_behavior.hpp"

namespace demo {

Command RandomWalkBehavior::getCommand(const Time& time) {
    util_caching::policies::ApproximateTime<Time, std::chrono::seconds> approximateTimePolicy{
        parameters_.selectionValidFor.count()};
    if (directionCache_.cached(time, approximateTimePolicy)) {
        return Command{directionCache_.cached(time, approximateTimePolicy).value()};
    }

    Direction randomDirection = selectRandomDirection();

    directionCache_.cache(time, randomDirection);
    return Command{randomDirection};
}

Direction RandomWalkBehavior::selectRandomDirection() {
    int randomIndex = discreteRandomDistribution_(randomGenerator_);
    Move randomMove = Move::possibleMoves()[randomIndex];

    return randomMove.direction;
}

} // namespace demo
