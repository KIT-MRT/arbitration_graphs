#include "utils/entities.hpp"

namespace utils {

std::optional<demo::Direction> toDemoDirection(const demo::entt::Direction& enttDirection) {
    const std::map<demo::entt::Direction, demo::Direction> typeMap{
        {demo::entt::Direction::up, demo::Direction::UP},
        {demo::entt::Direction::down, demo::Direction::DOWN},
        {demo::entt::Direction::left, demo::Direction::LEFT},
        {demo::entt::Direction::right, demo::Direction::RIGHT},
    };

    auto mapIterator = typeMap.find(enttDirection);
    if (mapIterator != typeMap.end()) {
        return mapIterator->second;
    }
    return std::nullopt;
}

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

    const auto& enttDirection = registry.get<demo::entt::ActualDirection>(entity);
    const std::optional<demo::Direction> demoDirection = toDemoDirection(enttDirection.d);
    if (demoDirection) {
        direction = demoDirection.value();
    }
}

Entities::Ghosts Entities::scaredGhosts() const {
    std::vector<Ghost> scaredGhosts;
    for (const auto& ghost : ghosts()) {
        if (ghost.mode == demo::GhostMode::SCARED) {
            scaredGhosts.push_back(ghost);
        }
    }
    return scaredGhosts;
}

} // namespace utils
