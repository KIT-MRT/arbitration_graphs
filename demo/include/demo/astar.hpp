#pragma once

#include <limits>
#include <memory>
#include <optional>
#include <queue>
#include <vector>

#include <util_caching/cache.hpp>
#include <pacman/util/grid.hpp>

#include "types.hpp"

namespace demo {

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
    using MazeStateConstPtr = std::shared_ptr<const entt::MazeState>;

    MazeAdapter(const MazeStateConstPtr& mazeState)
            : mazeState_(mazeState), cells_({mazeState_->width(), mazeState_->height()}) {};

    Cell& cell(const Position& position) const {
        if (!cells_[{position.x, position.y}]) {
            cells_[{position.x, position.y}] =
                Cell(position, mazeState_->operator[]({position.x, position.y}) == Tile::wall);
        }

        return cells_[{position.x, position.y}].value();
    }

    int width() const {
        return mazeState_->width();
    }
    int height() const {
        return mazeState_->height();
    }
    bool isPassableCell(const Position& position) const {
        return isInBounds(position) && !cell(position).isWall;
    }
    bool isInBounds(const Position& position) const {
        return position.x >= 0 && position.x < width() && position.y >= 0 && position.y < height();
    }

private:
    const MazeStateConstPtr mazeState_;
    mutable Grid<std::optional<Cell>> cells_;
};

class AStar {
public:
    using Set = std::priority_queue<Cell, std::vector<Cell>, Cell::CompareCells>;

    constexpr static int NO_PATH_FOUND = std::numeric_limits<int>::max();

    AStar(const entt::MazeState mazeState) : mazeState_(std::make_shared<const entt::MazeState>(mazeState)) {};

    /**
     * @brief Calculates the Manhattan distance between two positions using A*.
     *
     * A distance of 1 is the distance between two adjacent positions in the maze.
     * Will consider walls when calculating the distance.
     */
    int distance(const Position& start, const Position& goal) const;

private:
    void expandCell(Set& openSet, MazeAdapter& mazeAdapter, const Position& goal) const;
    mutable util_caching::Cache<std::pair<Position, Position>, int> distanceCache;

    MazeAdapter::MazeStateConstPtr mazeState_;
};

} // namespace demo
