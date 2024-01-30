FROM ubuntu:22.04

ARG DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && \
    apt-get install -y \
      build-essential \
      cmake \
      libgoogle-glog-dev \
      libgtest-dev \
      libyaml-cpp-dev && \
    apt-get clean

# Install util_caching
COPY thirdparty/util_caching /tmp/util_caching
RUN mkdir /tmp/util_caching_build && \
    cd /tmp/util_caching_build && \
    cmake /tmp/util_caching && \
    cmake --build . && \
    cmake --install . && \
    rm -rf /tmp/util_caching_build /tmp/util_caching


# Install arbitration_graphs
COPY . /tmp/arbitration_graphs
RUN mkdir /tmp/arbitration_graphs_build && \
    cd /tmp/arbitration_graphs_build && \
    cmake /tmp/arbitration_graphs && \
    cmake --build . && \
    cmake --install . && \
    rm -rf /tmp/arbitration_graphs_build /tmp/arbitration_graphs

RUN useradd --create-home --uid 1000 blinky
USER blinky

WORKDIR /home/blinky/

