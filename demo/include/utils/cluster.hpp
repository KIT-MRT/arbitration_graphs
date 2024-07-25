#pragma once

#include <vector>

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
    explicit Cluster(const int& clusterId, const std::vector<Position>& dots)
            : id(clusterId), dots(dots), center{findClusterCenter()} {
    }

    int id;
    std::vector<Position> dots;

    /// @brief The dot closest to the average position of all the dots of this cluster
    Position center;

private:
    Position findClusterCenter() const;
};

class ClusterFinder {
public:
    using Clusters = std::vector<Cluster>;
    using ClusterMazeAdapter = MazeAdapter<ClusterCell>;
    using Cell = ClusterCell;

    explicit ClusterFinder(Maze::ConstPtr maze) : maze_(std::move(maze)), clusters_{findDotClusters()} {
    }
    Clusters clusters() const {
        return clusters_;
    }
    std::vector<Position> clusterCenters() const;


private:
    std::vector<Position> expandDot(const Cell& start, const ClusterMazeAdapter& mazeAdapter) const;
    Clusters findDotClusters() const;

    Maze::ConstPtr maze_;
    Clusters clusters_;
};

} // namespace utils
