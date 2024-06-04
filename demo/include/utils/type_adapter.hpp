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

} // namespace utils
