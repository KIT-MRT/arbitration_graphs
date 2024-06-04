#pragma once

#include <util_caching/cache.hpp>
#include <pacman/comp/chase_target.hpp>
#include <pacman/comp/player.hpp>
#include <pacman/core/maze.hpp>

#include "types.hpp"
#include "utils/astar.hpp"

namespace demo {

/**
 * @class EnvironmentModel
 * @brief Represents the environment model for a game.
 *
 * The EnvironmentModel class serves as a centralized location for individual behaviors to query the current state of
 * the world. */
class EnvironmentModel {
public:
    using Ptr = std::shared_ptr<EnvironmentModel>;
    using ConstPtr = std::shared_ptr<const EnvironmentModel>;

    struct PoseStore {
        Positions ghostPositions() const {
            return {blinky.position, pinky.position, inky.position, clyde.position};
        }

        Pose pacman;
        Pose blinky;
        Pose pinky;
        Pose inky;
        Pose clyde;
    };

    EnvironmentModel(const Game& game) : mazeState_(game.maze), astar_(game.maze) {
        updatePoses(game.reg);
    };

    /**
     * @brief Update the environment model to reflect the current state of the world.
     */
    void update(const Game& game) {
        mazeState_ = game.maze;
        updatePoses(game.reg);
    }

    Position pacmanPosition() const {
        return entityPoses_.pacman.position;
    }
    /**
     * @brief The position and manhattan distance to the closest ghost.
     *
     * This function uses the A* distance function so walls will be considered.
     */
    PositionWithDistance closestGhost(const Time& time) const;

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
        return mazeState_[{position.x, position.y}] == Tile::wall;
    }

protected:
    void updatePoses(const entt::registry& registry);

    PoseStore entityPoses_;
    entt::MazeState mazeState_;

    utils::AStar astar_;
    mutable util_caching::Cache<Time, PositionWithDistance> closestGhostCache_;
};

} // namespace demo
