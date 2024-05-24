#pragma once

#include <pacman/comp/chase_target.hpp>
#include <pacman/comp/player.hpp>
#include <pacman/core/maze.hpp>

#include "types.hpp"
#include "demo/astar.hpp"

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
        astar_ = AStar(mazeState_);
    }

    Position pacmanPosition() const {
        return entityPositions_.pacman;
    }
    Position closestGhostPosition() const;

    int distance(const Position& start, const Position& goal) const {
        return astar_.distance(start, goal);
    }
    double closestGhostDistance() const {
        return distance(pacmanPosition(), closestGhostPosition());
    }

    bool isWall(const Position& position) const {
        return mazeState_[{position.x,position.y}] == Tile::wall;
    }

protected:
    void updatePositions(entt::registry& registry);

    PositionStore entityPositions_;
    entt::MazeState mazeState_;
    AStar astar_;
};

} // namespace demo