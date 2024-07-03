#include "demo/environment_model.hpp"

namespace demo {

void EnvironmentModel::updateEntities(const entt::Registry& registry) {
    auto view = registry.view<const entt::Position>();
    for (auto entity : view) {
        if (registry.has<Player>(entity)) {
            entities_.pacman.update(registry, entity);
        } else if (registry.has<BlinkyChaseTarget>(entity)) {
            entities_.blinky.update(registry, entity);
        } else if (registry.has<PinkyChaseTarget>(entity)) {
            entities_.pinky.update(registry, entity);
        } else if (registry.has<InkyChaseTarget>(entity)) {
            entities_.inky.update(registry, entity);
        } else if (registry.has<ClydeChaseTarget>(entity)) {
            entities_.clyde.update(registry, entity);
        }
    }
}

PositionWithDistance EnvironmentModel::closestGhost(const Time& time) const {
    if (closestGhostCache_.cached(time)) {
        return closestGhostCache_.cached(time).value();
    }

    int minGhostDistance = std::numeric_limits<int>::max();
    Position closestGhostPosition;
    for (const auto& ghostPosition : entities_.ghostPositions()) {
        int ghostDistance = distance(pacmanPosition(), ghostPosition);
        if (ghostDistance < minGhostDistance) {
            minGhostDistance = ghostDistance;
            closestGhostPosition = ghostPosition;
        }
    }
    PositionWithDistance closestGhost{closestGhostPosition, minGhostDistance};
    closestGhostCache_.cache(time, closestGhost);
    return closestGhost;
}

} // namespace demo
