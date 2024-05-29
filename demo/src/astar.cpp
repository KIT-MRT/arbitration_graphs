#include "demo/astar.hpp"
#include "demo/types.hpp"

namespace demo {

AStar::AStar(const entt::MazeState& mazeState) {
    cells_ = Grid<Cell>{{mazeState.width(), mazeState.height()}};
    for (int y = 0; y < mazeState.height(); y++) {
        for (int x = 0; x < mazeState.width(); x++) {
            Cell& cell = cells_[{x, y}];
            cell.position = {x, y};
            cell.isWall = mazeState[{x, y}] == Tile::wall;
            cell.visited = false;
        }
    }
    initialized_ = true;
}

int AStar::distance(const Position& start, const Position& goal) const {
    if (!initialized_) {
        throw std::runtime_error("AStar not yet initialized.");
    }

    Grid<Cell> cells = cells_;
    Set openSet;

    Cell& startCell = cells[{start.x, start.y}];
    Cell& goalCell = cells[{goal.x, goal.y}];
    if (startCell.isWall || goalCell.isWall) {
        throw std::runtime_error("Can't compute distance from/to wall cell");
    }
    startCell.distanceFromStart = 0;
    startCell.heuristic = start.distance(goal);

    openSet.push(startCell);

    while (!openSet.empty()) {
        if (openSet.top().position == goal) {
            return openSet.top().distanceFromStart;
        }
        expandCell(openSet, cells, goal);
    }

    return std::numeric_limits<int>::max();
}

void AStar::expandCell(Set& openSet, Grid<Cell>& cells, const Position& goal) const {
    Cell current = openSet.top();
    openSet.pop();

    cells[{current.position.x, current.position.y}].visited = true;

    for (const auto& move : Move::possibleMoves()) {
        Position nextPosition = current.position + move.deltaPosition;

        // If we are about to step of the maze and both the left and right end of the cell are passable,
        // we assume they are connected by a tunnel.
        if (nextPosition.x == -1 && isPassableCell({cells.width() - 1, nextPosition.y})) {
            nextPosition.x = cells.width() - 1;
        } else if (nextPosition.x == cells.width() && isPassableCell({0, nextPosition.y})) {
            nextPosition.x = 0;
        }

        if (!isPassableCell(nextPosition)) {
            continue;
        }

        Cell& neighbor = cells[{nextPosition.x, nextPosition.y}];
        if (neighbor.visited) {
            continue;
        }

        int newDistance = current.distanceFromStart + 1;
        if (newDistance < neighbor.distanceFromStart) {
            neighbor.distanceFromStart = newDistance;
            // The heuristic must always underestimate the actual distance.
            // The first term is just the euclidian distance.
            // The second term estimates the distance through the tunnel.
            int heuristic = std::min(neighbor.distance(goal), cells.width() - neighbor.distance(goal));
            neighbor.heuristic = heuristic;
            openSet.push(neighbor);
        }
    }
}

} // namespace demo
