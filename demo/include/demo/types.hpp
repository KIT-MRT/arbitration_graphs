#pragma once

#include <map>
#include <vector>

#include <SDL_scancode.h>
#include <arbitration_graphs/types.hpp>
#include <comp/position.hpp>
#include <core/maze.hpp>
#include <entt/entity/registry.hpp>

namespace demo {

using Clock = arbitration_graphs::Clock;
using Time = arbitration_graphs::Time;

namespace entt {
using Position = ::Position;
using registry = ::entt::registry;
using MazeState = ::MazeState;
} // namespace entt

enum class Direction { UP, DOWN, LEFT, RIGHT, NONE };

struct Position {
    int x;
    int y;

    double distance(const Position& other) const {
        return std::sqrt(std::pow(x - other.x, 2) + std::pow(y - other.y, 2));
    }
    Position operator+(const Position& other) {
        return {x + other.x, y + other.y};
    }
};
using Positions = std::vector<Position>;

struct Command {
    Command(Direction direction) : direction(direction) {
    }
    SDL_Scancode scancode() const {
        return scancodeMap.at(direction);
    }

    Direction direction;
    const std::map<Direction, SDL_Scancode> scancodeMap{
        {Direction::UP, SDL_SCANCODE_UP},
        {Direction::DOWN, SDL_SCANCODE_DOWN},
        {Direction::LEFT, SDL_SCANCODE_LEFT},
        {Direction::RIGHT, SDL_SCANCODE_RIGHT},
        {Direction::NONE, SDL_SCANCODE_UNKNOWN},
    };
};


} // namespace demo
