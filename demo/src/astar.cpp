#include "utils/astar.hpp"

namespace utils {

int AStar::mazeDistance(const Position& start, const Position& goal) const {
    if (distanceCache_.cached({start, goal})) {
        return distanceCache_.cached({start, goal}).value();
    }

    std::optional<Path> path = shortestPath(start, goal);
    int pathLength = path ? static_cast<int>(path->size()) : NoPathFound;

    distanceCache_.cache({start, goal}, pathLength);
    return pathLength;
}

std::optional<Path> AStar::shortestPath(const Position& start, const Position& goal) const {
    // There is a "virtual" position outside of the maze that entities are on when entering the tunnel. We accept a
    // small error in the distance computation by neglecting this and wrapping the position to be on either end of the
    // tunnel.
    Position wrappedStart = maze_->positionConsideringTunnel(start);
    Position wrappedGoal = maze_->positionConsideringTunnel(goal);

    AStarMazeAdapter mazeAdapter(maze_);
    Set openSet;

    Cell& startCell = mazeAdapter.cell(wrappedStart);
    Cell& goalCell = mazeAdapter.cell(wrappedGoal);
    if (startCell.type == TileType::WALL || goalCell.type == TileType::WALL) {
        throw std::runtime_error("Can't compute path from/to wall cell");
    }
    startCell.distanceFromStart = 0;
    HeuristicFunction heuristic = [this, &wrappedGoal](const Cell& cell) {
        return optimisticDistanceToGoal(cell, wrappedGoal);
    };
    startCell.heuristic = heuristic(startCell);

    openSet.push(startCell);

    while (!openSet.empty()) {
        if (openSet.top().position == wrappedGoal) {
            return extractPathTo(mazeAdapter, openSet.top().position);
        }
        expandCell(openSet, mazeAdapter, heuristic);
    }

    return std::nullopt;
}

std::optional<Path> AStar::pathToClosestDot(const Position& start) const {
    // There is a "virtual" position outside of the maze that entities are on when entering the tunnel. We accept a
    // small error in the distance computation by neglecting this and wrapping the position to be on either end of the
    // tunnel.
    Position wrappedStart = maze_->positionConsideringTunnel(start);

    AStarMazeAdapter mazeAdapter(maze_);
    Set openSet;

    Cell& startCell = mazeAdapter.cell(wrappedStart);
    if (startCell.type == TileType::WALL) {
        throw std::runtime_error("Can't compute path from wall cell");
    }
    startCell.distanceFromStart = 0;
    HeuristicFunction heuristic = [](const Cell&) { return 0; };
    startCell.heuristic = 0;

    openSet.push(startCell);

    while (!openSet.empty()) {
        // Unfortunately, the pacman simulation will handle the dot consumption after the move, therefore we need to
        // explicitly exclude the start position from the search.
        if (openSet.top().type == TileType::DOT && openSet.top().position != start) {
            return extractPathTo(mazeAdapter, openSet.top().position);
        }
        expandCell(openSet, mazeAdapter, heuristic);
    }

    return std::nullopt;
}

void AStar::expandCell(Set& openSet, AStarMazeAdapter& mazeAdapter, const HeuristicFunction& heuristic) const {
    Cell current = openSet.top();
    openSet.pop();

    mazeAdapter.cell(current.position).visited = true;

    for (const auto& move : demo::Move::possibleMoves()) {
        Position nextPosition = current.position + move.deltaPosition;

        nextPosition = maze_->positionConsideringTunnel(nextPosition);

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
            neighbor.heuristic = heuristic(neighbor);
            neighbor.moveFromPredecessor = move;
            openSet.push(neighbor);
        }
    }
}

Path AStar::extractPathTo(const AStarMazeAdapter& maze, const Position& goal) const {
    Path path;
    Cell current = maze.cell(goal);
    while (current.moveFromPredecessor) {
        path.push_back(current.moveFromPredecessor->direction);
        Position predecessorPosition = current.position - current.moveFromPredecessor->deltaPosition;
        predecessorPosition = maze_->positionConsideringTunnel(predecessorPosition);
        current = maze.cell(predecessorPosition);
    }

    std::reverse(path.begin(), path.end());
    return path;
}

} // namespace utils
