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
    std::vector<Cluster> cluster = clusterFinder.findDotClusters();

    EXPECT_EQ(cluster.size(), 2);
    EXPECT_EQ(cluster.front().dots.size(), 3);
    EXPECT_EQ(cluster.back().dots.size(), 2);
}

} // namespace utils::a_star
