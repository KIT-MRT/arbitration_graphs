#pragma once

#include <limits>
#include <queue>
#include <vector>

#include "types.hpp"
#include "util/grid.hpp"

namespace demo {

class AStar {
public:
    struct Cell {
        Position position;
        int distanceFromStart{std::numeric_limits<int>::max()};
        double heuristic{std::numeric_limits<int>::max()};
        bool visited{false};
        bool isWall;

        double distance(const Position& other) const {
            return std::sqrt(std::pow(position.x - other.x, 2) + std::pow(position.y - other.y, 2));
        }
    };

    struct CompareCells {
        bool operator()(const Cell& c1, const Cell& c2) {
            return (c1.distanceFromStart + c1.heuristic) > (c2.distanceFromStart + c2.heuristic);
        }
    };

    using Set = std::priority_queue<Cell, std::vector<Cell>, CompareCells>;

    AStar() = default;
    AStar(const entt::MazeState& mazeState);

    int distance(const Position& start, const Position& goal) const;

private:
    void expandCell(Set& openSet, Grid<Cell>& cells, const Position& goal) const;
    bool isPassableCell(const Position& position) const {
        return isInBounds(position) && !cells_[{position.x, position.y}].isWall;
    }
    bool isInBounds(const Position& position) const {
        return position.x >= 0 && position.x < cells_.width() && position.y >= 0 && position.y < cells_.height();
    }

    Grid<Cell> cells_;
    bool initialized_{false};
};

} // namespace demo