#pragma once

#include <pacman/comp/chase_target.hpp>
#include <pacman/comp/player.hpp>
#include <pacman/core/maze.hpp>

#include "astar.hpp"
#include "types.hpp"

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

    /**
     * @brief Update the environment model to reflect the current state of the world.
     *
     * @param registry The EnTT registry containing the current state of the world.
     * @param mazeState The current state of the maze.
     */
    void update(entt::registry& registry, const entt::MazeState& mazeState) {
        updatePositions(registry);
        mazeState_ = mazeState;
        astar_ = AStar(mazeState_);
    }

    Position pacmanPosition() const {
        return entityPositions_.pacman;
    }
    Position closestGhostPosition() const;

    /**
     * @brief Calculates the Manhattan distance between two positions using A*.
     *
     * A distance of 1 is the distance between two adjacent positions in the maze.
     * Will consider walls when calculating the distance.
     */
    int distance(const Position& start, const Position& goal) const {
        return astar_.distance(start, goal);
    }
    double closestGhostDistance() const {
        return distance(pacmanPosition(), closestGhostPosition());
    }

    bool isWall(const Position& position) const {
        return mazeState_[{position.x, position.y}] == Tile::wall;
    }

protected:
    void updatePositions(entt::registry& registry);

    PositionStore entityPositions_;
    entt::MazeState mazeState_;
    AStar astar_;
};

} // namespace demo
