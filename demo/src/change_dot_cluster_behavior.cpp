#include "demo/change_dot_cluster_behavior.hpp"

namespace demo {

Command ChangeDotClusterBehavior::getCommand(const Time& /*time*/, const EnvironmentModel& environmentModel) {
    std::optional<Path> pathToTargetClusterCenter = environmentModel.pathTo(targetCluster_->center);

    if (!pathToTargetClusterCenter) {
        throw std::runtime_error("Failed to compute path to target cluster. Can not provide a sensible command.");
    }

    return Command{pathToTargetClusterCenter.value()};
}

bool ChangeDotClusterBehavior::checkInvocationCondition(const Time& /*time*/,
                                                        const EnvironmentModel& environmentModel) const {
    auto pacmanPosition = environmentModel.pacmanPosition();
    Clusters clusters = environmentModel.dotCluster();

    if (clusters.empty()) {
        // We cannot navigate to a cluster if there aren't any
        return false;
    }
    if (clusters.size() == 1 && clusters.front().isInCluster(pacmanPosition)) {
        // The only cluster left is the one we are already in
        return false;
    }

    return true;
}

bool ChangeDotClusterBehavior::checkCommitmentCondition(const Time& /*time*/,
                                                        const EnvironmentModel& environmentModel) const {
    Position pacmanPosition = environmentModel.pacmanPosition();
    return !targetCluster_->isInCluster(pacmanPosition);
}

void ChangeDotClusterBehavior::setTargetCluster(const EnvironmentModel& environmentModel) {
    auto pacmanPosition = environmentModel.pacmanPosition();
    Clusters clusters = environmentModel.dotCluster();

    int minDistance = std::numeric_limits<int>::max();
    for (const auto& cluster : clusters) {
        int distance = environmentModel.mazeDistance(pacmanPosition, cluster.center);
        if (distance < minDistance && !cluster.isInCluster(pacmanPosition)) {
            minDistance = distance;
            targetCluster_ = cluster;
        }
    }
}

} // namespace demo
