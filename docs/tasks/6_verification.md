---
title: "Arbitration Graphs Tutorial"
menu_title: "Verification and Fallbacks"
---

# Task 6: Better safe than sorry

Execute only safe commands and add a fallback strategy.

## Context

The arbitration graph is now complete and PacMan is eating dots like a pro.
But there is one last topic we want to talk about: **safety and robustness**.

Depending on your application, you might only want to execute commands that you know meet certain criteria. 
The specific requirements will depend on your application and could be anything from physical constraints to safety requirements.
In our case, we only want to execute commands where PacMan does not run into walls.

We can ensure that commands obey these requirements by adding a verifier to the arbitrators.
The arbitrator will then run the **verification step** and only choose commands that pass this step.

The leads us to another issue.
What to do if the command we wanted to execute does not pass the verification step?

Glad you asked!
The first thing that happens out-of-the-box: the arbitrator will just choose the next best option passing verification.
E.g., if the `EatClosestDot` is not safe, the `EatDot` arbitrator will just return the `ChangeDotCluster` command to the root arbitrator
  in case `ChangeDotCluster` is both applicable and does itself pass verification.

If that's not the case though, we can think about adding additional behavior components as fallback layers to enable **graceful degradation** of the system.
The first one is already there: `MoveRandomly` is something we probably don't really want to do under normal circumstances.
But if we run out of ideas, it is still a valid option.
It might also give our main behavior components a chance to recover or to solve deadlock situations.

Finally, it is a good idea to add a **last resort** fallback layer.
This behavior component should be a simple implementation that is always applicable and does not require a lot of context knowledge.
If the system is in a failing state, the latter might not be available.
We can mark a behavior component as last resort fallback layer in order to exclude it from verification.
After all, it's our last straw and it's better to execute that than to do nothing.

In our case, we will add a `StayInPlace` behavior component.
PacMan is not actually able to stop, so he will just keep moving back and forth.
Probably not an ideal strategy to win the game, but we can be sure to have a comprehensible command at all times.
Also, PacMan will never run into a wall with this behavior component.

Phew, that was long read. Time to get our hands dirty!


## Goal

Finish the implementation of the `Verifier` class and have the existing arbitrators use it.
Add the `MoveRandomly` behavior component as a last resort fallback layer.

## Instructions

- In `verifier.cpp`, finish the implementation of the `Verifier::analyze()` method.
- Add an instance of the `Verifier` to the `PacmanAgent` class and initialize it in the constructor.
- Pass the `Verifier` instance to the constructors of the arbitrators.
  (Hint: You'll need to adjust the template parameters of the arbitrators.)
- Add the `StayInPlace` behavior component analogously to the other behavior components.
- Mark the `StayInPlace` behavior component as a last resort fallback layer.
- Try breaking a behavior component on purpose and see how the system reacts.
  (Try throwing an exception in the `getCommand()` method of a behavior component or returning a command that will lead to a collision with a wall.)

## Solution

<details>
<summary>Click here to expand the solution</summary>

In the `Verifier::analyze()` method (in `include/demo/verifier.hpp`), we simply check if the command would lead to an invalid position:
```cpp
VerificationResult analyze(const Time /*time*/, const Command& command) const {
    Move nextMove = Move{command.path.front()};
    Position nextPosition = environmentModel_->pacmanPosition() + nextMove.deltaPosition;

    // The command is considered safe if the next position is in bounds and not a wall
    return VerificationResult{environmentModel_->isPassableCell(nextPosition)};
}
```

Include the verifier header you just implemented, in `include/demo/pacman_agent.hpp`.
Also, include `stay_in_place_behavior.hpp`.
```cpp
#include "stay_in_place_behavior.hpp"
#include "verifier.hpp"
```

Adjust the template parameters in the alias definitions to contain the verifier types:
```cpp
public:
    using CostArbitrator = arbitration_graphs::CostArbitrator<Command, Command, Verifier, VerificationResult>;
    using PriorityArbitrator = arbitration_graphs::PriorityArbitrator<Command, Command, Verifier, VerificationResult>;
```

Add the verifier and the fallback behavior component as members of the `PacmanAgent` class:
```cpp
private:
    StayInPlaceBehavior::Ptr stayInPlaceBehavior_;

    Verifier verifier_;
```

In the constructor of the `PacmanAgent` class, initialize the verifier and the `StayInPlace` behavior component.
Make sure to also pass the verifier to the arbitrator constructors:
```cpp
    explicit PacmanAgent(const entt::Game& game)
            : parameters_{},
              environmentModel_{std::make_shared<EnvironmentModel>(game)},
              verifier_{environmentModel_} // We can initialize the verifier in the member initializer list {

        avoidGhostBehavior_ = std::make_shared<AvoidGhostBehavior>(environmentModel_, parameters_.avoidGhostBehavior);
        changeDotClusterBehavior_ = std::make_shared<ChangeDotClusterBehavior>(environmentModel_);
        chaseGhostBehavior_ = std::make_shared<ChaseGhostBehavior>(environmentModel_, parameters_.chaseGhostBehavior);
        eatClosestDotBehavior_ = std::make_shared<EatClosestDotBehavior>(environmentModel_);
        moveRandomlyBehavior_ = std::make_shared<MoveRandomlyBehavior>(parameters_.moveRandomlyBehavior);
        // Initialize the StayInPlace behavior component
        stayInPlaceBehavior_ = std::make_shared<StayInPlaceBehavior>(environmentModel_);

        // Pass the verifier instance to the cost arbitrator
        eatDotsArbitrator_ = std::make_shared<CostArbitrator>("EatDots", verifier_);
        costEstimator_ = std::make_shared<CostEstimator>(environmentModel_, parameters_.costEstimator);
        eatDotsArbitrator_->addOption(
            changeDotClusterBehavior_, CostArbitrator::Option::Flags::INTERRUPTABLE, costEstimator_);
        eatDotsArbitrator_->addOption(
            eatClosestDotBehavior_, CostArbitrator::Option::Flags::INTERRUPTABLE, costEstimator_);

        // Pass the verifier instance to the priority arbitrator
        rootArbitrator_ = std::make_shared<PriorityArbitrator>("Pacman", verifier_);
        rootArbitrator_->addOption(chaseGhostBehavior_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
        rootArbitrator_->addOption(avoidGhostBehavior_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
        rootArbitrator_->addOption(eatDotsArbitrator_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
        rootArbitrator_->addOption(moveRandomlyBehavior_, PriorityArbitrator::Option::Flags::INTERRUPTABLE);
        // Add the StayInPlace behavior component. Mark it as a last resort fallback layer using the FALLBACK flag.
        rootArbitrator_->addOption(stayInPlaceBehavior_,
                                   PriorityArbitrator::Option::Flags::INTERRUPTABLE |
                                       PriorityArbitrator::Option::FALLBACK);
    }
```
</details>


---
[← Previous task](5_cost_arbitration.md)
|
[Tutorial Home](../Tutorial.md)
