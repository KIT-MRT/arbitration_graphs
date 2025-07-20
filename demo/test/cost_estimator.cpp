#include "demo/cost_estimator.hpp"

#include <gtest/gtest.h>

#include "mock_environment_model.hpp"

namespace demo {

class CostEstimatorTest : public ::testing::Test {
protected:
    CostEstimatorTest() : costEstimator{parameters} {
        setMaze();
        environmentModel.setGhostPositions({1, 1});
    }

    void setMaze() {
        environmentModel.setMaze({10, 10},
                                 "##########"
                                 "#        #"
                                 "#        #"
                                 "#        #"
                                 "#        #"
                                 "#.       #"
                                 "#.       #"
                                 "#...     #"
                                 "#...     #"
                                 "##########");
    }

    // NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes)
    Time time = Clock::now();

    MockEnvironmentModel environmentModel;
    CostEstimator::Parameters parameters{1};

    CostEstimator costEstimator;
    // NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes)
};

TEST_F(CostEstimatorTest, estimateCostDownPath) {
    // Move from the top left to the bottom left corner
    environmentModel.setPacmanPosition({1, 1});
    using D = Direction;
    Path path{D::Down, D::Down, D::Down, D::Down, D::Down, D::Down, D::Down};

    // Number of cells along the path and within the neighborhood search area
    int numOfCells = 7 + 9;

    // Number of dots along the path and within the neighborhood search area
    int numOfDots = 4 + 4;

    double expectedCost = static_cast<double>(numOfCells) / numOfDots;
    double actualCost = costEstimator.estimateCost(time, environmentModel, Command{path}, true);

    EXPECT_EQ(expectedCost, actualCost);
}

TEST_F(CostEstimatorTest, estimateCostUpPath) {
    // Move from the bottom left to the top left corner
    environmentModel.setPacmanPosition({1, 8});
    using D = Direction;
    Path path{D::Up, D::Up, D::Up, D::Up, D::Up, D::Up, D::Up};

    // Number of cells along the path and within the neighborhood search area
    int numOfCells = 7 + 9;

    // Number of dots along the path and within the neighborhood search area
    // Note: The dot on the current position is not counted - it would have already been eaten in a "real-life" scenario
    int numOfDots = 3 + 0;

    double expectedCost = static_cast<double>(numOfCells) / numOfDots;
    double actualCost = costEstimator.estimateCost(time, environmentModel, Command{path}, true);

    EXPECT_EQ(expectedCost, actualCost);
}

TEST_F(CostEstimatorTest, estimateCostNoDots) {
    // Move from the top left to the top right corner
    environmentModel.setPacmanPosition({1, 1});
    using D = Direction;
    Path path{D::Right, D::Right, D::Right, D::Right, D::Right, D::Right, D::Right};

    // There are neither points along the path nor in the neighborhood search area
    double expectedCost = std::numeric_limits<double>::max();
    double actualCost = costEstimator.estimateCost(time, environmentModel, Command{path}, true);

    EXPECT_EQ(expectedCost, actualCost);
}

} // namespace demo
