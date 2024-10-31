FROM ubuntu:22.04

ARG DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && \
    apt-get install -y \
      build-essential \
      cmake \
      git \
      libgoogle-glog-dev \
      libgtest-dev \
      libyaml-cpp-dev && \
    apt-get clean

# Install util_caching
RUN git clone https://github.com/KIT-MRT/util_caching.git /tmp/util_caching && \
    mkdir /tmp/util_caching/build && \
    cd /tmp/util_caching/build && \
    cmake .. && \
    cmake --build . && \
    cmake --install . && \
    rm -rf /tmp/util_caching


# Install arbitration_graphs
COPY CMakeLists.txt /tmp/arbitration_graphs/
COPY cmake /tmp/arbitration_graphs/cmake
COPY include /tmp/arbitration_graphs/include
COPY test /tmp/arbitration_graphs/test

RUN mkdir /tmp/arbitration_graphs/build && \
    cd /tmp/arbitration_graphs/build && \
    cmake .. && \
    cmake --build . && \
    cmake --install . && \
    rm -rf /tmp/arbitration_graphs

RUN useradd --create-home --uid 1000 blinky
USER blinky

WORKDIR /home/blinky/

