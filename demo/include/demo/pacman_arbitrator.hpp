#pragma once

#include <arbitration_graphs/priority_arbitrator.hpp>

#include "do_nothing_behavior.hpp"
#include "environment_model.hpp"
#include "types.hpp"
#include "demo/do_nothing_behavior.hpp"
#include "demo/run_away_from_ghost_behavior.hpp"

namespace demo {

/**
 * @brief The main arbitrator class for the Pacman demo.
 *
 * The PacmanArbitrator class defines the arbitration graph by adding the individual behaviors.
 * It is the represents the top level of the hierarchy and will be called in each time step to return the command that
 * Pacman should execute.
 */
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
        doNothingBehavior_ = std::make_shared<DoNothingBehavior>();

        rootArbitrator_ = std::make_shared<PriorityArbitrator>();
        rootArbitrator_->addOption(runAwayFromGhostBehavior_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
        rootArbitrator_->addOption(doNothingBehavior_, PriorityArbitrator::Option::Flags::NO_FLAGS);
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

    DoNothingBehavior::Ptr doNothingBehavior_;
    RunAwayFromGhostBehavior::Ptr runAwayFromGhostBehavior_;

    PriorityArbitrator::Ptr rootArbitrator_;
};

} // namespace demo