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

int AStar::manhattanDistance(const Position& start, const Position& goal) const {
    // There is a "virtual" position outside of the maze that entities are on when entering the tunnel. We accept a
    // small error in the distance computation by neglecting this and wrapping the position to be on either end of the
    // tunnel.
    Position wrappedStart = positionConsideringTunnel(start);
    Position wrappedGoal = positionConsideringTunnel(goal);
    if (distanceCache_.cached({wrappedStart, wrappedGoal})) {
        return distanceCache_.cached({wrappedStart, wrappedGoal}).value();
    }

    MazeAdapter mazeAdapter(maze_);
    Set openSet;

    Cell& startCell = mazeAdapter.cell(wrappedStart);
    Cell& goalCell = mazeAdapter.cell(wrappedGoal);
    if (startCell.isWall || goalCell.isWall) {
        throw std::runtime_error("Can't compute distance from/to wall cell");
    }
    startCell.distanceFromStart = 0;
    startCell.heuristic = computeHeuristic(startCell, wrappedGoal);

    openSet.push(startCell);

    int result = NoPathFound;
    while (!openSet.empty()) {
        if (openSet.top().position == wrappedGoal) {
            result = openSet.top().distanceFromStart;
            break;
        }
        expandCell(openSet, mazeAdapter, wrappedGoal);
    }


    distanceCache_.cache({wrappedStart, wrappedGoal}, result);
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
    Position wrappedPosition{nonNegativeModulus(position.x, maze_->width()),
                             nonNegativeModulus(position.y, maze_->height())};
    if (maze_->isPassableCell(wrappedPosition)) {
        return wrappedPosition;
    }
    return position;
}

} // namespace utils
