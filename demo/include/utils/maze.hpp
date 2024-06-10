#pragma once

#include <utility>
#include <pacman/core/maze.hpp>

#include "demo/types.hpp"

namespace utils {

class Maze {
public:
    using MazeState = demo::entt::MazeState;
    using Position = demo::Position;
    using Tile = demo::entt::Tile;

    using Ptr = std::shared_ptr<Maze>;
    using ConstPtr = std::shared_ptr<const Maze>;

    explicit Maze(MazeState mazeState) : mazeState_(std::move(mazeState)) {
    }

    Tile operator()(const Position& position) const {
        return mazeState_[{position.x, position.y}];
    }

    int width() const {
        return mazeState_.width();
    }
    int height() const {
        return mazeState_.height();
    }

    bool isWall(const Position& position) const {
        return operator()(position) == Tile::wall;
    }
    bool isInBounds(const Position& position) const {
        return position.x >= 0 && position.x < width() && position.y >= 0 && position.y < height();
    }
    bool isPassableCell(const Position& position) const {
        return isInBounds(position) && !isWall(position);
    }

private:
    MazeState mazeState_;
};

} // namespace utils
