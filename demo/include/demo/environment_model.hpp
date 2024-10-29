#pragma once

#include <util_caching/cache.hpp>
#include <pacman/comp/chase_target.hpp>
#include <pacman/comp/player.hpp>

#include "types.hpp"
#include "utils/astar.hpp"
#include "utils/cluster.hpp"
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
    using Cluster = utils::Cluster;
    using Clusters = utils::DotClusterFinder::Clusters;
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

    explicit EnvironmentModel(const Game& game)
            : maze_{std::make_shared<Maze>(game.maze)}, astar_{maze_}, clusterFinder_{maze_} {
        updateEntities(game.reg);
    };

    /**
     * @brief Update the environment model to reflect the current state of the world.
     */
    void update(const Game& game) {
        maze_ = std::make_shared<Maze>(game.maze);
        astar_.updateMaze(maze_);
        clusterFinder_ = utils::DotClusterFinder{maze_};
        updateEntities(game.reg);
    }

    Position pacmanPosition() const {
        return entities_.pacman.position;
    }

    Direction pacmanDirection() const {
        return entities_.pacman.direction;
    }

    /**
     * @brief The currently closest ghost and the corresponding manhattan distance.
     *
     * This function uses the A* distance function so walls will be considered.
     */
    GhostWithDistance closestGhost(const Time& time) const;

    /**
     * @brief The closest scared ghost and the corresponding and manhattan distance.
     *        Returns std::nullopt if there is no scared ghost.
     *
     * Very similar to closestGhost() but only considering ghosts that are in the scared mode.
     */
    std::optional<GhostWithDistance> closestScaredGhost(const Time& time) const;

    /**
     * @brief Returns a vector of all dot clusters.
     *
     * A dot cluster is a set of dots (including power pellets) that can be connected by a path passing through neither
     * walls nor empty space.
     */
    Clusters dotCluster() const {
        return clusterFinder_.clusters();
    }

    /**
     * @brief Calculates the Manhattan distance between two positions using A* considering the maze geometry.
     *
     * A distance of 1 is the distance between two adjacent positions in the maze.
     */
    int mazeDistance(const Position& start, const Position& goal) const {
        return astar_.mazeDistance(start, goal);
    }

    std::optional<Path> pathTo(const Position& goal) {
        return astar_.shortestPath(pacmanPosition(), goal);
    }

    std::optional<Path> pathToClosestDot(const Position& position) const {
        return astar_.pathToClosestDot(position);
    }

    Positions toAbsolutePath(const Path& path) const;

    bool isDot(const Position& position) const {
        return maze_->isDot(position);
    }
    bool isWall(const Position& position) const {
        return maze_->isWall(position);
    }
    bool isInBounds(const Position& position) const {
        return maze_->isInBounds(position);
    }
    bool isPassableCell(const Position& position) const {
        return maze_->isPassableCell(position);
    }
    Position positionConsideringTunnel(const Position& position) const {
        return maze_->positionConsideringTunnel(position);
    }

protected:
    void updateEntities(const entt::Registry& registry);

    GhostWithDistance closestGhost(const Ghosts& ghosts) const;

    Entities entities_;
    Maze::ConstPtr maze_;

    utils::AStar astar_;
    utils::DotClusterFinder clusterFinder_;
    mutable util_caching::Cache<Time, GhostWithDistance> closestGhostCache_;
    mutable util_caching::Cache<Time, GhostWithDistance> closestScaredGhostCache_;
};

} // namespace demo
