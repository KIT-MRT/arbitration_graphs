#pragma once

#include <arbitration_graphs/behavior.hpp>

#include "environment_model.hpp"
#include "types.hpp"
#include "utils/cluster.hpp"

namespace demo {

/**
 * @brief The ChangeDotClusterBehavior makes Pac-Man move towards the closest dot cluster
 * that he is not currently inside of.
 */
class ChangeDotClusterBehavior : public arbitration_graphs::Behavior<EnvironmentModel, Command> {
public:
    using Ptr = std::shared_ptr<ChangeDotClusterBehavior>;
    using ConstPtr = std::shared_ptr<const ChangeDotClusterBehavior>;

    using Cluster = utils::Cluster;
    using Clusters = utils::DotClusterFinder::Clusters;

    explicit ChangeDotClusterBehavior(const std::string& name = "ChangeDotCluster") : Behavior(name) {
    }

    ChangeDotClusterBehavior(const ChangeDotClusterBehavior&) = default;
    ChangeDotClusterBehavior(ChangeDotClusterBehavior&&) = default;
    ChangeDotClusterBehavior& operator=(const ChangeDotClusterBehavior&) = default;
    ChangeDotClusterBehavior& operator=(ChangeDotClusterBehavior&&) = default;
    virtual ~ChangeDotClusterBehavior() = default;

    Command getCommand(const Time& /*time*/, const EnvironmentModel& environmentModel) override;

    bool checkInvocationCondition(const Time& /*time*/, const EnvironmentModel& environmentModel) const override;
    bool checkCommitmentCondition(const Time& /*time*/, const EnvironmentModel& environmentModel) const override;

    void gainControl(const Time& /*time*/, const EnvironmentModel& environmentModel) override {
        setTargetCluster(environmentModel);
    }
    void loseControl(const Time& /*time*/, const EnvironmentModel& /*environmentModel*/) override {
        targetCluster_.reset();
    }

private:
    void setTargetCluster(const EnvironmentModel& environmentModel);

    std::optional<Cluster> targetCluster_;
};

} // namespace demo
