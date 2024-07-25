#pragma once

#include "demo/types.hpp"
#include "utils/maze.hpp"

namespace utils {

using Path = demo::Path;
using Position = demo::Position;
using TileType = demo::TileType;

struct ClusterCell : public BaseCell {
    ClusterCell(const Position& position, const TileType& type) : BaseCell(position, type) {};

    bool visited{false};
};

struct Cluster {
    explicit Cluster(int clusterId) : id(clusterId) {
    }

    int id;
    std::vector<Position> dots;
};

class ClusterFinder {
public:
    using ClusterMazeAdapter = MazeAdapter<ClusterCell>;
    using Cell = ClusterCell;
    explicit ClusterFinder(Maze::ConstPtr maze) : maze_(std::move(maze)) {
    }

    std::vector<Cluster> findDotClusters() const;

private:
    Cluster expandDot(const Cell& start, const ClusterMazeAdapter& mazeAdapter, const int& clusterID) const;

    Maze::ConstPtr maze_;
};

} // namespace utils
