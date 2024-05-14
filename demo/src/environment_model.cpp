#include "demo/environment_model.hpp"

namespace demo {

    void EnvironmentModel::updatePositions(entt::registry& registry) {
        auto view = registry.view<entt::Position>();
        for (auto entity : view) {
            if (registry.has<Player>(entity)) {
                auto& pacmanPosition = registry.get<entt::Position>(entity);
                entityPositions_.pacman = {pacmanPosition.p.x, pacmanPosition.p.y};
            } else if (registry.has<BlinkyChaseTarget>(entity)) {
                auto& blinkyPosition = registry.get<entt::Position>(entity);
                entityPositions_.blinky = {blinkyPosition.p.x, blinkyPosition.p.y};
            } else if (registry.has<PinkyChaseTarget>(entity)) {
                auto& pinkyPosition = registry.get<entt::Position>(entity);
                entityPositions_.pinky = {pinkyPosition.p.x, pinkyPosition.p.y};
            } else if (registry.has<InkyChaseTarget>(entity)) {
                auto& inkyPosition = registry.get<entt::Position>(entity);
                entityPositions_.inky = {inkyPosition.p.x, inkyPosition.p.y};
            } else if (registry.has<ClydeChaseTarget>(entity)) {
                auto& clydePosition = registry.get<entt::Position>(entity);
                entityPositions_.clyde = {clydePosition.p.x, clydePosition.p.y};
            }
        }
    }

    Position EnvironmentModel::closestGhostPosition() const {
        double minDistance = std::numeric_limits<double>::max();
        Position closestGhostPosition;
        for (const auto& ghostPosition : entityPositions_.ghostPositions()) {
            double distance = ghostPosition.distance(pacmanPosition());
            if (distance < minDistance) {
                minDistance = distance;
                closestGhostPosition = ghostPosition;
            }
        }
        return closestGhostPosition;
    }
}