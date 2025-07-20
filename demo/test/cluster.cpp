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

namespace {
bool clusterExists(const std::vector<Cluster>& clusters, const ExpectedCluster& expectedCluster) {
    return std::any_of(
        clusters.begin(), clusters.end(), [&](const Cluster& cluster) { return expectedCluster.matches(cluster); });
}
} // namespace

class ClusterTest : public ::testing::Test {
protected:
    MockEnvironmentModel environmentModel;
};

TEST_F(ClusterTest, dotClusters) {
    environmentModel.setMaze({5, 5},
                             "#####"
                             "#o..#"
                             "     "
                             "#.. #"
                             "#####");

    DotClusterFinder dotClusterFinder(environmentModel.maze());
    std::vector<Cluster> clusters = dotClusterFinder.clusters();
    ASSERT_EQ(clusters.size(), 2);

    ExpectedCluster firstExpectedCluster{3, {2, 1}};
    ExpectedCluster secondExpectedCluster{2, {1, 3}};

    EXPECT_TRUE(clusterExists(clusters, firstExpectedCluster));
    EXPECT_TRUE(clusterExists(clusters, secondExpectedCluster));
}

} // namespace utils::a_star
