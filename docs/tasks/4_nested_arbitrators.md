---
title: "Arbitration Graphs Tutorial"
menu_title: "Nesting"
---

# Task 4: Nested arbitration graphs

Integrate a long-term behavior and add another layer to the arbitration graph.

## Context

We have now implemented a few behavior components and integrated them into the arbitration graph.
So far, all behavior components are children of the root arbitrator.

Let's make things a bit more interesting by adding a long-term behavior component that's also about eating dots.
The `ChangeDotCluster` behavior will move to an area in the maze where there is a higher density of dots.

For now, we'll just decide between the two dot eating strategies using chance.
We can achieve that by adding them to a random arbitrator which is then added as an option to the root arbitrator.

There are more sophisticated ways to decide between behavior components, we'll cover those in the [next task](5_cost_arbitration.md).

## Goal

Add the `EatClosestDot` and `ChangeDotCluster` behavior components to a random arbitrator nested within the root arbitrator.

## Instructions

- Add the `ChangeDotCluster` behavior component as a new member of the `PacmanAgent` class and initialize it in the constructor.
- Add a random arbitrator as a new member of the `PacmanAgent` class, analogous to the priority arbitrator.
- Add the `EatClosestDot` and `ChangeDotCluster` behavior components as options to the random arbitrator.
- Add the random arbitrator as an option to the root arbitrator.
- Run the game and observe how PacMan behaves.

## Solution

<details>
<summary>Click here to expand the solution</summary>

Include the header of the `ChangeDotCluster` behavior component and the random arbitrator in `include/demo/pacman_agent.hpp`:
```cpp
#include <arbitration_graphs/random_arbitrator.hpp>

#include "change_dot_cluster_behavior.hpp"
```

For better code readability, add the following alias near the top of the class definition:
```cpp
using RandomArbitrator = arbitration_graphs::RandomArbitrator<Command, Command>;
```

Add the `ChangeDotCluster` behavior component and the `RandomArbitrator` as a new members of the `PacmanAgent` class:
```cpp
private:
    ChangeDotClusterBehavior::Ptr changeDotClusterBehavior_;

    RandomArbitrator::Ptr eatDotsArbitrator_;
```

In the constructor of the `PacmanAgent` class, initialize the `ChangeDotCluster` behavior component and the `RandomArbitrator`:
Add the `EatClosestDot` and `ChangeDotCluster` behavior components as options to the random arbitrator.
Finally, add the random arbitrator as an option to the root arbitrator:
```cpp
explicit PacmanAgent(const entt::Game& game)
        : parameters_{}, environmentModel_{std::make_shared<EnvironmentModel>(game)} {

    avoidGhostBehavior_ = std::make_shared<AvoidGhostBehavior>(environmentModel_, parameters_.avoidGhostBehavior);
    // Initialize the ChangeDotCluster behavior component
    changeDotClusterBehavior_ = std::make_shared<ChangeDotClusterBehavior>(environmentModel_);
    chaseGhostBehavior_ = std::make_shared<ChaseGhostBehavior>(environmentModel_, parameters_.chaseGhostBehavior);
    eatClosestDotBehavior_ = std::make_shared<EatClosestDotBehavior>(environmentModel_);
    moveRandomlyBehavior_ = std::make_shared<MoveRandomlyBehavior>(parameters_.moveRandomlyBehavior);

    // Initialize the random arbitrator and add the EatClosestDot and ChangeDotCluster behavior components as options
    eatDotsArbitrator_ = std::make_shared<RandomArbitrator>("EatDots");
    eatDotsArbitrator_->addOption( changeDotClusterBehavior_, RandomArbitrator::Option::Flags::INTERRUPTABLE);
    eatDotsArbitrator_->addOption( eatClosestDotBehavior_, RandomArbitrator::Option::Flags::INTERRUPTABLE);

    rootArbitrator_ = std::make_shared<PriorityArbitrator>("Pacman");
    rootArbitrator_->addOption(chaseGhostBehavior_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
    rootArbitrator_->addOption(avoidGhostBehavior_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
    // The EatDot arbitrator is itself an option of the root arbitrator
    rootArbitrator_->addOption(eatDotsArbitrator_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
    rootArbitrator_->addOption(moveRandomlyBehavior_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
}
```


</details>


---
[← Previous task](2_extend_arbitration_graph.md)
|
[Tutorial Home](../Tutorial.md)
|
[Next task →](5_cost_arbitration.md)
