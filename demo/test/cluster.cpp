#include "utils/cluster.hpp"

#include <gtest/gtest.h>

#include "mock_environment_model.hpp"

namespace utils::a_star {

using namespace demo;

struct ExpectedCluster {
    int expectedSize;
    Position center;

    bool matches(const Cluster& cluster) const {
        return cluster.dots.size() == expectedSize && cluster.center == center;
    }
};

bool clusterExists(const std::vector<Cluster>& clusters, const ExpectedCluster& expectedCluster) {
    return std::any_of(
        clusters.begin(), clusters.end(), [&](const Cluster& cluster) { return expectedCluster.matches(cluster); });
}

class ClusterTest : public ::testing::Test {
protected:
    ClusterTest() : environmentModel_(std::make_shared<MockEnvironmentModel>()) {
    }

    MockEnvironmentModel::Ptr environmentModel_;
};

TEST_F(ClusterTest, dotClusters) {
    const char str[] = {"#####"
                        "#o..#"
                        "     "
                        "#.. #"
                        "#####"};
    environmentModel_->setMaze({5, 5}, str);

    DotClusterFinder dotClusterFinder(environmentModel_->maze());
    std::vector<Cluster> clusters = dotClusterFinder.clusters();
    ASSERT_EQ(clusters.size(), 2);

    ExpectedCluster firstExpectedCluster{3, {2, 1}};
    ExpectedCluster secondExpectedCluster{2, {1, 3}};

    EXPECT_TRUE(clusterExists(clusters, firstExpectedCluster));
    EXPECT_TRUE(clusterExists(clusters, secondExpectedCluster));
}

} // namespace utils::a_star
