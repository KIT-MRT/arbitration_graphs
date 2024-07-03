#pragma once

#include <optional>

#include "demo/types.hpp"

namespace utils {

struct Ghost {
    void update(const demo::entt::Registry& registry, const demo::entt::Entity& entity);

    demo::Position position;
    demo::GhostMode mode;
    std::optional<int> scaredCountdown;
};

struct Pacman {
    void update(const demo::entt::Registry& registry, const demo::entt::Entity& entity);

    demo::Position position;
};

} // namespace utils
