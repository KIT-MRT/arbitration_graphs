#pragma once

#include <map>
#include <utility>
#include <vector>

#include <SDL_scancode.h>
#include <arbitration_graphs/types.hpp>
#include <entt/entity/registry.hpp>
#include <pacman/comp/dir.hpp>
#include <pacman/comp/ghost_mode.hpp>
#include <pacman/comp/position.hpp>
#include <pacman/core/game.hpp>
#include <pacman/core/maze.hpp>
#include <pacman/util/dir.hpp>

namespace demo {

using Clock = arbitration_graphs::Clock;
using Duration = arbitration_graphs::Duration;
using Time = arbitration_graphs::Time;

namespace entt {
using Direction = ::Dir;
using ActualDirection = ::ActualDir;
using Entity = ::entt::entity;
using Game = ::Game;
using Position = ::Position;
using Registry = ::entt::registry;
using MazeState = ::MazeState;
using ScaredMode = ::ScaredMode;
using Tile = ::Tile;
} // namespace entt

enum class Direction { UP, DOWN, LEFT, RIGHT, LAST };
enum class GhostMode { CHASING, EATEN, SCARED, SCATTERING };
enum class TileType { EMPTY, DOT, ENGERIZER, WALL, DOOR };

using Path = std::vector<Direction>;
struct Position {
    int x;
    int y;

    double distance(const Position& other) const {
        return std::sqrt(std::pow(x - other.x, 2) + std::pow(y - other.y, 2));
    }
    Position operator+(const Position& other) const {
        return {x + other.x, y + other.y};
    }
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};
using Positions = std::vector<Position>;

struct Command {
    explicit Command(const Direction& direction) : path{direction} {
    }
    explicit Command(Path path) : path{std::move(path)} {
    }

    Direction nextDirection() const {
        return path.front();
    }
    SDL_Scancode scancode() const {
        return scancodeMap.at(nextDirection());
    }

    std::map<Direction, SDL_Scancode> scancodeMap{
        {Direction::UP, SDL_SCANCODE_UP},
        {Direction::DOWN, SDL_SCANCODE_DOWN},
        {Direction::LEFT, SDL_SCANCODE_LEFT},
        {Direction::RIGHT, SDL_SCANCODE_RIGHT},
        {Direction::LAST, SDL_SCANCODE_UNKNOWN},
    };

    Path path;
};

struct Move {
    Direction direction;
    Position deltaPosition;

    static std::vector<Move> possibleMoves() {
        return {{Direction::UP, {0, -1}},
                {Direction::DOWN, {0, 1}},
                {Direction::LEFT, {-1, 0}},
                {Direction::RIGHT, {1, 0}}};
    }
};
using Moves = std::vector<Move>;

} // namespace demo
