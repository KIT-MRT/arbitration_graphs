#pragma once

#include <map>

#include <comp/position.hpp>
#include <entt/entity/registry.hpp>
#include <SDL_scancode.h>

namespace demo {

namespace entt {
    using Position = ::Position;
    using registry = ::entt::registry;
}

enum class Direction { UP, DOWN, LEFT, RIGHT, NONE };

struct Position {
    int x;
    int y;
};
class Command {
public:
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
    };
};


} // namespace demo
