#include "utils/cluster.hpp"

#include <cmath>
#include <queue>

namespace utils {

Position Cluster::findClusterCenter() const {
    int sumX = 0;
    int sumY = 0;

    for (const auto& dot : dots) {
        sumX += dot.x;
        sumY += dot.y;
    }

    int avgX = std::floor(sumX / dots.size());
    int avgY = std::floor(sumY / dots.size());

    Position avgPosition{avgX, avgY};
    Position closestDot = dots.front();
    double minDistance = std::numeric_limits<double>::max();

    for (const auto& dot : dots) {
        double distance = avgPosition.distance(dot);
        if (distance < minDistance) {
            minDistance = distance;
            closestDot = dot;
        }
    }

    return closestDot;
}


std::vector<Cluster> ClusterFinder::findDotClusters() const {
    ClusterMazeAdapter mazeAdapter(maze_);
    std::vector<Cluster> clusters;
    int clusterId = 0;

    for (int row = 0; row < maze_->height(); row++) {
        for (int column = 0; column < maze_->width(); column++) {
            Position start{column, row};
            ClusterCell& startCell = mazeAdapter.cell(start);

            if (startCell.type != TileType::DOT || startCell.visited) {
                continue;
            }
            std::vector<Position> dots = expandDot(startCell, mazeAdapter);
            clusters.emplace_back(clusterId++, dots);
        }
    }
    return clusters;
}

std::vector<Position> ClusterFinder::expandDot(const Cell& start, const ClusterMazeAdapter& mazeAdapter) const {
    std::vector<Position> dots;

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

            if (neighbor.type == TileType::DOT && !neighbor.visited) {
                bfsQueue.push(nextPosition);
            }
        }
    }

    return dots;
}

} // namespace utils
