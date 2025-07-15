#include "utils/astar.hpp"

#include <ctime>

#include <gtest/gtest.h>

#include "mock_environment_model.hpp"

namespace utils::a_star {

using namespace demo;

class AStarTest : public ::testing::Test {
protected:
    MockEnvironmentModel environmentModel_{};
};

TEST_F(AStarTest, simpleDistance) {
    AStar astar(environmentModel_.maze());

    EXPECT_EQ(astar.mazeDistance({1, 1}, {1, 1}), 0);
    EXPECT_EQ(astar.mazeDistance({1, 1}, {2, 2}), 2);
    EXPECT_EQ(astar.mazeDistance({5, 1}, {1, 6}), 9);
}

TEST_F(AStarTest, distanceWithWalls) {
    const char str[] = {"#####"
                        "#   #"
                        "# # #"
                        "#   #"
                        "#####"};
    environmentModel_.setMaze({5, 5}, str);

    AStar astar(environmentModel_.maze());
    EXPECT_EQ(astar.mazeDistance({1, 1}, {3, 3}), 4);
    EXPECT_EQ(astar.mazeDistance({1, 2}, {3, 2}), 4);
}

TEST_F(AStarTest, distanceWithHorizontalTunnel) {
    const char str[] = {"#####"
                        "#   #"
                        "     "
                        "#   #"
                        "#####"};
    environmentModel_.setMaze({5, 5}, str);

    AStar astar(environmentModel_.maze());
    EXPECT_EQ(astar.mazeDistance({0, 2}, {4, 2}), 1);
    EXPECT_EQ(astar.mazeDistance({4, 2}, {0, 2}), 1);
    EXPECT_EQ(astar.mazeDistance({1, 1}, {3, 3}), 4);
    EXPECT_EQ(astar.mazeDistance({1, 3}, {4, 2}), 3);
}

TEST_F(AStarTest, distanceWithVerticalTunnel) {
    const char str[] = {"## ##"
                        "#   #"
                        "#   #"
                        "#   #"
                        "## ##"};
    environmentModel_.setMaze({5, 5}, str);

    AStar astar(environmentModel_.maze());
    EXPECT_EQ(astar.mazeDistance({2, 0}, {2, 4}), 1);
    EXPECT_EQ(astar.mazeDistance({2, 4}, {2, 0}), 1);
    EXPECT_EQ(astar.mazeDistance({1, 1}, {3, 3}), 4);
    EXPECT_EQ(astar.mazeDistance({3, 1}, {2, 4}), 3);
}

TEST_F(AStarTest, cachedDistance) {
    AStar astar(environmentModel_.maze());

    // In the first run, we need to compute distance the hard way
    std::clock_t startWithoutCaching = std::clock();
    EXPECT_EQ(astar.mazeDistance({1, 1}, {8, 8}), 14);
    std::clock_t endWithoutCaching = std::clock();

    // In the second run, the cached distance can be returned which should of course be the same value...
    std::clock_t startWithCaching = std::clock();
    EXPECT_EQ(astar.mazeDistance({1, 1}, {8, 8}), 14);
    std::clock_t endWithCaching = std::clock();

    // ...but we should get it faster
    EXPECT_LT((endWithCaching - startWithCaching), (endWithoutCaching - startWithoutCaching));
}

TEST_F(AStarTest, path) {
    const char str[] = {"#####"
                        "#   #"
                        "# ###"
                        "#   #"
                        "#####"};
    environmentModel_.setMaze({5, 5}, str);

    AStar astar(environmentModel_.maze());
    std::optional<Path> path = astar.shortestPath({2, 1}, {3, 3});
    Path targetPath = {Direction::LEFT, Direction::DOWN, Direction::DOWN, Direction::RIGHT, Direction::RIGHT};
    ASSERT_TRUE(path.has_value());
    ASSERT_EQ(path->size(), targetPath.size());
    for (int i = 0; i < targetPath.size(); i++) {
        EXPECT_EQ(path->at(i), targetPath.at(i));
    }
}

TEST_F(AStarTest, pathWithTunnel) {
    const char str[] = {"#####"
                        "#   #"
                        "     "
                        "#   #"
                        "#####"};
    environmentModel_.setMaze({5, 5}, str);

    AStar astar(environmentModel_.maze());
    std::optional<Path> path = astar.shortestPath({0, 2}, {4, 2});
    ASSERT_TRUE(path.has_value());
    ASSERT_EQ(path->size(), 1);
    EXPECT_EQ(path->front(), demo::Direction::LEFT);

    path = astar.shortestPath({4, 2}, {0, 2});
    ASSERT_TRUE(path.has_value());
    ASSERT_EQ(path->size(), 1);
    EXPECT_EQ(path->front(), demo::Direction::RIGHT);
}

TEST_F(AStarTest, pathToClosestDot) {
    const char str[] = {"#####"
                        "#  .#"
                        "# ###"
                        "#   #"
                        "#  .#"
                        "#####"};
    environmentModel_.setMaze({5, 6}, str);

    AStar astar(environmentModel_.maze());
    std::optional<Path> path = astar.pathToClosestDot({1, 2});
    ASSERT_TRUE(path.has_value());

    Path targetPath = {Direction::UP, Direction::RIGHT, Direction::RIGHT};
    ASSERT_EQ(path->size(), targetPath.size());
    for (int i = 0; i < targetPath.size(); i++) {
        EXPECT_EQ(path->at(i), targetPath.at(i));
    }

    path = astar.pathToClosestDot({1, 3});
    ASSERT_TRUE(path.has_value());
    ASSERT_EQ(path->size(), 3);
}

} // namespace utils::a_star
