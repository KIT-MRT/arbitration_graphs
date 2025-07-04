#include "demo/move_randomly_behavior.hpp"

namespace demo {

Command MoveRandomlyBehavior::getCommand(const Time& time, const EnvironmentModel& /*environmentModel*/) {
    util_caching::policies::ApproximateTime<Time, std::chrono::seconds> approximateTimePolicy{
        parameters_.selectionFixedFor.count()};
    if (directionCache_.cached(time, approximateTimePolicy)) {
        return Command{directionCache_.cached(time, approximateTimePolicy).value()};
    }

    Direction randomDirection = selectRandomDirection();

    directionCache_.cache(time, randomDirection);
    return Command{randomDirection};
}

Direction MoveRandomlyBehavior::selectRandomDirection() {
    int randomIndex = discreteRandomDistribution_(randomGenerator_);
    Move randomMove = Move::possibleMoves().at(randomIndex);

    return randomMove.direction;
}

bool MoveRandomlyBehavior::checkInvocationCondition(const Time& /*time*/,
                                                    const EnvironmentModel& /*environmentModel*/) const {
    return true;
}
bool MoveRandomlyBehavior::checkCommitmentCondition(const Time& /*time*/,
                                                    const EnvironmentModel& /*environmentModel*/) const {
    return false;
}

} // namespace demo
