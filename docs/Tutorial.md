# Arbitration Graphs Tutorial

Let's write an agent for the famous PacMan game using Arbitration Graphs 🕹️

tl;dr: Find links to the individual tasks at the bottom of this page.
## Introduction


### Goal

The goal of this tutorial is to help you understand how to use the Arbitration Graphs library.
To keep things interesting, we will re-implement some parts of our PacMan demo.

We'll start by looking into the implementation of a single behavior component
  and then learn how to integrate it into an arbitration graph using a simple priority arbitrator.

We'll then start adding more and more behavior components to the graph and learn about other aspects of the library
  such as cost arbitrators, nested structures and verification.

The tutorial is structured into several tasks that are meant to be completed in order.

### What to find where

Each behavior component is implemented as a separate class inheriting from the abstract `Behavior` class.
The header files can be found in the `demo/include/demo/` directory
  and with corresponding implementation files in the `demo/src/` directory.

Next, there is the `EnvironmentModel` class.
You guessed it, it contains the environment model for the arbitration graph.
In it, we store things like current positions of PacMan and the ghosts, the maze, several utility functions
  and other things required by the behavior components.

Finally, there is the `PacmanAgent` class.
This is where we assemble the behavior components into an arbitration graph.
It's also where you'll spend most of your time during this tutorial.

If you are interested in how the demo works in detail,
  you'll find additional code not directly relevant to the arbitration graphs concept in the `demo/include/utils/` directory

Everything described above is thoroughly tested using the unit tests you'll find in the `demo/test/` directory.

### Development Environment

The easiest way to get started is to use the provided Docker image.
Running
```bash
cd arbitration_graphs/demo
docker compose run --rm tutorial
```
will drop you into a shell with all required dependencies installed and the current directory mounted.

To work on the tutorial, checkout the `tutorial` branch.
```bash
git checkout tutorial
```
Some parts of the demo implementation have been removed on this branch for the purpose of this tutorial.

You can then create a build directory and run CMake to build the project.
You should enable the `BUILD_TESTS` option to build the unit tests as well.

```bash
mkdir -p arbitration_graphs/demo/build
cd arbitration_graphs/demo/build
cmake -DBUILD_TESTS=true ..
cmake --build .
```

You can then run the demo with
```bash
./arbitration_graphs_pacman_demo_exe
```

You'll also find the individual unit executables in this directory.
To execute them all at once, run
```bash
find -executable -type f -name '*-gtest-*' -exec {} \;
```



## Tasks

With the basics out of the way, let's work through the tasks.

1. [Implement your first behavior component](./tasks/1_implement_behavior_component.md)
2. [Extend the arbitration graph with that behavior](./tasks/2_extend_arbitration_graph.md)
3. [Add even more behavior components](./tasks/3_add_more_behaviors.md)
4. [Learn about nested arbitration graphs](./tasks/4_nested_arbitrators.md)
5. [Arbitrate based on predicted utility](./tasks/5_cost_arbitration.md)


- [ ] Finish chase ghost finish so that is passes unit tests by re-implementing some removed part, e.g. getCommand
- [ ] Integrate chase ghost into priority arbitrator
- [ ] Integrate eat closest dot behavior
- [ ] Integrate eat dot arbitrator as random arbitrator
- [ ] Replace arbitrator from previous step with cost arbitrator, finish cost estimator

## General TODOs
- [ ] Add a bit of an explanation about arbitration graphs to top-level readme
