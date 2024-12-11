#pragma once

#include <algorithm>
#include <optional>
#include <utility>

#include <pacman/core/maze.hpp>

#include "demo/types.hpp"

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
inline int nonNegativeModulus(const int& numerator, const int& denominator) {
    return (denominator + (numerator % denominator)) % denominator;
}

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

    Position positionConsideringTunnel(const Position& position) const {
        Position wrappedPosition{nonNegativeModulus(position.x, width()), nonNegativeModulus(position.y, height())};
        if (isPassableCell(wrappedPosition)) {
            return wrappedPosition;
        }
        return {std::clamp(position.x, 0, width() - 1), std::clamp(position.y, 0, height() - 1)};
    }

    bool isDot(const Position& position) const {
        return at(position) == TileType::DOT;
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

struct BaseCell {
    using TileType = demo::TileType;
    using Position = demo::Position;

    BaseCell(const Position& position, const TileType& type) : position(position), type(type){};

    double manhattanDistance(const Position& other) const {
        return std::abs(position.x - other.x) + std::abs(position.y - other.y);
    }
    bool isConsumable() const {
        return type == TileType::DOT || type == TileType::ENGERIZER;
    }

    Position position;
    TileType type;
};

/**
 * @brief Adapter class for storing properties per cell of the underlying maze.
 *
 * The MazeAdapter is a helpful little class to put between an algorithm and the actual maze class. It can be used to
 * store properties per cell of the maze grid using the templated CellT class. Cells are lazily initialized.
 */
template <typename CellT = BaseCell>
class MazeAdapter {
public:
    using MazeStateConstPtr = std::shared_ptr<const MazeState>;
    using Position = demo::Position;

    explicit MazeAdapter(Maze::ConstPtr maze) : maze_(std::move(maze)), cells_({maze_->width(), maze_->height()}){};

    CellT& cell(const Position& position) const {
        if (!cells_[{position.x, position.y}]) {
            cells_[{position.x, position.y}] = CellT(position, maze_->operator[](position));
        }

        return cells_[{position.x, position.y}].value();
    }

private:
    Maze::ConstPtr maze_;
    mutable Grid<std::optional<CellT>> cells_;
};

} // namespace utils
