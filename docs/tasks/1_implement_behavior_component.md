---
title: "Task 1: Implement a Behavior Component"
description: Implement your first checkInvocationCondition and getCommand function, such that the ChaseGhost behavior component passes its unit tests.
---

# Chase Ghost

## Context

Before we start building our arbitration graph, we want to take a closer look into behavior components.
Don't worry, most of the behavior components are already implemented for you
  but we want to make sure you have an idea of how they work.

With the current state of the arbitration graph, PacMan will just move around randomly until a ghost gets too close.
That's great and all but if we ate a power pellet, we want to chase the ghosts to eat them for extra points.

To do this, we need to implement the `ChaseGhost` behavior component.
It essentially does the exact opposite of the `AvoidGhost` behavior component
  but is only applicable when PacMan ate a power pellet.
We can ensure that's always the case using the behavior's invocation condition.

But wait - the current implementation of the invocation condition is not complete.
It should only be applicable if there is one of these tasty ghosts close by.

Once that's out of the way, we'll take a closer look at the `getCommand` function which is missing some core logic right now.

## Goal

Finish the implementation of the `checkInvocationCondition` and `getCommand` functions
  of the `ChaseGhost` behavior component such that it passes its unit tests.

## Instructions

- Open the implementation of the `ChaseGhost` behavior component in `src/chase_ghost_behavior.cpp`.
- The `checkInvocationCondition` function is already implemented but does not check for the presence of a ghost.
- Implement the missing piece. Take a look at the implementation of `AvoidGhostBehavior::checkInvocationCondition` if you need inspiration.
- The `getCommand` function is partially implemented but the core logic is missing.
- Implement the missing piece. Take a look at the implementation of `AvoidGhostBehavior::getCommand` if you need inspiration.
- Compile and run the unit tests for the `ChaseGhost` behavior component to verify that your implementation is correct.


---
[Tutorial Home](../Tutorial.md)
|
[Next task →](2_extend_arbitration_graph.md)
