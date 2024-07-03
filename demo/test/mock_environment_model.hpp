#pragma once

#include "demo/environment_model.hpp"
#include "demo/types.hpp"
#include "utils/maze.hpp"

namespace demo {

class MockEnvironmentModel : public EnvironmentModel {
public:
    using Ptr = std::shared_ptr<MockEnvironmentModel>;
    using ConstPtr = std::shared_ptr<const MockEnvironmentModel>;

    MockEnvironmentModel() : EnvironmentModel(dummyGame_) {
        setPositionsInOppositeCorners();
        setEmptyMaze();
    }

    Entities entities() const {
        return entities_;
    }
    void setEntities(const Entities& entities) {
        entities_ = entities;
    }
    void setPacmanPosition(const Position& position) {
        entities_.pacman.position = position;
    }
    void setGhostPositions(const Position& position) {
        entities_.blinky.position = position;
        entities_.pinky.position = position;
        entities_.inky.position = position;
        entities_.clyde.position = position;
    }
    void setPositionsInOppositeCorners() {
        entities_.pacman.position = {1, 1};
        setGhostPositions({8, 8});
    }

    Maze::ConstPtr maze() const {
        return maze_;
    }
    template <std::size_t Size>
    void setMaze(const Position& size, const char (&str)[Size]) {
        maze_ = std::make_shared<Maze>(makeCustomMazeState({size.x, size.y}, str));
        astar_ = utils::AStar(maze_);
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
