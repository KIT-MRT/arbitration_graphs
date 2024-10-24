---
title: "Task 3: Even more behavior components"
description: Integrate the EatClosestDot behavior component into the arbitration graph.
---

# Eat Closest Dot

## Context

So far, we have not looked into a behavior component that handles the most important aspect of the game: eating dots.
So now is a good a time as any to further extend our arbitration graph with the `EatClosestDot` behavior component.

We don't want to bore you with the details of planning a path through a PacMan maze, so we have already implemented that for you.
You just need to integrate it into the arbitration graph, very similarly to the last task.

We'll keep it simple and just add it as another option to the priority arbitrator.
Arbitration graphs can be nested of course, but we'll save that for the next task.

## Goal

Integrate the `EatClosestDot` behavior component into the arbitration graph defined in the `PacmanAgent` class.

## Instructions

- Integrate the new component just like you did in the last task.
- Start the game and see how PacMan stop wandering around aimlessly and starts eating dots.

## Solution

<details>
<summary>Click here to expand the solution</summary>

Include the header of the `EatClosestDot` behavior component in `include/demo/pacman_agent.hpp`:
```cpp
#include "eat_closest_dot_behavior.hpp"
```

Add the `ChaseGhost` behavior component as a new member of the `PacmanAgent` class:
```cpp
private:
    EatClosestDotBehavior::Ptr eatClosestDotBehavior_;
```

In the constructor of the `PacmanAgent` class, initialize the `ChaseGhost` behavior component and add it to the priority arbitrator:
```cpp
explicit PacmanAgent(const entt::Game& game)
        : parameters_{}, environmentModel_{std::make_shared<EnvironmentModel>(game)} {

    avoidGhostBehavior_ = std::make_shared<AvoidGhostBehavior>(environmentModel_, parameters_.avoidGhostBehavior);
    chaseGhostBehavior_ = std::make_shared<ChaseGhostBehavior>(environmentModel_, parameters_.chaseGhostBehavior); 
    // Initialize the EatClosestDot behavior component
    eatClosestDotBehavior_ = std::make_shared<EatClosestDotBehavior>(environmentModel_);
    moveRandomlyBehavior_ = std::make_shared<MoveRandomlyBehavior>(parameters_.moveRandomlyBehavior);

    rootArbitrator_ = std::make_shared<PriorityArbitrator>("Pacman");
    rootArbitrator_->addOption(chaseGhostBehavior_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
    rootArbitrator_->addOption(avoidGhostBehavior_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
    // Add the EatClosestDot behavior component to the priority arbitrator (after the ghost behavior components!)
    rootArbitrator_->addOption(eatClosestDotBehavior_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
    rootArbitrator_->addOption(moveRandomlyBehavior_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
}
```
</details>



---
[← Previous task](2_extend_arbitration_graph.md)
|
[Tutorial Home](../Tutorial.md)
|
[Next task →](4_nested_arbitrators.md)
