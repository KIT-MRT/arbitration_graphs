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
    EXPECT_EQ(clusters.front().dots.size(), 3);
    EXPECT_EQ(clusters.front().center.x, 2);
    EXPECT_EQ(clusters.front().center.y, 1);

    EXPECT_EQ(clusters.back().dots.size(), 2);
    EXPECT_EQ(clusters.back().center.x, 1);
    EXPECT_EQ(clusters.back().center.y, 3);
}

} // namespace utils::a_star
