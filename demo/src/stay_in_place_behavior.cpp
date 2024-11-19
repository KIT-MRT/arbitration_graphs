#include "demo/stay_in_place_behavior.hpp"

namespace demo {

Command StayInPlaceBehavior::getCommand(const Time& /*time*/) {
    Direction currentDirection = environmentModel_->pacmanDirection();
    return Command{oppositeDirection(currentDirection)};
}

bool StayInPlaceBehavior::checkInvocationCondition(const Time& /*time*/) const {
    return true;
}
bool StayInPlaceBehavior::checkCommitmentCondition(const Time& /*time*/) const {
    return false;
}

} // namespace demo