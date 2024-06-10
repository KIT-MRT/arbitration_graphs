#pragma once

#include <limits>
#include <memory>
#include <optional>
#include <queue>
#include <vector>

#include <comp/position.hpp>
#include <util_caching/cache.hpp>
#include <pacman/util/grid.hpp>

#include "demo/types.hpp"
#include "utils/maze.hpp"

namespace utils {

using Position = demo::Position;

struct Cell {
    struct CompareCells {
        bool operator()(const Cell& c1, const Cell& c2) {
            return (c1.distanceFromStart + c1.heuristic) > (c2.distanceFromStart + c2.heuristic);
        }
    };

    Cell(const Position& position, const bool& isWall) : position(position), isWall(isWall) {};

    Position position;
    int distanceFromStart{std::numeric_limits<int>::max()};
    double heuristic{std::numeric_limits<int>::max()};
    bool visited{false};
    bool isWall;

    double distance(const Position& other) const {
        return std::sqrt(std::pow(position.x - other.x, 2) + std::pow(position.y - other.y, 2));
    }
};

class MazeAdapter {
public:
    using MazeStateConstPtr = std::shared_ptr<const MazeState>;

    MazeAdapter(const Maze::ConstPtr& maze) : maze_(maze), cells_({maze_->width(), maze_->height()}) {};

    Cell& cell(const Position& position) const {
        if (!cells_[{position.x, position.y}]) {
            cells_[{position.x, position.y}] = Cell(position, maze_->isWall(position));
        }

        return cells_[{position.x, position.y}].value();
    }

private:
    Maze::ConstPtr maze_;
    mutable Grid<std::optional<Cell>> cells_;
};

class AStar {
public:
    using Set = std::priority_queue<Cell, std::vector<Cell>, Cell::CompareCells>;

    constexpr static int NO_PATH_FOUND = std::numeric_limits<int>::max();

    AStar(const Maze::ConstPtr maze) : maze_(maze) {};

    /**
     * @brief Calculates the Manhattan distance between two positions using A*.
     *
     * A distance of 1 is the distance between two adjacent positions in the maze.
     * Will consider walls when calculating the distance.
     */
    int distance(const Position& start, const Position& goal) const;

private:
    void expandCell(Set& openSet, MazeAdapter& mazeAdapter, const Position& goal) const;

    /**
     * @brief If we are about to step of the maze and both the left and right end of the cell are passable,
     * we assume they are connected by a tunnel and adjust the x position accordingly.
     */
    Position positionConsideringTunnel(const Position& position) const;
    bool enteringTunnelOnTheLeft(const Position& position) const {
        return position.x == -1 && maze_->isPassableCell({maze_->width() - 1, position.y});
    }
    bool enteringTunnelOnTheRight(const Position& position) const {
        return position.x == maze_->width() && maze_->isPassableCell({0, position.y});
    }

    Maze::ConstPtr maze_;
    mutable util_caching::Cache<std::pair<Position, Position>, int> distanceCache_;
};

} // namespace utils
