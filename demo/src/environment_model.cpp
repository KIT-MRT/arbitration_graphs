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

    PositionWithDistance currentlyClosestGhost = closestGhost(entities_.ghosts());
    closestGhostCache_.cache(time, currentlyClosestGhost);

    return currentlyClosestGhost;
}

std::optional<PositionWithDistance> EnvironmentModel::closestScaredGhost(const Time& time) const {
    Ghosts scaredGhosts = entities_.scaredGhosts();
    if (scaredGhosts.empty()) {
        return std::nullopt;
    }

    if (closestScaredGhostCache_.cached(time)) {
        return closestScaredGhostCache_.cached(time).value();
    }

    PositionWithDistance currentlyClosestScaredGhost = closestGhost(scaredGhosts);
    closestScaredGhostCache_.cache(time, currentlyClosestScaredGhost);

    return currentlyClosestScaredGhost;
}


PositionWithDistance EnvironmentModel::closestGhost(const Ghosts& ghosts) const {
    int minGhostDistance = std::numeric_limits<int>::max();
    Position closestGhostPosition;

    for (const auto& ghost : ghosts) {
        int ghostDistance = distance(pacmanPosition(), ghost.position);
        if (ghostDistance < minGhostDistance) {
            minGhostDistance = ghostDistance;
            closestGhostPosition = ghost.position;
        }
    }

    return {closestGhostPosition, minGhostDistance};
}

} // namespace demo
