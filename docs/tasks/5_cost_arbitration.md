---
title: "Arbitration Graphs Tutorial"
menu_title: "Cost Arbitrator"
---

# Task 5: Arbitrate based on predicted utility

Learn how the cost arbitrator can help you to arbitrate between behaviors based on their expected cost/utility.

## Context

The `EatDot` arbitrator we added in the [previous task](4_nested_arbitrators.md) decides between the two dot eating strategies randomly.
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
  Don't forget to include the necessary headers and extend the parameter struct with the parameters for the `CostEstimator`.
- Replace the random arbitrator with a cost arbitrator in the `PacmanAgent` class. Pass the `CostEstimator` instance to the `addOption()` method.

## Solution

<details>
<summary>Click here to expand the solution</summary>

Finish the implementation of the `CostEstimator` class in `cost_estimator.cpp`:
```cpp
double CostEstimator::estimateCost(const Command& command, bool /*isActive*/) {
    Positions absolutePath = utils::toAbsolutePath(command.path, environmentModel_);

    // Compute the number of dots along the path and in the neighborhood of the path end using helper functions
    const int nDotsAlongPath = utils::dotsAlongPath(absolutePath, environmentModel_);
    const int nDotsInRadius =
        utils::dotsInRadius(absolutePath.back(), environmentModel_, parameters_.pathEndNeighborhoodRadius);
    const int nDots = nDotsAlongPath + nDotsInRadius;

    if (nDots == 0) {
        return std::numeric_limits<double>::max();
    }

    // Compute the size of the path and the neighborhood of the path end
    const int pathLength = static_cast<int>(absolutePath.size());
    const int neighborhoodSize = static_cast<int>(std::pow(2 * parameters_.pathEndNeighborhoodRadius + 1, 2));
    const int nCells = pathLength + neighborhoodSize;

    // We can define a cost as the inverse of a benefit.
    // Our benefit is a dot density (number of dots / number of examined cells)
    return static_cast<double>(nCells) / nDots;
} 
```

Replace the include of the random arbitrator with the cost arbitrator in `include/demo/pacman_agent.hpp`.
Also, include `cost_estimator.hpp`:
```cpp
#include <arbitration_graphs/cost_arbitrator.hpp>

#include "cost_estimator.hpp"
```

Change the type of the `eatDotsArbitrator_` member in the `PacmanAgent` class to `CostArbitrator` and add an instance of the `CostEstimator`:
```cpp
private:
    CostArbitrator::Ptr eatDotsArbitrator_;

    CostEstimator::Ptr costEstimator_;
```

Extend the `Parameters` struct to contain the parameters for the `CostEstimator`:
```cpp
struct Parameters {
    AvoidGhostBehavior::Parameters avoidGhostBehavior;
    ChaseGhostBehavior::Parameters chaseGhostBehavior;
    MoveRandomlyBehavior::Parameters moveRandomlyBehavior;

    // Add the parameters for the CostEstimator
    CostEstimator::Parameters costEstimator;
};
```

As always, the magic happens in the constructor of the `PacmanAgent` class.
Instantiate the cost estimator and pass it in the `addOption` calls:
```cpp
explicit PacmanAgent(const entt::Game& game)
        : parameters_{}, environmentModel_{std::make_shared<EnvironmentModel>(game)} {

    avoidGhostBehavior_ = std::make_shared<AvoidGhostBehavior>(environmentModel_, parameters_.avoidGhostBehavior);
    changeDotClusterBehavior_ = std::make_shared<ChangeDotClusterBehavior>(environmentModel_);
    chaseGhostBehavior_ = std::make_shared<ChaseGhostBehavior>(environmentModel_, parameters_.chaseGhostBehavior);
    eatClosestDotBehavior_ = std::make_shared<EatClosestDotBehavior>(environmentModel_);
    moveRandomlyBehavior_ = std::make_shared<MoveRandomlyBehavior>(parameters_.moveRandomlyBehavior);

    // This is now a cost arbitrator
    eatDotsArbitrator_ = std::make_shared<CostArbitrator>("EatDots");
    // Construct the cost estimator
    costEstimator_ = std::make_shared<CostEstimator>(environmentModel_, parameters_.costEstimator);
    // Add the ChangeDotCluster and EatClosestDot behavior components as options to the
    // cost arbitrator while also passing the cost estimator
    eatDotsArbitrator_->addOption(
        changeDotClusterBehavior_, CostArbitrator::Option::Flags::INTERRUPTABLE, costEstimator_);
    eatDotsArbitrator_->addOption(
        eatClosestDotBehavior_, CostArbitrator::Option::Flags::INTERRUPTABLE, costEstimator_);

    rootArbitrator_ = std::make_shared<PriorityArbitrator>("Pacman");
    rootArbitrator_->addOption(chaseGhostBehavior_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
    rootArbitrator_->addOption(avoidGhostBehavior_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
    rootArbitrator_->addOption(eatDotsArbitrator_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
    rootArbitrator_->addOption(moveRandomlyBehavior_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
    rootArbitrator_->addOption(stayInPlaceBehavior_,
                               PriorityArbitrator::Option::Flags::INTERRUPTABLE |
                                   PriorityArbitrator::Option::FALLBACK);
}
```

</details>


---
[← Previous task](4_nested_arbitrators.md)
|
[Tutorial Home](../Tutorial.md)
|
[Next task →](6_verification.md)
