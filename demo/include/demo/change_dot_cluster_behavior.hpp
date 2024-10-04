#pragma once

#include <arbitration_graphs/behavior.hpp>

#include "environment_model.hpp"
#include "types.hpp"
#include "utils/cluster.hpp"

namespace demo {

/**
 * @brief The ChangeDotClusterBehavior makes pacman move towards the closest dot cluster
 * that he is not currently inside of.
 */
class ChangeDotClusterBehavior : public arbitration_graphs::Behavior<Command> {
public:
    using Ptr = std::shared_ptr<ChangeDotClusterBehavior>;
    using ConstPtr = std::shared_ptr<const ChangeDotClusterBehavior>;

    using Cluster = utils::Cluster;
    using Clusters = utils::DotClusterFinder::Clusters;

    explicit ChangeDotClusterBehavior(EnvironmentModel::Ptr environmentModel,
                                      const std::string& name = "ChangeDotClusterBehavior")
            : Behavior(name), environmentModel_{std::move(environmentModel)} {
    }

    Command getCommand(const Time& /*time*/) override {
        std::optional<Path> pathToTargetClusterCenter = environmentModel_->pathTo(targetCluster_->center);

        if (!pathToTargetClusterCenter) {
            throw std::runtime_error("Failed to compute path to target cluster. Can not provide a sensible command.");
        }

        return Command{pathToTargetClusterCenter.value()};
    }

    bool checkInvocationCondition(const Time& /*time*/) const override;

    bool checkCommitmentCondition(const Time& /*time*/) const override {
        Position pacmanPosition = environmentModel_->pacmanPosition();
        return !targetCluster_->isInCluster(pacmanPosition);
    }

    void gainControl(const Time& /*time*/) override {
        setTargetCluster();
    }
    void loseControl(const Time& /*time*/) override {
        targetCluster_.reset();
    }

private:
    void setTargetCluster();

    std::optional<Cluster> targetCluster_;
    EnvironmentModel::Ptr environmentModel_;
};

} // namespace demo
