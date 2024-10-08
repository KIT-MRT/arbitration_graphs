#pragma once

#include <arbitration_graphs/cost_arbitrator.hpp>
#include <arbitration_graphs/priority_arbitrator.hpp>

#include "avoid_ghost_behavior.hpp"
#include "change_dot_cluster_behavior.hpp"
#include "chase_ghost_behavior.hpp"
#include "cost_estimator.hpp"
#include "eat_closest_dot_behavior.hpp"
#include "environment_model.hpp"
#include "move_randomly_behavior.hpp"
#include "stay_in_place_behavior.hpp"
#include "types.hpp"
#include "verifier.hpp"

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
    using CostArbitrator = arbitration_graphs::CostArbitrator<Command, Command, Verifier, VerificationResult>;
    using PriorityArbitrator = arbitration_graphs::PriorityArbitrator<Command, Command, Verifier, VerificationResult>;

    struct Parameters {
        AvoidGhostBehavior::Parameters avoidGhostBehavior;
        ChaseGhostBehavior::Parameters chaseGhostBehavior;
        MoveRandomlyBehavior::Parameters moveRandomlyBehavior;
        AvoidGhostBehavior::Parameters avoidGhostBehavior_;

        CostEstimator::Parameters costEstimator;
    };

    explicit PacmanAgent(const entt::Game& game)
            : parameters_{}, environmentModel_{std::make_shared<EnvironmentModel>(game)}, verifier_{environmentModel_} {

        avoidGhostBehavior_ = std::make_shared<AvoidGhostBehavior>(environmentModel_, parameters_.avoidGhostBehavior);
        changeDotClusterBehavior_ = std::make_shared<ChangeDotClusterBehavior>(environmentModel_);
        chaseGhostBehavior_ = std::make_shared<ChaseGhostBehavior>(environmentModel_, parameters_.chaseGhostBehavior);
        eatClosestDotBehavior_ = std::make_shared<EatClosestDotBehavior>(environmentModel_);
        moveRandomlyBehavior_ = std::make_shared<MoveRandomlyBehavior>(parameters_.moveRandomlyBehavior);
        stayInPlaceBehavior_ = std::make_shared<StayInPlaceBehavior>(environmentModel_);

        eatDotsArbitrator_ = std::make_shared<CostArbitrator>("EatDots", verifier_);
        costEstimator_ = std::make_shared<CostEstimator>(environmentModel_, parameters_.costEstimator);
        eatDotsArbitrator_->addOption(
            changeDotClusterBehavior_, CostArbitrator::Option::Flags::INTERRUPTABLE, costEstimator_);
        eatDotsArbitrator_->addOption(
            eatClosestDotBehavior_, CostArbitrator::Option::Flags::INTERRUPTABLE, costEstimator_);

        rootArbitrator_ = std::make_shared<PriorityArbitrator>("Pacman", verifier_);
        rootArbitrator_->addOption(chaseGhostBehavior_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
        rootArbitrator_->addOption(avoidGhostBehavior_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
        rootArbitrator_->addOption(eatDotsArbitrator_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
        rootArbitrator_->addOption(moveRandomlyBehavior_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
        rootArbitrator_->addOption(stayInPlaceBehavior_,
                                   PriorityArbitrator::Option::Flags::INTERRUPTABLE |
                                       PriorityArbitrator::Option::FALLBACK);
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

    AvoidGhostBehavior::Ptr avoidGhostBehavior_;
    ChangeDotClusterBehavior::Ptr changeDotClusterBehavior_;
    ChaseGhostBehavior::Ptr chaseGhostBehavior_;
    EatClosestDotBehavior::Ptr eatClosestDotBehavior_;
    MoveRandomlyBehavior::Ptr moveRandomlyBehavior_;
    StayInPlaceBehavior::Ptr stayInPlaceBehavior_;

    PriorityArbitrator::Ptr rootArbitrator_;
    CostArbitrator::Ptr eatDotsArbitrator_;

    CostEstimator::Ptr costEstimator_;
    Verifier verifier_;
};

} // namespace demo
