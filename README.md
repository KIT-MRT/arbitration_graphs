# Arbitration Graphs

[![License](https://img.shields.io/github/license/KIT-MRT/arbitration_graphs)](./LICENSE)

**Hierarchical behavior models for complex decision-making and behavior generation in robotics!**

<!-- 
TODO: add example arbitration graph from robotics (with POMDP, RL methods)
<img
  src="./docs/assets/img/pacman_arbitrator_safe.svg"
  alt="Example arbitration graph from robotics"
  style="margin:10px; width: 50%"
  align="right"
/>
-->

- 🌱 **Bottom-up**  
  Combine simple atomic behavior components to generate complex behaviors.
- 🧩 **Functional decomposition**  
  Behavior components define the *How to do it?* and *Can we do it?*, while Arbitrators take the decision on *What to do?*
 - 🧠 **Meta-framework**  
  Integrate diverse methods in one decision-making framework. Why not combine optimization-based planning, probabilistic approaches (POMDPs), and machine learning (RL)? Use any approach where it performs best!
- 📈 **Scalability**  
  Stack behavior components in arbitrators to create hierarchical behavior models.
- 🛠️ **Maintainability**  
  Add new behaviors without having to touch others – did we mention strict modularity and functional decomposition?
- 💡 **Transparency**  
  Easily follow and understand the decision-making process.
- 🛡️ **Behavior Verification**  
  Use tightly integrated verifiers to ensure only valid and safe behavior commands are executed.
- 🪂 **Graceful Degradation**  
  Your behavior is unreliable or unsafe? Arbitrators will gracefully fall back to the next-best option.


## Demo

We provide a demo of this library using Pac-Man as an example application.  
The arbitration graph controls Pac-Man on its journey to collect tasty dots 🍬

Run the demo with:

```bash
git clone https://github.com/KIT-MRT/arbitration_graphs.git
cd arbitration_graphs/demo
docker compose up
```

### Explanation

You will see the *Pacman Agent* arbitrator selecting between five behavior options (by priority).  
The *Eat Dots* option is itself an arbitrator with two sub-behaviors (selecting by expected benefit).

In this scene,
- the *Chase Ghost* and *Avoid Ghost* behaviors are not applicable (no ghosts in close vicinity) → grey background,
- the *Eat Closest Dot* and *Move Randomly* behavior failed verification (our verification showcase) → red background,
- thus, the least-prioritized *Stay in Place* behavior is being executed → green background.

<p align="center">
  <img src="docs/assets/img/pacman_scenario_cropped.png" width="300" />
</p>
<p align="center">
  <img src="docs/assets/img/pacman_arbitrator_safe.svg" width="500" /> 
</p>


We will shortly add an [arbitration graph GUI](https://github.com/KIT-MRT/arbitration_graphs/pull/10) and a [tutorial](https://github.com/KIT-MRT/arbitration_graphs/pull/51) based on this demo – stay tuned! 


## Installation

From easy to advanced:

### Using Docker image

We provide a Docker image with the library already installed globally.

```bash
docker pull ghcr.io/kit-mrt/arbitration_graphs
```

The library is located under `/usr/local/include/arbitration_graphs/` and `/usr/local/lib/cmake/arbitration_graphs/`.
So, it can be easily loaded with CMake:

```cmake
find_package(arbitration_graphs REQUIRED)
```


### Building from source using CMake

First make sure all dependencies are installed:
- [glog](https://github.com/google/glog)
- [Googletest](https://github.com/google/googletest) (only if you want to build unit tests)
- [yaml-cpp](https://github.com/jbeder/yaml-cpp)
- [util_caching](https://github.com/KIT-MRT/util_caching)

See also the [`Dockerfile`](./Dockerfile) for how to install these packages under Debian or Ubuntu.

Now, clone the repository:

```bash
git clone https://github.com/KIT-MRT/arbitration_graphs.git
cd arbitration_graphs
```

Compile and install the project with CMake:

```bash
mkdir -p arbitration_graphs/build
cd arbitration_graphs/build
cmake ..
cmake --build .
sudo cmake --install .
```


## Development

### Using Docker image

Clone the repository and run the development image

```bash
git clone https://github.com/KIT-MRT/arbitration_graphs.git
cd arbitration_graphs
docker compose build
docker compose run --rm arbitration_graphs_devel
```

This mounts the source into the container's `/home/blinky/arbitration_graphs` folder.
There, you can edit the source code, compile and run the tests etc.


### Compiling unit tests

In order to compile with tests define `BUILD_TESTS=true`
```bash
mkdir -p arbitration_graphs/build
cd arbitration_graphs/build
cmake -DBUILD_TESTS=true ..
cmake --build .
```

Run all unit tests:

```bash
find -executable -type f -name '*-gtest-*' -exec {} \;
```

