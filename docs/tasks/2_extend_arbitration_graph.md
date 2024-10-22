---
title: "Task 2: Extend the Arbitration Graph"
description: Extend the arbitration graph with the ChaseGhost behavior component.
---

# Chase Ghost

## Context

With our next behavior component ready to go, we need to think about integrating it into our arbitration graph.

For this purpose, we need to modify the `PacmanAgent` class to include the `ChaseGhost` behavior component we implemented in the previous task.
Integrating a new behavior component into the arbitration graph is as simple as instantiating it and adding it as a new option to one of the arbitrators.
Since right now there is just one arbitrator - a priority arbitrator - the choice is simple.
We just need to worry about the order in which the options are assigned to the arbitrator.
Should chasing a ghost have a higher priority than avoiding a ghost or vice versa?

## Goal

Integrate the `ChaseGhost` behavior component into the arbitration graph defined in the `PacmanAgent` class.

## Instructions

- Take a look at how the other behavior components are defined in `include/demo/pacman_agent.hpp`.
- Add the `ChaseGhost` behavior component as a new member of the `PacmanAgent` class and initialize it in the constructor.
- Add a new option to the priority arbitrator.
- Run the game, take a look at the new arbitration graph and observe how PacMan behaves.


---
[← Previous task](1_implement_behavior_component.md)
|
[Tutorial Home](../Tutorial.md)
|
[Next task →](3_add_more_behaviors.md)
