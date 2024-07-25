#include "utils/cluster.hpp"

#include <gtest/gtest.h>

#include "mock_environment_model.hpp"

namespace utils::a_star {

using namespace demo;

class ClusterTest : public ::testing::Test {
protected:
    ClusterTest() : environmentModel_(std::make_shared<MockEnvironmentModel>()) {
    }

    MockEnvironmentModel::Ptr environmentModel_;
};

TEST_F(ClusterTest, findDotClusters) {
    const char str[] = {"#####"
                        "#...#"
                        "     "
                        "#.. #"
                        "#####"};
    environmentModel_->setMaze({5, 5}, str);

    ClusterFinder clusterFinder(environmentModel_->maze());
    std::vector<Cluster> clusters = clusterFinder.findDotClusters();

    EXPECT_EQ(clusters.size(), 2);
    EXPECT_EQ(clusters.front().dots.size(), 3);
    EXPECT_EQ(clusters.front().center.x, 2);
    EXPECT_EQ(clusters.front().center.y, 1);

    EXPECT_EQ(clusters.back().dots.size(), 2);
    // We are using std::floor when computing the cluster center so in this
    // case the center should be the left of two dots
    EXPECT_EQ(clusters.back().center.x, 1);
    EXPECT_EQ(clusters.back().center.y, 3);
}

} // namespace utils::a_star
