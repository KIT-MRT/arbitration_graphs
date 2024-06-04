#pragma once

#include <pacman/core/maze.hpp>

#include "demo/environment_model.hpp"
#include "demo/types.hpp"

namespace demo {

class MockEnvironmentModel : public EnvironmentModel {
public:
    using Ptr = std::shared_ptr<MockEnvironmentModel>;
    using ConstPtr = std::shared_ptr<const MockEnvironmentModel>;

    MockEnvironmentModel() : EnvironmentModel(dummyGame_) {
        setPositionsInOppositeCorners();
        setEmptyMaze();
    }

    PoseStore poses() const {
        return entityPoses_;
    }
    void setPoses(const PoseStore& poses) {
        entityPoses_ = poses;
    }
    void setPositionsInOppositeCorners() {
        entityPoses_.pacman.position = {1, 1};
        entityPoses_.blinky.position = {8, 8};
        entityPoses_.pinky.position = {8, 8};
        entityPoses_.inky.position = {8, 8};
        entityPoses_.clyde.position = {8, 8};
    }

    entt::MazeState maze() const {
        return mazeState_;
    }
    template <std::size_t Size>
    void setMaze(const Position& size, const char (&str)[Size]) {
        mazeState_ = makeCustomMazeState({size.x, size.y}, str);
        astar_ = utils::AStar(mazeState_);
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

private:
    entt::Game dummyGame_; // We just need this to initialize the base environment model
};

} // namespace demo
