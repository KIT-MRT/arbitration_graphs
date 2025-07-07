#include "demo/eat_closest_dot_behavior.hpp"

namespace demo {

Command EatClosestDotBehavior::getCommand(const Time& /*time*/, const EnvironmentModel& environmentModel) {
    auto pacmanPosition = environmentModel.pacmanPosition();
    std::optional<Path> pathToClosestDot = environmentModel.pathToClosestDot(pacmanPosition);

    if (!pathToClosestDot) {
        throw std::runtime_error("Failed to compute path to closest dot. Can not provide a sensible command.");
    }

    return Command{pathToClosestDot.value()};
}

bool EatClosestDotBehavior::checkInvocationCondition(const Time& /*time*/,
                                                     const EnvironmentModel& /*environmentModel*/) const {
    // This behavior is only applicable if there is at least one dot. We should check for the presence of a dot
    // here, but since the game is won when all dots are collected, we assume at least one dot exists when this
    // behavior is invoked.
    return true;
}

bool EatClosestDotBehavior::checkCommitmentCondition(const Time& /*time*/,
                                                     const EnvironmentModel& /*environmentModel*/) const {
    return false;
}

} // namespace demo
