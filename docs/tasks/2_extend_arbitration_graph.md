---
title: "Arbitration Graphs Tutorial"
---

# Task 2: Extend the Arbitration Graph

Extend the arbitration graph with the `ChaseGhost` behavior component.

## Context

With our next behavior component ready to go, we need to think about integrating it into our arbitration graph.

For this purpose, we need to modify the `PacmanAgent` class to include the `ChaseGhost` behavior component we implemented in the [previous task](1_implement_behavior_component.md).
Integrating a new behavior component into the arbitration graph is as simple as instantiating it and adding it as a new option to one of the arbitrators.
Since right now there is just one arbitrator - a priority arbitrator - the choice is simple.
We just need to worry about the order in which the options are assigned to the arbitrator.
Should chasing a ghost have a higher priority than avoiding a ghost or vice versa?

## Goal

Integrate the `ChaseGhost` behavior component into the arbitration graph defined in the `PacmanAgent` class.

## Instructions

- Take a look at how the other behavior components are defined in `include/demo/pacman_agent.hpp`.
- Add the `ChaseGhost` behavior component as a new member of the `PacmanAgent` class and initialize it in the constructor.
- Extend the `PacmanAgent` parameter struct to include the parameters for the `ChaseGhost` behavior component.
- Add a new option to the priority arbitrator.
- Run the game, take a look at the new arbitration graph and observe how PacMan behaves.

## Solution

<details>
<summary>Click here to expand the solution</summary>

Include the header of the `ChaseGhost` behavior component in `include/demo/pacman_agent.hpp`:
```cpp
#include "chase_ghost_behavior.hpp"
```

Add the `ChaseGhost` behavior component as a new member of the `PacmanAgent` class:
```cpp
private:
    ChaseGhostBehavior::Ptr chaseGhostBehavior_;
```

Extend the `PacmanAgent` parameter struct to include the parameters for the `ChaseGhost` behavior component:
```cpp
struct Parameters {
    AvoidGhostBehavior::Parameters avoidGhostBehavior;
    // Add the parameters for the ChaseGhost behavior component
    ChaseGhostBehavior::Parameters chaseGhostBehavior;
    MoveRandomlyBehavior::Parameters moveRandomlyBehavior;
};
```

In the constructor of the `PacmanAgent` class, initialize the `ChaseGhost` behavior component and add it to the priority arbitrator:
```cpp
explicit PacmanAgent(const entt::Game& game)
        : parameters_{}, environmentModel_{std::make_shared<EnvironmentModel>(game)} {

    avoidGhostBehavior_ = std::make_shared<AvoidGhostBehavior>(environmentModel_, parameters_.avoidGhostBehavior);
    // Initialize the ChaseGhost behavior component
    chaseGhostBehavior_ = std::make_shared<ChaseGhostBehavior>(environmentModel_, parameters_.chaseGhostBehavior); 
    moveRandomlyBehavior_ = std::make_shared<MoveRandomlyBehavior>(parameters_.moveRandomlyBehavior);

    rootArbitrator_ = std::make_shared<PriorityArbitrator>("Pacman");
    // Add the ChaseGhost behavior component to the priority arbitrator (before the AvoidGhost behavior component!)
    rootArbitrator_->addOption(chaseGhostBehavior_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
    rootArbitrator_->addOption(avoidGhostBehavior_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
    rootArbitrator_->addOption(moveRandomlyBehavior_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
}
```
</details>


---
[← Previous task](1_implement_behavior_component.md)
|
[Tutorial Home](../Tutorial.md)
|
[Next task →](3_add_more_behaviors.md)
