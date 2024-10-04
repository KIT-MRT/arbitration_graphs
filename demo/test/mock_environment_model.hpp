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
        initializeEntitiesInOppositeCorners();
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
    void setPacmanDirection(const Direction& direction) {
        entities_.pacman.direction = direction;
    }
    void setGhostPositions(const Position& position) {
        entities_.blinky.position = position;
        entities_.pinky.position = position;
        entities_.inky.position = position;
        entities_.clyde.position = position;
    }
    void setGhostDirections(const Direction& direction) {
        entities_.blinky.direction = direction;
        entities_.pinky.direction = direction;
        entities_.inky.direction = direction;
        entities_.clyde.direction = direction;
    }
    void setGhostMode(const GhostMode& mode) {
        entities_.blinky.mode = mode;
        entities_.pinky.mode = mode;
        entities_.inky.mode = mode;
        entities_.clyde.mode = mode;
    }
    void setScaredCountdown(const std::optional<int> countdown) {
        entities_.blinky.scaredCountdown = countdown;
        entities_.pinky.scaredCountdown = countdown;
        entities_.inky.scaredCountdown = countdown;
        entities_.clyde.scaredCountdown = countdown;
    }

    void initializeEntitiesInOppositeCorners() {
        setPacmanPosition({1, 1});
        setPacmanDirection(Direction::RIGHT);
        setGhostPositions({8, 8});
        setGhostDirections(Direction::LEFT);
    }

    Maze::ConstPtr maze() const {
        return maze_;
    }
    template <std::size_t Size>
    void setMaze(const Position& size, const char (&str)[Size]) {
        maze_ = std::make_shared<Maze>(makeCustomMazeState({size.x, size.y}, str));
        astar_ = utils::AStar(maze_);
        clusterFinder_ = utils::ClusterFinder(maze_);
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
