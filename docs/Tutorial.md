# Arbitration Graphs Tutorial

Let's write an agent for the famous PacMan game using Arbitration Graphs 🕹️

**TL;DR:** Find links to the individual tasks at the bottom of this page.

## Introduction


### Goal

The goal of this tutorial is to help you understand how to use the Arbitration Graphs library.
To keep things interesting, we will re-implement some parts of our PacMan demo.

We'll start by looking into the implementation of a single behavior component
  and then learn how to integrate it into an arbitration graph using a simple priority arbitrator.

Next, we'll start adding more and more behavior components to the graph and learn about other aspects of the library
  such as cost arbitrators, nested structures and verification.

The tutorial is structured into several tasks that are meant to be completed in order.

### What to find where

Let's take a look at the structure and content of the `arbitration_graphs/demo/` directory.

```
demo
├── include
├── src
├── test
└── …
```

All header files can be found in the `include/` directory with corresponding implementation files in the `src/` directory.
The entire demo is thoroughly tested using the unit tests you'll find in the `test/` directory.

Each behavior component is implemented in a separate `<name>_behavior.hpp` file as a class inheriting from the abstract `Behavior` class.

Next, there is `environment_model.hpp`.
You guessed it, it contains the environment model for the arbitration graph.
In it, we store things like current positions of PacMan and the ghosts, the maze, several utility functions
  and other things required by the behavior components.

The `cost_estimator.hpp` file will be relevant for a later task when we cover cost arbitrators.

Similarly, the `verifier.hpp` file will be used to add a verification layer to the arbitration graph near the end of the tutorial.

Finally, in `pacman_agent.hpp`, the behavior components are assembled into an arbitration graph.
It's also where you'll spend most of your time during this tutorial.

If you are interested in how the demo works in detail,
  you'll find additional code not directly relevant to the tutorial in the `include/utils/` directory

### Development Environment

The easiest way to get started is to use the provided docker setup.

Start by checking out the `tutorial` branch where we have removed some parts
  of the demo implementation for the purpose of this tutorial.
```bash
git clone --branch tutorial https://github.com/KIT-MRT/arbitration_graphs.git
```

To start an interactive shell in the docker container with all required dependencies installed
  and the current directory mounted, run
```bash
cd arbitration_graphs/demo
docker compose run --rm tutorial
```

You can then create a build directory and run CMake to build the project.
You should enable the `BUILD_TESTS` option to build the unit tests as well.

```bash
cd /home/blinky/demo
mkdir -p arbitration_graphs/demo/build
cd arbitration_graphs/demo/build
cmake -DBUILD_TESTS=true ..
cmake --build .
```

You can then run the demo with
```bash
./arbitration_graphs_pacman_demo
```

You'll also find the individual unit executables in this directory.
To execute them all at once, run
```bash
find -executable -type f -name '*-gtest-*' -exec {} \;
```

We'll leave the setup of your favorite IDE up to you
  though most modern IDEs should support attaching to a running docker container.


## Tasks

With the basics out of the way, let's work through the tasks.

1. [Implement your first behavior component](./tasks/1_implement_behavior_component.md)
2. [Extend the arbitration graph with that behavior](./tasks/2_extend_arbitration_graph.md)
3. [Add even more behavior components](./tasks/3_add_more_behaviors.md)
4. [Learn about nested arbitration graphs](./tasks/4_nested_arbitrators.md)
5. [Arbitrate based on predicted utility](./tasks/5_cost_arbitration.md)

