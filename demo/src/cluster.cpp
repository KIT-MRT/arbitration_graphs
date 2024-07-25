#include "utils/cluster.hpp"

#include <queue>

namespace utils {

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
            Cluster cluster = expandDot(startCell, mazeAdapter, clusterId++);
            clusters.push_back(cluster);
        }
    }
    return clusters;
}

Cluster ClusterFinder::expandDot(const Cell& start, const ClusterMazeAdapter& mazeAdapter, const int& clusterID) const {
    Cluster cluster(clusterID);

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
        cluster.dots.push_back(currentPosition);

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

    return cluster;
}

} // namespace utils
