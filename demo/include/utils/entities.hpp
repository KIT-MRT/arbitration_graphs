#pragma once

#include <optional>

#include "demo/types.hpp"

namespace utils {

struct Ghost {
    void update(const demo::entt::Registry& registry, const demo::entt::Entity& entity);

    demo::Position position;
    demo::Direction direction;
    demo::GhostMode mode;
    std::optional<int> scaredCountdown;
};

struct Pacman {
    void update(const demo::entt::Registry& registry, const demo::entt::Entity& entity);

    demo::Position position;
    demo::Direction direction;
};

struct Entities {
    using Ghosts = std::vector<Ghost>;
    using Position = demo::Position;
    using Positions = demo::Positions;

    Ghosts ghosts() const {
        return {blinky, pinky, inky, clyde};
    }
    Positions ghostPositions() const {
        return {blinky.position, pinky.position, inky.position, clyde.position};
    }
    Ghosts scaredGhosts() const;

    utils::Pacman pacman;

    Ghost blinky;
    Ghost pinky;
    Ghost inky;
    Ghost clyde;
};

} // namespace utils
