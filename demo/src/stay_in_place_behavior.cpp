#include "demo/stay_in_place_behavior.hpp"

namespace demo {

Command StayInPlaceBehavior::getCommand(const Time& /*time*/, const EnvironmentModel& environmentModel) {
    Direction currentDirection = environmentModel.pacmanDirection();
    return Command{oppositeDirection(currentDirection)};
}

bool StayInPlaceBehavior::checkInvocationCondition(const Time& /*time*/,
                                                   const EnvironmentModel& /*environmentModel*/) const {
    return true;
}
bool StayInPlaceBehavior::checkCommitmentCondition(const Time& /*time*/,
                                                   const EnvironmentModel& /*environmentModel*/) const {
    return false;
}

} // namespace demo
