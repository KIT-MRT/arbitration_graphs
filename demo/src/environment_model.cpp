#include "demo/environment_model.hpp"

#include "utils/type_adapter.hpp"

namespace demo {

Pose poseFromRegistry(const entt::registry& registry, const entt::entity& entity) {
    auto& enttPosition = registry.get<entt::Position>(entity);
    Position position = {enttPosition.p.x, enttPosition.p.y};
    auto& enttDirection = registry.get<entt::ActualDirection>(entity).d;
    Direction direction = utils::DirectionAdapter().fromEnttDirection(enttDirection);
    return {position, direction};
}

void EnvironmentModel::updatePoses(const entt::registry& registry) {
    auto view = registry.view<const entt::Position, const entt::ActualDirection>();
    for (auto entity : view) {
        if (registry.has<Player>(entity)) {
            entityPoses_.pacman = poseFromRegistry(registry, entity);
        } else if (registry.has<BlinkyChaseTarget>(entity)) {
            entityPoses_.blinky = poseFromRegistry(registry, entity);
        } else if (registry.has<PinkyChaseTarget>(entity)) {
            entityPoses_.pinky = poseFromRegistry(registry, entity);
        } else if (registry.has<InkyChaseTarget>(entity)) {
            entityPoses_.inky = poseFromRegistry(registry, entity);
        } else if (registry.has<ClydeChaseTarget>(entity)) {
            entityPoses_.clyde = poseFromRegistry(registry, entity);
        }
    }
}

PositionWithDistance EnvironmentModel::closestGhost(const Time& time) const {
    if (closestGhostCache_.cached(time)) {
        return closestGhostCache_.cached(time).value();
    }

    int minDistance = std::numeric_limits<int>::max();
    Position closestGhostPosition;
    for (const auto& ghostPosition : entityPoses_.ghostPositions()) {
        double distance = ghostPosition.distance(pacmanPosition());
        if (distance < minDistance) {
            minDistance = distance;
            closestGhostPosition = ghostPosition;
        }
    }
    PositionWithDistance closestGhost{closestGhostPosition, minDistance};
    closestGhostCache_.cache(time, closestGhost);
    return closestGhost;
}

} // namespace demo
