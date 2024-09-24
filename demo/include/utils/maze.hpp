#pragma once

#include <utility>
#include <pacman/core/maze.hpp>

#include "demo/types.hpp"

namespace utils {

class Maze {
public:
    using MazeState = demo::entt::MazeState;
    using Position = demo::Position;
    using TileType = demo::TileType;

    using Ptr = std::shared_ptr<Maze>;
    using ConstPtr = std::shared_ptr<const Maze>;

    explicit Maze(MazeState mazeState) : mazeState_(std::move(mazeState)) {
    }

    TileType at(const Position& position) const {
        return tileTypeMap_.at(mazeState_[{position.x, position.y}]);
    }
    
    TileType operator[](const Position& position) const {
        return at(position);
    }

    int width() const {
        return mazeState_.width();
    }
    int height() const {
        return mazeState_.height();
    }

    bool isWall(const Position& position) const {
        return at(position) == TileType::WALL;
    }
    bool isInBounds(const Position& position) const {
        return position.x >= 0 && position.x < width() && position.y >= 0 && position.y < height();
    }
    bool isPassableCell(const Position& position) const {
        return isInBounds(position) && !isWall(position);
    }

private:
    MazeState mazeState_;
    std::map<demo::entt::Tile, demo::TileType> tileTypeMap_{
        {demo::entt::Tile::empty, TileType::EMPTY},
        {demo::entt::Tile::dot, TileType::DOT},
        {demo::entt::Tile::energizer, TileType::ENGERIZER},
        {demo::entt::Tile::wall, TileType::WALL},
        {demo::entt::Tile::door, TileType::DOOR},
    };
};

} // namespace utils
