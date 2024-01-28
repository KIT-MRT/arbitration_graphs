# ARBITRATION GRAPHS

Arbitration graphs combine simple atomic behavior blocks to more complex behaviors for decision making and behavior generation


## Installation

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
