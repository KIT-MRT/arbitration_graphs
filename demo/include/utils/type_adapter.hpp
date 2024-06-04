#pragma once

#include "demo/types.hpp"

namespace utils {

class DirectionAdapter {
public:
    using Direction = demo::Direction;
    using enttDirection = demo::entt::Direction;

    Direction fromEnttDirection(const enttDirection& enttDirection) const {
        return enttDirectionMap.at(enttDirection);
    }

private:
    const std::map<enttDirection, Direction> enttDirectionMap{
        {enttDirection::up, Direction::UP},
        {enttDirection::down, Direction::DOWN},
        {enttDirection::left, Direction::LEFT},
        {enttDirection::right, Direction::RIGHT},
        {enttDirection::none, Direction::LAST},
    };
};

class MoveAdapter {
public:
    using Move = demo::Move;
    using Direction = demo::Direction;
    using Position = demo::Position;

    Move fromDirection(const Direction& direction) {
        return {direction, deltaPositionMap.at(direction)};
    }
    Position getDeltaPosition(const Direction& direction) {
        return deltaPositionMap.at(direction);
    }

private:
    const std::map<Direction, Position> deltaPositionMap{{Direction::UP, {0, -1}},
                                                         {Direction::DOWN, {0, 1}},
                                                         {Direction::LEFT, {-1, 0}},
                                                         {Direction::RIGHT, {1, 0}},
                                                         {Direction::LAST, {0, 0}}};
};

} // namespace utils
