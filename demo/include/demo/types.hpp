#pragma once

#include <map>

#include <SDL_scancode.h>

namespace demo {


class Command {
public:
    enum class Direction {
        UP,
        DOWN,
        LEFT,
        RIGHT,
    };

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
