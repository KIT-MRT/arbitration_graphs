---
title: "Task 4: Nested arbitration graphs"
description: Integrate a long-term behavior and add another layer to the arbitration graph
---

# Nesting

## Context

We have now implemented a few behavior components and integrated them into the arbitration graph.
So far, all behavior components are children of the root arbitrator.

Let's make things a bit more interesting by adding a long-term behavior component that's also about eating dots.
The `ChangeDotCluster` behavior will move to an area in the maze where there is a higher density of dots.

For now, we'll just decide between the two dot eating strategies using chance.
We can achieve that by adding them to a random arbitrator which is then added as an option to the root arbitrator.

There are more sophisticated ways to decide between behavior components, we'll cover those in the next task.

## Goal

Add the `EatClosestDot` and `ChangeDotCluster` behavior components to a random arbitrator nested within the root arbitrator.

## Instructions

- Add the `ChangeDotCluster` behavior component as a new member of the `PacmanAgent` class and initialize it in the constructor.
- Add a random arbitrator as a new member of the `PacmanAgent` class, analogous to the priority arbitrator.
- Add the `EatClosestDot` and `ChangeDotCluster` behavior components as options to the random arbitrator.
- Add the random arbitrator as an option to the root arbitrator.
- Run the game and observe how PacMan behaves.


---
[← Previous task](3_add_more_behaviors.md)
|
[Tutorial Home](../Tutorial.md)
|
[Next task →](5_cost_arbitration.md)
