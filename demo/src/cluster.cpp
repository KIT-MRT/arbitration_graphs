#include "utils/cluster.hpp"

#include <cmath>
#include <queue>

namespace utils {

Position Cluster::findClusterCenter() const {
    if (dots.empty()) {
        throw std::runtime_error("Cannot find center of an empty cluster");
    }
    int sumX = 0;
    int sumY = 0;

    for (const auto& point : dots) {
        sumX += point.x;
        sumY += point.y;
    }

    const int avgX = std::floor(sumX / dots.size());
    const int avgY = std::floor(sumY / dots.size());

    const Position avgPosition{avgX, avgY};
    auto distanceComparator = [&avgPosition](const Position& lhs, const Position& rhs) {
        return avgPosition.distance(lhs) < avgPosition.distance(rhs);
    };
    Position closestDot = *std::min_element(dots.begin(), dots.end(), distanceComparator);

    return closestDot;
}

DotClusterFinder::Clusters DotClusterFinder::findDotClusters() const {
    ClusterMazeAdapter mazeAdapter(maze_);
    Clusters clusters;
    int clusterId = 0;

    for (int row = 0; row < maze_->height(); row++) {
        for (int column = 0; column < maze_->width(); column++) {
            Position start{column, row};
            ClusterCell& startCell = mazeAdapter.cell(start);

            if (!startCell.isConsumable() || startCell.visited) {
                continue;
            }
            Positions dots = expandDot(startCell, mazeAdapter);
            clusters.emplace_back(clusterId++, dots);
        }
    }
    return clusters;
}

Positions DotClusterFinder::expandDot(const Cell& start, const ClusterMazeAdapter& mazeAdapter) const {
    Positions dots;

    std::queue<Position> bfsQueue;
    bfsQueue.push(start.position);

    while (!bfsQueue.empty()) {
        Position currentPosition = bfsQueue.front();
        bfsQueue.pop();
        ClusterCell& current = mazeAdapter.cell(currentPosition);

        if (current.visited) {
            continue;
        }
        current.visited = true;
        dots.push_back(currentPosition);

        for (const auto& move : demo::Move::possibleMoves()) {
            Position nextPosition = currentPosition + move.deltaPosition;
            nextPosition = maze_->positionConsideringTunnel(nextPosition);

            if (!maze_->isPassableCell(nextPosition)) {
                continue;
            }
            ClusterCell& neighbor = mazeAdapter.cell(nextPosition);

            if (neighbor.isConsumable() && !neighbor.visited) {
                bfsQueue.push(nextPosition);
            }
        }
    }

    return dots;
}

} // namespace utils
