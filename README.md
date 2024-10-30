# Arbitration Graphs

[![License](https://img.shields.io/github/license/KIT-MRT/arbitration_graphs)](./LICENSE)

Arbitration graphs combine simple atomic behavior components into more complex behaviors for decision-making and behavior generation

## Demonstration

We are currently working on a demonstration of this library using Pac-Man as an example application.

<p align="center">
  <img src="docs/assets/img/pacman_scenario_cropped.png" width="300" />
</p>
<p align="center">
  <img src="docs/assets/img/pacman_arbitrator_safe.svg" width="500" /> 
</p>

Feel free to take a look at the work in progress in the [pacman-demo branch](https://github.com/KIT-MRT/arbitration_graphs/tree/pacman-demo).

## Installation

First, clone this repository:

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

