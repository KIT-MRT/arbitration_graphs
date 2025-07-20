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

EnvironmentModel::GhostWithDistance EnvironmentModel::closestGhost(const Time& time) const {
    if (closestGhostCache_.cached(time)) {
        return closestGhostCache_.cached(time).value();
    }

    GhostWithDistance currentlyClosestGhost = closestGhost(entities_.ghosts());
    closestGhostCache_.cache(time, currentlyClosestGhost);

    return currentlyClosestGhost;
}

std::optional<EnvironmentModel::GhostWithDistance> EnvironmentModel::closestScaredGhost(const Time& time) const {
    Ghosts scaredGhosts = entities_.scaredGhosts();
    if (scaredGhosts.empty()) {
        return std::nullopt;
    }

    if (closestScaredGhostCache_.cached(time)) {
        return closestScaredGhostCache_.cached(time);
    }

    GhostWithDistance currentlyClosestScaredGhost = closestGhost(scaredGhosts);
    closestScaredGhostCache_.cache(time, currentlyClosestScaredGhost);

    return currentlyClosestScaredGhost;
}


EnvironmentModel::GhostWithDistance EnvironmentModel::closestGhost(const Ghosts& ghosts) const {
    int minGhostDistance = std::numeric_limits<int>::max();
    Ghost closestGhost;

    for (const auto& ghost : ghosts) {
        int ghostDistance = mazeDistance(pacmanPosition(), ghost.position);
        if (ghostDistance < minGhostDistance) {
            minGhostDistance = ghostDistance;
            closestGhost = ghost;
        }
    }

    return {closestGhost, minGhostDistance};
}

Positions EnvironmentModel::toAbsolutePath(const Path& path) const {

    Position currentPosition = pacmanPosition();
    Positions absolutePath;

    // Let's follow the path and convert it to absolute positions
    for (const auto& direction : path) {
        currentPosition = currentPosition + Move(direction).deltaPosition;
        currentPosition = positionConsideringTunnel(currentPosition);
        absolutePath.push_back(currentPosition);
    }

    return absolutePath;
}

} // namespace demo
