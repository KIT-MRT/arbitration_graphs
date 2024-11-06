FROM ubuntu:22.04 AS base

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

# Install Crow dependencies
RUN apt-get update && \
    apt-get install -y \
      libasio-dev \
      unzip \
      wget &&\
    apt-get clean

# Install Crow (C++ REST/WebSocket server)
RUN cd /tmp && \
    wget https://github.com/CrowCpp/Crow/releases/download/v1.2.0/Crow-1.2.0.zip && \
    unzip Crow-*.zip -d /tmp && \
    cp -r /tmp/Crow-*/* /usr/local && \
    rm -r Crow-*

# Install util_caching
RUN git clone https://github.com/KIT-MRT/util_caching.git /tmp/util_caching && \
    mkdir /tmp/util_caching/build && \
    cd /tmp/util_caching/build && \
    cmake .. && \
    cmake --build . && \
    cmake --install . && \
    rm -rf /tmp/util_caching



FROM base AS devel

RUN useradd --create-home --uid 1000 blinky
USER blinky

WORKDIR /home/blinky/



FROM base AS install

# Install arbitration_graphs
COPY CMakeLists.txt /tmp/arbitration_graphs/
COPY cmake /tmp/arbitration_graphs/cmake
COPY gui /tmp/arbitration_graphs/gui
COPY include /tmp/arbitration_graphs/include
COPY test /tmp/arbitration_graphs/test
COPY version /tmp/arbitration_graphs/version

RUN mkdir /tmp/arbitration_graphs/build && \
    cd /tmp/arbitration_graphs/build && \
    cmake .. && \
    cmake --build . && \
    cmake --install . && \
    rm -rf /tmp/arbitration_graphs