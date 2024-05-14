#pragma once

#include <comp/chase_target.hpp>
#include <comp/player.hpp>

#include "types.hpp"
#include "core/maze.hpp"

namespace demo {

class EnvironmentModel {
public:
    using Ptr = std::shared_ptr<EnvironmentModel>;
    using ConstPtr = std::shared_ptr<const EnvironmentModel>;

    struct PositionStore {
        Positions ghostPositions() const {
            return {blinky, pinky, inky, clyde};
        }

        Position pacman;
        Position blinky;
        Position pinky;
        Position inky;
        Position clyde;
    };

    void update(entt::registry& registry, const entt::MazeState& mazeState) {
        updatePositions(registry);
        mazeState_ = mazeState;
    }

    Position pacmanPosition() const {
        return entityPositions_.pacman;
    }
    Position closestGhostPosition() const;
    double closestGhostDistance() const {
        return pacmanPosition().distance(closestGhostPosition());
    }

    bool isWall(const Position& position) const {
        return mazeState_[{position.x,position.y}] == Tile::wall;
    }

protected:
    void updatePositions(entt::registry& registry);

    PositionStore entityPositions_;
    entt::MazeState mazeState_;
};

} // namespace demo