#pragma once

#include <pacman/core/maze.hpp>

#include "demo/environment_model.hpp"

namespace demo {

class MockEnvironmentModel : public EnvironmentModel {
public:
    using Ptr = std::shared_ptr<MockEnvironmentModel>;
    using ConstPtr = std::shared_ptr<const MockEnvironmentModel>;

    MockEnvironmentModel() : EnvironmentModel() {
        setPositionsInOppositeCorners();
        setEmptyMaze();
    }

    PositionStore positions() const {
        return entityPositions_;
    }
    void setPositions(const PositionStore& positions) {
        entityPositions_ = positions;
    }
    void setPositionsInOppositeCorners() {
        entityPositions_.pacman = {1, 1};
        entityPositions_.blinky = {8, 8};
        entityPositions_.pinky = {8, 8};
        entityPositions_.inky = {8, 8};
        entityPositions_.clyde = {8, 8};
    }

    MazeState maze() const {
        return mazeState_;
    }
    template <std::size_t Size>
    void setMaze(const Position& size, const char (&str)[Size]) {
        mazeState_ = makeCustomMazeState({size.x, size.y}, str);
    }
    void setEmptyMaze() {
        const char str[] = {"##########"
                            "#        #"
                            "#        #"
                            "#        #"
                            "#        #"
                            "#        #"
                            "#        #"
                            "#        #"
                            "#        #"
                            "##########"};
        setMaze({10, 10}, str);
    }
};

} // namespace demo
