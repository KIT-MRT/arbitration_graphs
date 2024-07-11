#pragma once

#include <arbitration_graphs/priority_arbitrator.hpp>

#include "chase_ghost_behavior.hpp"
#include "environment_model.hpp"
#include "random_walk_behavior.hpp"
#include "run_away_from_ghost_behavior.hpp"
#include "stay_in_place_behavior.hpp"
#include "types.hpp"

namespace demo {

/**
 * @brief The main arbitrator class for the Pacman demo.
 *
 * The PacmanArbitrator class defines the arbitration graph by adding the individual behaviors.
 * It is the represents the top level of the hierarchy and will be called in each time step to return the command that
 * Pacman should execute.
 */
class PacmanAgent {
public:
    using PriorityArbitrator = arbitration_graphs::PriorityArbitrator<Command>;

    struct Parameters {
        ChaseGhostBehavior::Parameters chaseGhostBehavior;
        RandomWalkBehavior::Parameters randomWalkBehavior;
        RunAwayFromGhostBehavior::Parameters runAwayFromGhostBehavior;
    };

    explicit PacmanAgent(const entt::Game& game) {
        parameters_ = Parameters{};
        environmentModel_ = std::make_shared<EnvironmentModel>(game);

        chaseGhostBehavior_ = std::make_shared<ChaseGhostBehavior>(environmentModel_, parameters_.chaseGhostBehavior);
        randomWalkBehavior_ = std::make_shared<RandomWalkBehavior>(parameters_.randomWalkBehavior);
        runAwayFromGhostBehavior_ =
            std::make_shared<RunAwayFromGhostBehavior>(environmentModel_, parameters_.runAwayFromGhostBehavior);
        stayInPlaceBehavior_ = std::make_shared<StayInPlaceBehavior>();

        rootArbitrator_ = std::make_shared<PriorityArbitrator>();
        rootArbitrator_->addOption(chaseGhostBehavior_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
        rootArbitrator_->addOption(runAwayFromGhostBehavior_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
        rootArbitrator_->addOption(randomWalkBehavior_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
        rootArbitrator_->addOption(stayInPlaceBehavior_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
    }

    Command getCommand(const Time& time) {
        return rootArbitrator_->getCommand(time);
    }

    void updateEnvironmentModel(const entt::Game& game) {
        environmentModel_->update(game);
    }

    std::ostream& to_stream(std::ostream& output, const Time& time) const {
        return rootArbitrator_->to_stream(output, time);
    }
    std::string to_str(const Time& time) const {
        std::stringstream stringStream;
        to_stream(stringStream, time);
        return stringStream.str();
    }

private:
    EnvironmentModel::Ptr environmentModel_;
    Parameters parameters_;

    ChaseGhostBehavior::Ptr chaseGhostBehavior_;
    RandomWalkBehavior::Ptr randomWalkBehavior_;
    RunAwayFromGhostBehavior::Ptr runAwayFromGhostBehavior_;
    StayInPlaceBehavior::Ptr stayInPlaceBehavior_;

    PriorityArbitrator::Ptr rootArbitrator_;
};

} // namespace demo
