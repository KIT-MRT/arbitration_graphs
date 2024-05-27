# ARBITRATION GRAPHS

Arbitration graphs combine simple atomic behavior blocks to more complex behaviors for decision making and behavior generation


## Installation

First, clone this repository:

```bash
git clone https://github.com/KIT-MRT/arbitration_graphs.git
cd arbitration_graphs
```


### Using Docker image

We provide a [`Dockerfile`](./Dockerfile) with the library already installed globally.

In the source directory, build and run the docker image with `docker compose`:

```bash
docker compose build
docker compose run --rm arbitration_graphs
```

The library is installed in the Docker image under `/usr/local/include/arbitration_graphs/` and `/usr/local/lib/cmake/arbitration_graphs/`.
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

Follow the steps above to setup the Docker image.
Then, run the development image.

```bash
docker compose -f docker-compose.devel.yaml build
docker compose -f docker-compose.devel.yaml run --rm arbitration_graphs_devel
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

