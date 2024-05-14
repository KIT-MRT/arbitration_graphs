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

    for (const auto& move : possibleMoves) {
        Position nextPosition = current.position + move.deltaPosition;

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
            neighbor.heuristic = neighbor.distance(goal);
            openSet.push(neighbor);
        }
    }
}

} // namespace demo