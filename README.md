# Arbitration Graphs

[![Latest Release](https://img.shields.io/github/v/release/KIT-MRT/arbitration_graphs?color=green)](https://github.com/KIT-MRT/arbitration_graphs/releases)
[![License](https://img.shields.io/github/license/KIT-MRT/arbitration_graphs)](./LICENSE)
[![Unit Test Status](https://img.shields.io/github/actions/workflow/status/KIT-MRT/arbitration_graphs/run-unit-tests.yaml?branch=main&label=tests)](https://github.com/KIT-MRT/arbitration_graphs/actions/workflows/run-unit-tests.yaml?query=branch%3Amain)

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
  Behavior components address *How to do it?* and *Can we do it?*, while Arbitrators decide *What to do?*
 - 🧠 **Meta-framework**  
  Integrate diverse methods in one decision-making framework. Why not combine optimization-based planning, probabilistic approaches (POMDPs), and machine learning (RL)? Use any approach where it performs best!
- 📈 **Scalability**  
  Stack behavior components in arbitrators to create hierarchical behavior models.
- 🛠️ **Maintainability**  
  Add new behaviors without having to touch existing ones – did we mention strict modularity and functional decomposition?
- 💡 **Transparency**  
  Easily follow and understand the decision-making process.
- 🛡️ **Behavior Verification**  
  Use tightly integrated verifiers to ensure that only safe and valid behavior commands are executed.
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

Open the GUI with your favorite browser:  
[http://0.0.0.0:8080](http://0.0.0.0:8080)

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


We will shortly add a [tutorial](https://github.com/KIT-MRT/arbitration_graphs/pull/51) based on this demo – stay tuned! 


## Installation

<details>
<summary>Prerequisites</summary>

First make sure all dependencies are installed:
- [glog](https://github.com/google/glog)
- [Googletest](https://github.com/google/googletest) (optional, if you want to build unit tests)
- [yaml-cpp](https://github.com/jbeder/yaml-cpp)
- [util_caching](https://github.com/KIT-MRT/util_caching)
- [Crow](https://crowcpp.org) (optional, needed for GUI only)

See also the [`Dockerfile`](./Dockerfile) for how to install these packages under Debian or Ubuntu.
</details>

<details>
<summary>Installation using Debian package (recommended)</summary>

We provide a Debian package for easy installation on Debian-based distributions.
Download the latest `.deb` packages for the [core library](https://github.com/KIT-MRT/arbitration_graphs/releases/latest/download/libarbitration-graphs-core-dev.deb)
and optionally for [the gui](https://github.com/KIT-MRT/arbitration_graphs/releases/latest/download/libarbitration-graphs-gui-dev.deb) install them with `dpkg`:

```bash
sudo dpkg -i libarbitration-graphs-core-dev.deb
sudo dpkg -i libarbitration-graphs-gui-dev.deb
```
</details>

<details>
<summary>Using Docker image with pre-installed library</summary>

We provide a Docker image with the library and all dependencies already installed globally.

```bash
docker pull ghcr.io/kit-mrt/arbitration_graphs
```

The library is located under `/usr/local/include/arbitration_graphs/` and `/usr/local/lib/cmake/arbitration_graphs/`.
So, it can be easily loaded with CMake:

```cmake
find_package(arbitration_graphs REQUIRED)
```

</details>

<details>
<summary>Building from source using CMake</summary>

Clone the repository:

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

In order to skip compiling the GUI, use `cmake -DBUILD_GUI=false ..` instead.

</details>


## Development

<details>
<summary>Using Docker image</summary>

Clone the repository and run the development image

```bash
git clone https://github.com/KIT-MRT/arbitration_graphs.git
cd arbitration_graphs
docker compose build
docker compose run --rm arbitration_graphs_devel
```

This mounts the source into the container's `/home/blinky/arbitration_graphs` folder.
There, you can edit the source code, compile and run the tests etc.

</details>


<details>
<summary>Compiling unit tests</summary>

In order to compile with tests define `BUILD_TESTS=true`
```bash
mkdir -p arbitration_graphs/build
cd arbitration_graphs/build
cmake -DBUILD_TESTS=true ..
cmake --build . -j9
```

Run all unit tests with CTest:

```bash
cmake --build . --target test
```

</details>


<details>
<summary>Serving the WebApp GUI</summary>

The GUI consists of
- a web server with static WebApp files, see [`gui/app`](./gui/app)
- a websocket server providing the arbitration graph state to the WebApp

In order to serve the WebApp files, their location must be known to the executable running the web server.
By default (and without further setup), we support these locations:
- the install path, i.e. `/opt/share/arbitration_graphs`
- the current source path for local builds, i.e. `/home/blinky/arbitration_graphs/gui/app/arbitration_graphs` (only works, if no installation has been found)

If you intend to override these, please use the `APP_DIRECTORY` environment variable to define the WebApp path:

```
APP_DIRECTORY=/my/custom/app/path my_awesome_executable
```

</details>

