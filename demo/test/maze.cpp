#include "utils/maze.hpp"

#include <gtest/gtest.h>

namespace utils::a_star {

using namespace demo;

TEST(MazeState, positionConsideringTunnel) {
    const char str[] = {"### #"
                        "#    "
                        "### #"};
    Maze maze{makeCustomMazeState({5, 3}, str)};

    // Regular position -> no wrapping
    EXPECT_EQ(maze.positionConsideringTunnel({1, 1}).x, 1);
    EXPECT_EQ(maze.positionConsideringTunnel({1, 1}).y, 1);

    // Regular position -> no wrapping
    EXPECT_EQ(maze.positionConsideringTunnel({4, 1}).x, 4);
    EXPECT_EQ(maze.positionConsideringTunnel({4, 1}).y, 1);

    // Position out of maze, without tunnel -> clamping
    EXPECT_EQ(maze.positionConsideringTunnel({5, 1}).x, 4);
    EXPECT_EQ(maze.positionConsideringTunnel({5, 1}).y, 1);

    // Regular position -> no wrapping
    EXPECT_EQ(maze.positionConsideringTunnel({3, 0}).x, 3);
    EXPECT_EQ(maze.positionConsideringTunnel({3, 0}).y, 0);

    // Position out of maze, with tunnel -> wrapping
    EXPECT_EQ(maze.positionConsideringTunnel({3, -1}).x, 3);
    EXPECT_EQ(maze.positionConsideringTunnel({3, -1}).y, 2);
}


} // namespace utils::a_star