#pragma once

#include <arbitration_graphs/behavior.hpp>

#include "environment_model.hpp"
#include "types.hpp"

namespace demo {

struct NextMove {
    Direction direction;
    Position deltaPosition;
};
using NextMoves = std::vector<NextMove>;

class RunAwayFromGhostBehavior : public arbitration_graphs::Behavior<Command> {
public:
    using Ptr = std::shared_ptr<RunAwayFromGhostBehavior>;
    using ConstPtr = std::shared_ptr<const RunAwayFromGhostBehavior>;

    struct Parameters {
        double invocationMinDistance{5};
        double commitmentMinDistance{7};
    };

    RunAwayFromGhostBehavior(EnvironmentModel::Ptr environmentModel,
                             const Parameters& parameters,
                             const std::string& name = "RunAwayFromGhostBehavior")
            : Behavior(name), environmentModel_{std::move(environmentModel)}, parameters_{parameters} {
    }

    Command getCommand(const Time& time) override;

    bool checkInvocationCondition(const Time& time) const override;
    bool checkCommitmentCondition(const Time& time) const override;

    void gainControl(const Time& time) override;
    void loseControl(const Time& time) override;

private:
    EnvironmentModel::Ptr environmentModel_;
    Parameters parameters_;

    NextMoves possibleNextMoves_{
        {Direction::UP, {0, -1}}, {Direction::DOWN, {0, 1}}, {Direction::LEFT, {-1, 0}}, {Direction::RIGHT, {1, 0}}};

    bool isActive_{false};
};

} // namespace demo
