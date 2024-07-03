#include "utils/entities.hpp"
#include <comp/ghost_mode.hpp>

namespace utils {

void Ghost::update(const demo::entt::Registry& registry, const demo::entt::Entity& entity) {
    const auto& enttPosition = registry.get<demo::entt::Position>(entity);
    position = {enttPosition.p.x, enttPosition.p.y};

    scaredCountdown = std::nullopt;
    if (registry.has<ChaseMode>(entity)) {
        mode = demo::GhostMode::CHASING;
        return;
    }

    if (registry.has<EatenMode>(entity)) {
        mode = demo::GhostMode::EATEN;
        return;
    }

    if (registry.has<ScaredMode>(entity)) {
        mode = demo::GhostMode::SCARED;
        scaredCountdown = registry.get<ScaredMode>(entity).timer;
        return;
    }

    if (registry.has<ScatterMode>(entity)) {
        mode = demo::GhostMode::SCATTERING;
        return;
    }
}

void Pacman::update(const demo::entt::Registry& registry, const demo::entt::Entity& entity) {
    const auto& enttPosition = registry.get<demo::entt::Position>(entity);
    position = {enttPosition.p.x, enttPosition.p.y};
}
} // namespace utils
