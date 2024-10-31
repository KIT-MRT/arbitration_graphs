#include "demo/change_dot_cluster_behavior.hpp"

namespace demo {

Command ChangeDotClusterBehavior::getCommand(const Time& /*time*/) {
    std::optional<Path> pathToTargetClusterCenter = environmentModel_->pathTo(targetCluster_->center);

    if (!pathToTargetClusterCenter) {
        throw std::runtime_error("Failed to compute path to target cluster. Can not provide a sensible command.");
    }

    return Command{pathToTargetClusterCenter.value()};
}

bool ChangeDotClusterBehavior::checkInvocationCondition(const Time& /*time*/) const {
    auto pacmanPosition = environmentModel_->pacmanPosition();
    Clusters clusters = environmentModel_->dotCluster();

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

bool ChangeDotClusterBehavior::checkCommitmentCondition(const Time& /*time*/) const {
    Position pacmanPosition = environmentModel_->pacmanPosition();
    return !targetCluster_->isInCluster(pacmanPosition);
}

void ChangeDotClusterBehavior::setTargetCluster() {
    auto pacmanPosition = environmentModel_->pacmanPosition();
    Clusters clusters = environmentModel_->dotCluster();

    int minDistance = std::numeric_limits<int>::max();
    for (const auto& cluster : clusters) {
        int distance = environmentModel_->mazeDistance(pacmanPosition, cluster.center);
        if (distance < minDistance && !cluster.isInCluster(pacmanPosition)) {
            minDistance = distance;
            targetCluster_ = cluster;
        }
    }
}

} // namespace demo
