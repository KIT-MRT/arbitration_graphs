#include "demo/astar.hpp"

#include <gtest/gtest.h>

#include "mock_environment_model.hpp"

namespace demo {

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

} // namespace demo
