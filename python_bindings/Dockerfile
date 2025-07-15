# ==========
# Base image
# ==========

ARG VERSION=latest
FROM ghcr.io/kit-mrt/arbitration_graphs:$VERSION AS base

RUN apt-get update && \
    apt-get install -y \
      ninja-build \
      pybind11-dev \
      python3-dev \
      python3-pip && \
    apt-get clean


# ===========
# Development
# ===========

FROM base AS devel

RUN useradd --create-home --uid 1000 blinky
USER blinky

WORKDIR /home/blinky/


# ==========
# Unit tests
# ==========

FROM base AS unit_test

COPY CMakeLists.txt /tmp/arbitration_graphs/
COPY README.md /tmp/arbitration_graphs/
COPY pyproject.toml /tmp/arbitration_graphs/
COPY version /tmp/arbitration_graphs/
COPY include /tmp/arbitration_graphs/include
COPY src /tmp/arbitration_graphs/src
COPY test /tmp/arbitration_graphs/test

WORKDIR /tmp/arbitration_graphs
RUN pip install .

CMD ["python3", "-m", "unittest", "discover", "-s", "test"]


# =============
# Wheel builder
# =============

FROM quay.io/pypa/manylinux_2_28_x86_64 AS wheel_builder

# Install build dependencies
RUN yum update -y && \
    yum install -y \
      gcc gcc-c++ \
      make \
      cmake \
      yaml-cpp-devel && \
    yum clean all

# Build glog from source to get up-to-date version that can be found without FindGlog.cmake
RUN git clone https://github.com/google/glog.git && \
    cd glog && \
    mkdir build && \
    cd build && \
    cmake -DBUILD_SHARED_LIBS=OFF .. && \
    make && \
    make install && \
    cd ../.. && \
    rm -rf glog

# Build util_caching
RUN git clone https://github.com/KIT-MRT/util_caching.git && \
    cd util_caching && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make && \
    make install && \
    cd ../.. && \
    rm -rf util_caching

# Build arbitration_graphs
RUN git clone https://github.com/KIT-MRT/arbitration_graphs.git && \
    cd arbitration_graphs && \
    mkdir build && \
    cd build && \
    cmake -DBUILD_GUI=false .. && \
    make && \
    make install && \
    cd ../.. && \
    rm -rf arbitration_graphs

