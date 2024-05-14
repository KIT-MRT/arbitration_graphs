#pragma once

#include <core/maze.hpp>

#include "demo/environment_model.hpp"

namespace demo {

class MockEnvironmentModel : public EnvironmentModel {
public:
    using Ptr = std::shared_ptr<MockEnvironmentModel>;
    using ConstPtr = std::shared_ptr<const MockEnvironmentModel>;

    MockEnvironmentModel() : EnvironmentModel() {
        setPositionsFarAway();
        setEmptyMaze();
    }

    PositionStore positions() const {
        return entityPositions_;
    }
    void setPositions(const PositionStore& positions) {
        entityPositions_ = positions;
    }

    void setPositionsFarAway() {
        entityPositions_.pacman = {100, 100};
        entityPositions_.blinky = {100, 100};
        entityPositions_.pinky = {100, 100};
        entityPositions_.inky = {100, 100};
        entityPositions_.clyde = {100, 100};
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
