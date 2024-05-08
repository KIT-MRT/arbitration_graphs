#pragma once

#include <comp/chase_target.hpp>
#include <comp/player.hpp>

#include "types.hpp"

namespace demo {

class EnvironmentModel {
public:
    struct Positions {
        Position player;
        Position blinky;
        Position pinky;
        Position inky;
        Position clyde;
    };

    void update(entt::registry& registry) {
        auto view = registry.view<entt::Position>();
        for (auto entity : view) {
            if (registry.has<Player>(entity)) {
                auto& playerPosition = registry.get<entt::Position>(entity);
                entityPositions.player = {playerPosition.p.x, playerPosition.p.y};
            } else if (registry.has<BlinkyChaseTarget>(entity)) {
                auto& blinkyPosition = registry.get<entt::Position>(entity);
                entityPositions.blinky = {blinkyPosition.p.x, blinkyPosition.p.y};
            } else if (registry.has<PinkyChaseTarget>(entity)) {
                auto& pinkyPosition = registry.get<entt::Position>(entity);
                entityPositions.pinky = {pinkyPosition.p.x, pinkyPosition.p.y};
            } else if (registry.has<InkyChaseTarget>(entity)) {
                auto& inkyPosition = registry.get<entt::Position>(entity);
                entityPositions.inky = {inkyPosition.p.x, inkyPosition.p.y};
            } else if (registry.has<ClydeChaseTarget>(entity)) {
                auto& clydePosition = registry.get<entt::Position>(entity);
                entityPositions.clyde = {clydePosition.p.x, clydePosition.p.y};
            }
        }
    }

private:
    Positions entityPositions;
};

} // namespace demo