#pragma once

#include <arbitration_graphs/priority_arbitrator.hpp>

#include "environment_model.hpp"
#include "types.hpp"
#include "demo/run_away_from_ghost_behavior.hpp"

namespace demo {


class PacmanArbitrator {
public:
    using PriorityArbitrator = arbitration_graphs::PriorityArbitrator<Command>;

    struct Parameters {
        RunAwayFromGhostBehavior::Parameters runAwayFromGhostBehavior;
    };

    PacmanArbitrator() {
        parameters_ = Parameters{};
        environmentModel_ = std::make_shared<EnvironmentModel>();

        runAwayFromGhostBehavior_ =
            std::make_shared<RunAwayFromGhostBehavior>(environmentModel_, RunAwayFromGhostBehavior::Parameters{});
    }

    Command getCommand(const Time& time) {
        return rootArbitrator_->getCommand(time);
    }

    void updateEnvironmentModel(entt::registry& registry, const entt::MazeState& mazeState) {
        environmentModel_->update(registry, mazeState);
    }

    std::ostream& to_stream(std::ostream& output, const Time& time) const {
        return rootArbitrator_->to_stream(output, time);
    }
    std::string to_str(const Time& time) const {
        std::stringstream ss;
        to_stream(ss, time);
        return ss.str();
    }

private:
    EnvironmentModel::Ptr environmentModel_;
    Parameters parameters_;

    RunAwayFromGhostBehavior::Ptr runAwayFromGhostBehavior_;

    PriorityArbitrator::Ptr rootArbitrator_;
};

} // namespace demo