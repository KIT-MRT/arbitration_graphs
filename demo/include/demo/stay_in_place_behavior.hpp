#pragma once

#include <arbitration_graphs/behavior.hpp>

#include "environment_model.hpp"
#include "types.hpp"

namespace demo {

/**
 * @brief The StayInPlaceBehavior does just that. Since Pac-Man cannot stop moving, this behavior will oscillate between
 * the current direction and the opposite direction.
 *
 * The behavior can be called any time. It is intended to be used as a fallback behavior when no other behavior is
 * available.
 */
class StayInPlaceBehavior : public arbitration_graphs::Behavior<EnvironmentModel, Command> {
public:
    using Ptr = std::shared_ptr<StayInPlaceBehavior>;
    using ConstPtr = std::shared_ptr<const StayInPlaceBehavior>;

    explicit StayInPlaceBehavior(const std::string& name = "StayInPlace") : Behavior(name) {
    }

    StayInPlaceBehavior(const StayInPlaceBehavior&) = default;
    StayInPlaceBehavior(StayInPlaceBehavior&&) = default;
    StayInPlaceBehavior& operator=(const StayInPlaceBehavior&) = default;
    StayInPlaceBehavior& operator=(StayInPlaceBehavior&&) = default;
    virtual ~StayInPlaceBehavior() = default;

    Command getCommand(const Time& /*time*/, const EnvironmentModel& environmentModel) override;

    bool checkInvocationCondition(const Time& /*time*/, const EnvironmentModel& environmentModel) const override;
    bool checkCommitmentCondition(const Time& /*time*/, const EnvironmentModel& environmentModel) const override;

private:
    static Direction oppositeDirection(const Direction& direction) {
        const std::map<Direction, Direction> oppositeDirectionMap{{Direction::Up, Direction::Down},
                                                                  {Direction::Down, Direction::Up},
                                                                  {Direction::Left, Direction::Right},
                                                                  {Direction::Right, Direction::Left}};
        return oppositeDirectionMap.at(direction);
    }
};

} // namespace demo
