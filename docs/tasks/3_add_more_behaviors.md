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



---
[← Previous task](2_extend_arbitration_graph.md)
|
[Tutorial Home](../Tutorial.md)
|
[Next task →](4_nested_arbitrators.md)
