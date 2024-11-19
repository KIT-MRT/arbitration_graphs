#pragma once

#include <arbitration_graphs/behavior.hpp>

#include "environment_model.hpp"
#include "types.hpp"

namespace demo {

/**
 * @brief The DoNothingBehavior does just that. Nothing.
 *
 * The behavior can be called any time and will always return a command with direction NONE.
 * It is intended to be used as a fallback behavior when no other behavior is available.
 * Due to the fact that Pacman will never stop moving, this means that when this behavior is called, Pacman will keep
 * moving in the same direction as before.
 */
class StayInPlaceBehavior : public arbitration_graphs::Behavior<Command> {
public:
    using Ptr = std::shared_ptr<StayInPlaceBehavior>;
    using ConstPtr = std::shared_ptr<const StayInPlaceBehavior>;

    explicit StayInPlaceBehavior(EnvironmentModel::Ptr environmentModel, const std::string& name = "StayInPlace")
            : Behavior(name), environmentModel_{std::move(environmentModel)} {
    }

    Command getCommand(const Time& /*time*/) override;

    bool checkInvocationCondition(const Time& /*time*/) const override;
    bool checkCommitmentCondition(const Time& /*time*/) const override;

private:
    EnvironmentModel::Ptr environmentModel_;

    static Direction oppositeDirection(const Direction& direction) {
        const std::map<Direction, Direction> oppositeDirectionMap{{Direction::UP, Direction::DOWN},
                                                                  {Direction::DOWN, Direction::UP},
                                                                  {Direction::LEFT, Direction::RIGHT},
                                                                  {Direction::RIGHT, Direction::LEFT}};
        return oppositeDirectionMap.at(direction);
    }
};

} // namespace demo
