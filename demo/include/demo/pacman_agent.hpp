#pragma once

#include <arbitration_graphs/priority_arbitrator.hpp>

#include "avoid_ghost_behavior.hpp"
#include "eat_closest_dot_behavior.hpp"
#include "environment_model.hpp"
#include "move_randomly_behavior.hpp"
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
    using PriorityArbitrator = arbitration_graphs::PriorityArbitrator<Command, Command>;

    struct Parameters {
        AvoidGhostBehavior::Parameters avoidGhostBehavior;
        MoveRandomlyBehavior::Parameters moveRandomlyBehavior;
    };

    explicit PacmanAgent(const entt::Game& game)
            : parameters_{}, environmentModel_{std::make_shared<EnvironmentModel>(game)} {

        avoidGhostBehavior_ = std::make_shared<AvoidGhostBehavior>(environmentModel_, parameters_.avoidGhostBehavior);
        eatClosestDotBehavior_ = std::make_shared<EatClosestDotBehavior>(environmentModel_);
        moveRandomlyBehavior_ = std::make_shared<MoveRandomlyBehavior>(parameters_.moveRandomlyBehavior);

        rootArbitrator_ = std::make_shared<PriorityArbitrator>("Pacman");
        rootArbitrator_->addOption(avoidGhostBehavior_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
        rootArbitrator_->addOption(eatClosestDotBehavior_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
        rootArbitrator_->addOption(moveRandomlyBehavior_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
    }

    Command getCommand(const Time& time) {
        return rootArbitrator_->getCommand(time);
    }

    void updateEnvironmentModel(const entt::Game& game) {
        environmentModel_->update(game);
    }

    EnvironmentModel::ConstPtr environmentModel() const {
        return environmentModel_;
    }

    std::ostream& to_stream(std::ostream& output, const Time& time) const {
        return rootArbitrator_->to_stream(output, time);
    }
    std::string to_str(const Time& time) const {
        std::stringstream stringStream;
        to_stream(stringStream, time);
        return stringStream.str();
    }
    std::string yamlString(const Time& time) const {
        YAML::Node node;
        node["type"] = "PacmanArbitrator";
        node["arbitration"] = rootArbitrator_->toYaml(time);
        std::stringstream yamlString;
        yamlString << node;
        return yamlString.str();
    }

private:
    EnvironmentModel::Ptr environmentModel_;
    Parameters parameters_;

    AvoidGhostBehavior::Ptr avoidGhostBehavior_;
    EatClosestDotBehavior::Ptr eatClosestDotBehavior_;
    MoveRandomlyBehavior::Ptr moveRandomlyBehavior_;

    PriorityArbitrator::Ptr rootArbitrator_;
};

} // namespace demo
