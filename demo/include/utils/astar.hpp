#pragma once

#include <limits>
#include <optional>
#include <queue>
#include <utility>
#include <vector>

#include <util_caching/cache.hpp>

#include "demo/types.hpp"
#include "utils/maze.hpp"

namespace utils {

using Path = demo::Path;
using Position = demo::Position;
using TileType = demo::TileType;

struct AStarCell : public BaseCell {
    using Move = demo::Move;

    struct CompareCells {
        bool operator()(const AStarCell& left, const AStarCell& right) {
            return (left.distanceFromStart + left.heuristic) > (right.distanceFromStart + right.heuristic);
        }
    };

    AStarCell(const Position& position, const TileType& type) : BaseCell(position, type) {};

    bool visited{false};
    int distanceFromStart{std::numeric_limits<int>::max()};
    double heuristic{std::numeric_limits<int>::max()};
    std::optional<Move> moveFromPredecessor;

    double manhattanDistance(const Position& other) const {
        return std::abs(position.x - other.x) + std::abs(position.y - other.y);
    }
};

class AStar {
public:
    using AStarMazeAdapter = MazeAdapter<AStarCell>;
    using Cell = AStarCell;
    using HeuristicFunction = std::function<int(const AStarCell&)>;
    using Set = std::priority_queue<AStarCell, std::vector<AStarCell>, AStarCell::CompareCells>;

    constexpr static int NoPathFound = std::numeric_limits<int>::max();

    explicit AStar(Maze::ConstPtr maze) : maze_{std::move(maze)} {};

    /**
     * @brief Calculates the Manhattan distance between two positions using A*.
     *
     * A distance of 1 is the distance between two adjacent positions in the maze.
     * Will consider walls when calculating the distance.
     */
    int mazeDistance(const Position& start, const Position& goal) const;

    /**
     * @brief Returns the shortest path from the start to the goal position considering the maze geometry.
     */
    std::optional<Path> shortestPath(const Position& start, const Position& goal) const;

    /**
     * @brief Returns the path from a given start position to the closest dot.
     *
     * If there is no dot or no path can be found, std::nullopt will be returned.
     */
    std::optional<Path> pathToClosestDot(const Position& start) const;

    void updateMaze(const Maze::ConstPtr& maze) {
        maze_ = maze;
    }

private:
    void expandCell(Set& openSet, AStarMazeAdapter& mazeAdapter, const HeuristicFunction& heuristic) const;

    /**
     * @brief Create a path by traversing predecessor relationships up to a goal position.
     *
     * Will expand the path backwards until no more predecessor relationship is available. If the cell at the goal
     * position does not have a predecessor, the path will be empty.
     */
    Path extractPathTo(const AStarMazeAdapter& maze, const Position& goal) const;

    /**
     * @brief Approximates the distance of a given cell to a goal while considering a shortcut via the tunnel.
     *
     * Always underestimate the actual distance making it suitable for an A* heuristic
     * The first term is the direct manhattan distance.
     * The second term approximates the distance through the tunnel.
     */
    double optimisticDistanceToGoal(const Cell& cell, const Position& goal) const {
        return std::min(cell.manhattanDistance(goal), maze_->width() - cell.manhattanDistance(goal));
    }

    Maze::ConstPtr maze_;
    mutable util_caching::Cache<std::pair<Position, Position>, int> distanceCache_;
};

} // namespace utils
