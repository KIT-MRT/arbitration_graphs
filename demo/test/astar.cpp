#include "utils/astar.hpp"

#include <gtest/gtest.h>

#include "mock_environment_model.hpp"

namespace utils {

using namespace demo;

class AStarTest : public ::testing::Test {
protected:
    AStarTest() : environmentModel_(std::make_shared<MockEnvironmentModel>()) {
    }

    MockEnvironmentModel::Ptr environmentModel_;
};

TEST_F(AStarTest, simpleDistance) {
    AStar astar(environmentModel_->maze());

    EXPECT_EQ(astar.distance({1, 1}, {1, 1}), 0);
    EXPECT_EQ(astar.distance({1, 1}, {2, 2}), 2);
    EXPECT_EQ(astar.distance({5, 1}, {1, 6}), 9);
}

TEST_F(AStarTest, distanceWithWalls) {
    const char str[] = {"#####"
                        "#   #"
                        "# # #"
                        "#   #"
                        "#####"};
    environmentModel_->setMaze({5, 5}, str);

    AStar astar(environmentModel_->maze());
    EXPECT_EQ(astar.distance({1, 1}, {3, 3}), 4);
    EXPECT_EQ(astar.distance({1, 2}, {3, 2}), 4);
}

TEST_F(AStarTest, distanceWithTunnel) {
    const char str[] = {"#####"
                        "#   #"
                        "     "
                        "#   #"
                        "#####"};
    environmentModel_->setMaze({5, 5}, str);

    AStar astar(environmentModel_->maze());
    EXPECT_EQ(astar.distance({0, 2}, {4, 2}), 1);
    EXPECT_EQ(astar.distance({4, 2}, {0, 2}), 1);
    EXPECT_EQ(astar.distance({1, 1}, {3, 3}), 4);
    EXPECT_EQ(astar.distance({1, 3}, {4, 2}), 3);
}

TEST_F(AStarTest, cachedDistance) {
    AStar astar(environmentModel_->maze());

    // In the first run, we need to compute distance the hard way
    Time startWithoutCaching = Clock::now();
    EXPECT_EQ(astar.distance({1, 1}, {8, 8}), 14);
    Time endWithoutCaching = Clock::now();

    // In the second run, the cached distance can be returned which should of course be the same value...
    Time startWithCaching = Clock::now();
    EXPECT_EQ(astar.distance({1, 1}, {8, 8}), 14);
    Time endWithCaching = Clock::now();

    // ...but we should get it faster
    EXPECT_LT((endWithCaching - startWithCaching).count(), (endWithoutCaching - startWithoutCaching).count());
}

} // namespace utils
