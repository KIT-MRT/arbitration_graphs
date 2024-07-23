#include "utils/astar.hpp"

namespace utils {

/**
 * @brief Computes the modulus of a given numerator and denominator, ensuring a non-negative result when the denominator
 * is positive.
 *
 * This function calculates the result of the modulus operation. If the denominator is positive, the result is
 * non-negative and in the range [0, denominator - 1].
 *
 * @param numerator The value to be divided (dividend).
 * @param denominator The value by which the numerator is divided (divisor).
 * @return An integer representing the modulus of the division.
 */
int nonNegativeModulus(const int& numerator, const int& denominator) {
    return (denominator + (numerator % denominator)) % denominator;
}

int AStar::mazeDistance(const Position& start, const Position& goal) const {
    if (distanceCache_.cached({start, goal})) {
        return distanceCache_.cached({start, goal}).value();
    }

    Path path = shortestPath(start, goal);
    int pathLength = static_cast<int>(path.size());

    distanceCache_.cache({start, goal}, pathLength);
    return pathLength;
}

Path AStar::shortestPath(const Position& start, const Position& goal) const {
    // There is a "virtual" position outside of the maze that entities are on when entering the tunnel. We accept a
    // small error in the distance computation by neglecting this and wrapping the position to be on either end of the
    // tunnel.
    Position wrappedStart = positionConsideringTunnel(start);
    Position wrappedGoal = positionConsideringTunnel(goal);

    MazeAdapter mazeAdapter(maze_);
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
            return pathTo(mazeAdapter, openSet.top().position);
        }
        expandCell(openSet, mazeAdapter, heuristic);
    }

    return {};
}

Path AStar::pathToClosestDot(const Position& start) const {
    // There is a "virtual" position outside of the maze that entities are on when entering the tunnel. We accept a
    // small error in the distance computation by neglecting this and wrapping the position to be on either end of the
    // tunnel.
    Position wrappedStart = positionConsideringTunnel(start);

    MazeAdapter mazeAdapter(maze_);
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
        // explicitely exclude the start position from the search.
        if (openSet.top().type == TileType::DOT && openSet.top().position != start) {
            return pathTo(mazeAdapter, openSet.top().position);
        }
        expandCell(openSet, mazeAdapter, heuristic);
    }

    return {};
}

void AStar::expandCell(Set& openSet, MazeAdapter& mazeAdapter, const HeuristicFunction& heuristic) const {
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
            neighbor.heuristic = heuristic(neighbor);
            neighbor.moveFromPredecessor = move;
            openSet.push(neighbor);
        }
    }
}

Path AStar::pathTo(const MazeAdapter& maze, const Position& goal) const {
    Path path;
    Cell current = maze.cell(goal);
    while (current.moveFromPredecessor) {
        path.push_back(current.moveFromPredecessor->direction);
        Position predecessorPosition = current.position - current.moveFromPredecessor->deltaPosition;
        predecessorPosition = positionConsideringTunnel(predecessorPosition);
        current = maze.cell(predecessorPosition);
    }

    std::reverse(path.begin(), path.end());
    return path;
}

Position AStar::positionConsideringTunnel(const Position& position) const {
    Position wrappedPosition{nonNegativeModulus(position.x, maze_->width()),
                             nonNegativeModulus(position.y, maze_->height())};
    if (maze_->isPassableCell(wrappedPosition)) {
        return wrappedPosition;
    }
    return position;
}

} // namespace utils
