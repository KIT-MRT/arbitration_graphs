#include "utils/astar.hpp"

namespace utils {

int AStar::distance(const Position& start, const Position& goal) const {
    if (distanceCache.cached({start, goal})) {
        return distanceCache.cached({start, goal}).value();
    }

    MazeAdapter mazeAdapter(mazeState_);
    Set openSet;

    Cell& startCell = mazeAdapter.cell(start);
    Cell& goalCell = mazeAdapter.cell(goal);
    if (startCell.isWall || goalCell.isWall) {
        throw std::runtime_error("Can't compute distance from/to wall cell");
    }
    startCell.distanceFromStart = 0;
    startCell.heuristic = start.distance(goal);

    openSet.push(startCell);

    int result = NO_PATH_FOUND;
    while (!openSet.empty()) {
        if (openSet.top().position == goal) {
            result = openSet.top().distanceFromStart;
            break;
        }
        expandCell(openSet, mazeAdapter, goal);
    }


    distanceCache.cache({start, goal}, result);
    return result;
}

void AStar::expandCell(Set& openSet, MazeAdapter& mazeAdapter, const Position& goal) const {
    Cell current = openSet.top();
    openSet.pop();

    mazeAdapter.cell(current.position).visited = true;

    for (const auto& move : demo::Move::possibleMoves()) {
        Position nextPosition = current.position + move.deltaPosition;

        // If we are about to step of the maze and both the left and right end of the cell are passable,
        // we assume they are connected by a tunnel.
        if (nextPosition.x == -1 && mazeAdapter.isPassableCell({mazeAdapter.width() - 1, nextPosition.y})) {
            nextPosition.x = mazeAdapter.width() - 1;
        } else if (nextPosition.x == mazeAdapter.width() && mazeAdapter.isPassableCell({0, nextPosition.y})) {
            nextPosition.x = 0;
        }

        if (!mazeAdapter.isPassableCell(nextPosition)) {
            continue;
        }

        Cell& neighbor = mazeAdapter.cell(nextPosition);
        if (neighbor.visited) {
            continue;
        }

        int newDistance = current.distanceFromStart + 1;
        if (newDistance < neighbor.distanceFromStart) {
            neighbor.distanceFromStart = newDistance;
            // The heuristic must always underestimate the actual distance.
            // The first term is just the euclidian distance.
            // The second term estimates the distance through the tunnel.
            int heuristic = std::min(neighbor.distance(goal), mazeAdapter.width() - neighbor.distance(goal));
            neighbor.heuristic = heuristic;
            openSet.push(neighbor);
        }
    }
}

} // namespace utils
