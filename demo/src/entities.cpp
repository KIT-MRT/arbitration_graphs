#include "utils/entities.hpp"

namespace utils {

namespace {
std::optional<demo::Direction> toDemoDirection(const demo::entt::Direction& enttDirection) {
    const std::map<demo::entt::Direction, demo::Direction> typeMap{
        {demo::entt::Direction::up, demo::Direction::Up},
        {demo::entt::Direction::down, demo::Direction::Down},
        {demo::entt::Direction::left, demo::Direction::Left},
        {demo::entt::Direction::right, demo::Direction::Right},
    };

    auto mapIterator = typeMap.find(enttDirection);
    if (mapIterator != typeMap.end()) {
        return mapIterator->second;
    }
    return std::nullopt;
}
} // namespace

void Ghost::update(const demo::entt::Registry& registry, const demo::entt::Entity& entity) {
    const auto& enttPosition = registry.get<demo::entt::Position>(entity);
    position = {enttPosition.p.x, enttPosition.p.y};

    const auto& enttDirection = registry.get<demo::entt::ActualDirection>(entity);
    const std::optional<demo::Direction> demoDirection = toDemoDirection(enttDirection.d);
    if (demoDirection) {
        direction = demoDirection.value();
    }

    scaredCountdown = std::nullopt;
    if (registry.has<ChaseMode>(entity)) {
        mode = demo::GhostMode::Chasing;
        return;
    }

    if (registry.has<EatenMode>(entity)) {
        mode = demo::GhostMode::Eaten;
        return;
    }

    if (registry.has<ScaredMode>(entity)) {
        mode = demo::GhostMode::Scared;
        scaredCountdown = registry.get<ScaredMode>(entity).timer;
        return;
    }

    if (registry.has<ScatterMode>(entity)) {
        mode = demo::GhostMode::Scattering;
        return;
    }
}

void Pacman::update(const demo::entt::Registry& registry, const demo::entt::Entity& entity) {
    const auto& enttPosition = registry.get<demo::entt::Position>(entity);
    position = {enttPosition.p.x, enttPosition.p.y};

    const auto& enttDirection = registry.get<demo::entt::ActualDirection>(entity);
    const std::optional<demo::Direction> demoDirection = toDemoDirection(enttDirection.d);
    if (demoDirection) {
        direction = demoDirection.value();
    }
}

Entities::Ghosts Entities::scaredGhosts() const {
    std::vector<Ghost> scaredGhosts;
    for (const auto& ghost : ghosts()) {
        if (ghost.mode == demo::GhostMode::Scared) {
            scaredGhosts.push_back(ghost);
        }
    }
    return scaredGhosts;
}

} // namespace utils
