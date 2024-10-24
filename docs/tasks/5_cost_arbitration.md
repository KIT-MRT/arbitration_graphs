---
title: "Task 5: Arbitrate based on predicted utility"
description: Learn how the cost arbitrator can help you to arbitrate between behaviors based on their expected cost/utility.
---

# Cost Arbitrator

## Context

The `EatDot` arbitrator we added in the previous task decides between the two dot eating strategies randomly.
That's obviously not the greatest idea.
There must be a better way.

Turns out, there is!
There is another type of arbitrator that might be more suitable for this task: the cost arbitrator.

As the name suggests, the cost arbitrator computes a cost for each command received from its children and selects the one with the lowest cost.
We need some kind of cost function for this, which we will implement in the `CostEstimator` class.

The idea is to reward a planned path that contains lots of dots while also moving into an area with a high dot density.
We prepared the general structure of the `CostEstimator` class for you, you just need to fill in the blanks.

Let's get started!

## Goal

Finish the implementation of the `CostEstimator` and replace the random arbitrator with a cost arbitrator.

## Instructions

- In `cost_estimator.cpp`, fill in the blanks to compute `nDots` and `nCells`.
- Add an instance of the `CostEstimator` to the `PacmanAgent` class and initialize it in the constructor.
- Replace the random arbitrator with a cost arbitrator in the `PacmanAgent` class. Pass the `CostEstimator` instance to the `addOption` method.


---
[← Previous task](4_nested_arbitrators.md)
|
[Tutorial Home](../Tutorial.md)
