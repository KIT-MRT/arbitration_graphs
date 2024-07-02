#pragma once

#include <util_caching/cache.hpp>
#include <pacman/comp/chase_target.hpp>
#include <pacman/comp/player.hpp>

#include "types.hpp"
#include "utils/astar.hpp"
#include "utils/maze.hpp"

namespace demo {

/**
 * @class EnvironmentModel
 * @brief Represents the environment model for a game.
 *
 * The EnvironmentModel class serves as a centralized location for individual behaviors to query the current state of
 * the world. */
class EnvironmentModel {
public:
    using Maze = utils::Maze;

    using Ptr = std::shared_ptr<EnvironmentModel>;
    using ConstPtr = std::shared_ptr<const EnvironmentModel>;

    struct PositionStore {
        Positions ghostPositions() const {
            return {blinky, pinky, inky, clyde};
        }

        Position pacman;
        Position blinky;
        Position pinky;
        Position inky;
        Position clyde;
    };

    EnvironmentModel(const Game& game) : maze_(std::make_shared<Maze>(game.maze)), astar_(maze_) {
        updatePositions(game.reg);
        updateGhostMode(game.reg);
    };

    /**
     * @brief Update the environment model to reflect the current state of the world.
     */
    void update(const Game& game) {
        maze_ = std::make_shared<Maze>(game.maze);
        updatePositions(game.reg);
        updateGhostMode(game.reg);
    }

    Position pacmanPosition() const {
        return entityPositions_.pacman;
    }
    /**
     * @brief The position and manhattan distance to the closest ghost.
     *
     * This function uses the A* distance function so walls will be considered.
     */
    PositionWithDistance closestGhost(const Time& time) const;

    bool ghostsScared() const {
        return ghostsScared_;
    }

    /**
     * @brief Calculates the Manhattan distance between two positions using A*.
     *
     * A distance of 1 is the distance between two adjacent positions in the maze.
     * Will consider walls when calculating the distance.
     */
    int distance(const Position& start, const Position& goal) const {
        return astar_.distance(start, goal);
    }

    bool isWall(const Position& position) const {
        return maze_->isWall(position);
    }

protected:
    void updatePositions(const entt::Registry& registry);
    void updateGhostMode(const entt::Registry& registry);

    PositionStore entityPositions_;
    Maze::ConstPtr maze_;
    bool ghostsScared_;

    utils::AStar astar_;
    mutable util_caching::Cache<Time, PositionWithDistance> closestGhostCache_;
};

} // namespace demo
