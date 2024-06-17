#include "utils/astar.hpp"

#include <ctime>

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

TEST_F(AStarTest, distanceWithHorizontalTunnel) {
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

TEST_F(AStarTest, distanceWithVerticalTunnel) {
    const char str[] = {"## ##"
                        "#   #"
                        "#   #"
                        "#   #"
                        "## ##"};
    environmentModel_->setMaze({5, 5}, str);

    AStar astar(environmentModel_->maze());
    EXPECT_EQ(astar.distance({2, 0}, {2, 4}), 1);
    EXPECT_EQ(astar.distance({2, 4}, {2, 0}), 1);
    EXPECT_EQ(astar.distance({1, 1}, {3, 3}), 4);
    EXPECT_EQ(astar.distance({3, 1}, {2, 4}), 3);
}

TEST_F(AStarTest, cachedDistance) {
    AStar astar(environmentModel_->maze());

    // In the first run, we need to compute distance the hard way
    std::clock_t startWithoutCaching = std::clock();
    EXPECT_EQ(astar.distance({1, 1}, {8, 8}), 14);
    std::clock_t endWithoutCaching = std::clock();

    // In the second run, the cached distance can be returned which should of course be the same value...
    std::clock_t startWithCaching = std::clock();
    EXPECT_EQ(astar.distance({1, 1}, {8, 8}), 14);
    std::clock_t endWithCaching = std::clock();

    // ...but we should get it faster
    EXPECT_LT((endWithCaching - startWithCaching), (endWithoutCaching - startWithoutCaching));
}

} // namespace utils
