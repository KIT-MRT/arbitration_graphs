#pragma once

#include <util_caching/cache.hpp>
#include <pacman/comp/chase_target.hpp>
#include <pacman/comp/player.hpp>

#include "types.hpp"
#include "utils/astar.hpp"
#include "utils/entities.hpp"
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
    using Entities = utils::Entities;
    using Maze = utils::Maze;
    using Ghost = utils::Ghost;
    using Ghosts = utils::Entities::Ghosts;

    using Ptr = std::shared_ptr<EnvironmentModel>;
    using ConstPtr = std::shared_ptr<const EnvironmentModel>;

    struct GhostWithDistance {
        Ghost ghost;
        int distance;
    };

    EnvironmentModel(const Game& game) : maze_(std::make_shared<Maze>(game.maze)), astar_(maze_) {
        updateEntities(game.reg);
    };

    /**
     * @brief Update the environment model to reflect the current state of the world.
     */
    void update(const Game& game) {
        maze_ = std::make_shared<Maze>(game.maze);
        updateEntities(game.reg);
    }

    Position pacmanPosition() const {
        return entities_.pacman.position;
    }
    /**
     * @brief The currently closest ghost and the corresponding manhattan distance.
     *
     * This function uses the A* distance function so walls will be considered.
     */
    GhostWithDistance closestGhost(const Time& time) const;

    /**
     * @brief The closest scared ghost and the corresponding and manhatten distance. Returns std::nullopt if there is no
     * scared ghost.
     *
     * Very similar to closestGhost() but only considering ghosts that are in the scared mode.
     */
    std::optional<GhostWithDistance> closestScaredGhost(const Time& time) const;

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
    void updateEntities(const entt::Registry& registry);

    GhostWithDistance closestGhost(const Ghosts& ghosts) const;

    Entities entities_;
    Maze::ConstPtr maze_;

    utils::AStar astar_;
    mutable util_caching::Cache<Time, GhostWithDistance> closestGhostCache_;
    mutable util_caching::Cache<Time, GhostWithDistance> closestScaredGhostCache_;
};

} // namespace demo
