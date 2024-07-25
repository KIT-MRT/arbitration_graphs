#pragma once

#include <vector>

#include "demo/types.hpp"
#include "utils/maze.hpp"

namespace utils {

using Path = demo::Path;
using Position = demo::Position;
using Positions = demo::Positions;
using TileType = demo::TileType;

struct ClusterCell : public BaseCell {
    ClusterCell(const Position& position, const TileType& type) : BaseCell(position, type) {};

    bool visited{false};
};

/**
 * @brief A cluster is defined by a set of points that can be connected by a path which passes through neither walls nor
 * empty space.
 */
struct Cluster {
    explicit Cluster(const int& clusterId, const std::vector<Position>& points)
            : id(clusterId), dots(points), center{findClusterCenter()} {
    }
    bool isInCluster(const Position& target) const {
        return std::any_of(dots.begin(), dots.end(), [target](Position dot) { return dot == target; });
    }

    int id;
    Positions dots;

    Position center; ///< The dot closest to the average position of all the dots of this cluster

private:
    Position findClusterCenter() const;
};

/**
 * @brief Search and store all clusters of dots (including power pellets) given the maze state.
 */
class DotClusterFinder {
public:
    using Cell = ClusterCell;
    using Clusters = std::vector<Cluster>;
    using ClusterMazeAdapter = MazeAdapter<ClusterCell>;

    explicit DotClusterFinder(Maze::ConstPtr maze) : maze_(std::move(maze)), clusters_{findDotClusters()} {
    }
    Clusters clusters() const {
        return clusters_;
    }

private:
    Positions expandDot(const Cell& start, const ClusterMazeAdapter& mazeAdapter) const;
    Clusters findDotClusters() const;

    Maze::ConstPtr maze_;
    Clusters clusters_;
};

} // namespace utils
