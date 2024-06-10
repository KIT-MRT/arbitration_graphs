#include "utils/astar.hpp"

namespace utils {

int AStar::distance(const Position& start, const Position& goal) const {
    if (distanceCache_.cached({start, goal})) {
        return distanceCache_.cached({start, goal}).value();
    }

    MazeAdapter mazeAdapter(maze_);
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


    distanceCache_.cache({start, goal}, result);
    return result;
}

void AStar::expandCell(Set& openSet, MazeAdapter& mazeAdapter, const Position& goal) const {
    Cell current = openSet.top();
    openSet.pop();

    mazeAdapter.cell(current.position).visited = true;

    for (const auto& move : demo::Move::possibleMoves()) {
        Position nextPosition = current.position + move.deltaPosition;

        nextPosition = positionConsideringTunnel(nextPosition);

        if (!maze_->isPassableCell(nextPosition)) {
            continue;
        }

        Cell& neighbor = mazeAdapter.cell(nextPosition);
        if (neighbor.visited) {
            continue;
        }

        int newDistance = current.distanceFromStart + 1;
        if (newDistance < neighbor.distanceFromStart) {
            neighbor.distanceFromStart = newDistance;
            neighbor.heuristic = computeHeuristic(neighbor, goal);
            openSet.push(neighbor);
        }
    }
}

Position AStar::positionConsideringTunnel(const Position& position) const {
    if (enteringTunnelOnTheLeft(position)) {
        return {maze_->width() - 1, position.y};
    }
    if (enteringTunnelOnTheRight(position)) {
        return {0, position.y};
    }
    return position;
}

} // namespace utils
