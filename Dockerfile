# ==========
# Base image
# ==========


FROM ubuntu:22.04 AS base

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

# Install Crow dependencies
RUN apt-get update && \
    apt-get install -y \
      libasio-dev \
      zlib1g-dev &&\
    apt-get clean


# Install Crow (C++ REST/WebSocket server) and util_caching

# `ADD` downloads the Crow and util_caching debian releases and adds them to the docker image
# This "bloats" the image. But otherwise, we'd have to installing wget and ca-certificates
# temporarily to download and install the package in one docker layer…
ADD https://github.com/CrowCpp/Crow/releases/download/v1.2.0/Crow-1.2.0-Linux.deb /tmp/debfiles/
ADD https://github.com/KIT-MRT/util_caching/releases/latest/download/libutil-caching-dev.deb /tmp/debfiles/

# Install Crow and util_caching from release debian package
RUN dpkg -i /tmp/debfiles/*.deb && \
    rm -rf /tmp/debfiles


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
COPY LICENSE /tmp/arbitration_graphs/
COPY README.md /tmp/arbitration_graphs/
COPY cmake /tmp/arbitration_graphs/cmake
COPY gui /tmp/arbitration_graphs/gui
COPY include /tmp/arbitration_graphs/include
COPY test /tmp/arbitration_graphs/test
COPY version /tmp/arbitration_graphs/version

WORKDIR /tmp/arbitration_graphs/build

RUN cmake -DBUILD_TESTS=true .. && \
    cmake --build . -j9

# Run unit tests
CMD ["cmake", "--build", ".", "--target", "test"]


# =======
# Release
# =======

FROM base AS release

COPY CMakeLists.txt /tmp/arbitration_graphs/
COPY LICENSE /tmp/arbitration_graphs/
COPY README.md /tmp/arbitration_graphs/
COPY version /tmp/arbitration_graphs/
COPY cmake /tmp/arbitration_graphs/cmake
COPY gui /tmp/arbitration_graphs/gui
COPY include /tmp/arbitration_graphs/include

WORKDIR /tmp/arbitration_graphs/build

RUN cmake .. && \
    cmake --build . && \
    cmake --build . --target package && \
    mv packages /release && \
    rm -rf /tmp/arbitration_graphs


# =============
# Release tests
# =============

FROM base AS release_test_core

ARG RELEASE_DOWNLOAD_URL=https://github.com/KIT-MRT/arbitration_graphs/releases/latest/download/

# This downloads the latest arbitration_graph debian release and adds it to the docker image
# This "bloats" the image. But otherwise, we'd have to installing wget and ca-certificates
# temporarily to download and install the package in one docker layer…
ADD ${RELEASE_DOWNLOAD_URL}/libarbitration-graphs-core-dev.deb /tmp/debfiles/

# Install arbitration_graphs core library from release debian package
RUN dpkg -i /tmp/debfiles/*.deb && \
    rm -rf /tmp/debfiles

COPY test /tmp/arbitration_graphs_test
WORKDIR /tmp/arbitration_graphs_test/build

RUN cmake .. && \
    cmake --build . -j9

CMD ["cmake", "--build", ".", "--target", "test"]

FROM base AS release_test_gui

ARG RELEASE_DOWNLOAD_URL=https://github.com/KIT-MRT/arbitration_graphs/releases/latest/download/

# This downloads the latest arbitration_graph debian release and adds it to the docker image
# This "bloats" the image. But otherwise, we'd have to installing wget and ca-certificates
# temporarily to download and install the package in one docker layer…
ADD ${RELEASE_DOWNLOAD_URL}/libarbitration-graphs-core-dev.deb /tmp/debfiles/
ADD ${RELEASE_DOWNLOAD_URL}/libarbitration-graphs-gui-dev.deb /tmp/debfiles/

# Install arbitration_graphs gui from release debian package
RUN dpkg -i /tmp/debfiles/*.deb && \
    rm -rf /tmp/debfiles

COPY gui/test /tmp/arbitration_graphs_test
WORKDIR /tmp/arbitration_graphs_test/build

RUN cmake .. && \
    cmake --build . -j9

CMD ["cmake", "--build", ".", "--target", "test"]


# =======
# Install
# =======

FROM base AS install

COPY CMakeLists.txt /tmp/arbitration_graphs/
COPY LICENSE /tmp/arbitration_graphs/
COPY README.md /tmp/arbitration_graphs/
COPY version /tmp/arbitration_graphs/
COPY cmake /tmp/arbitration_graphs/cmake
COPY gui /tmp/arbitration_graphs/gui
COPY include /tmp/arbitration_graphs/include
COPY test /tmp/arbitration_graphs/test

WORKDIR /tmp/arbitration_graphs/build

RUN cmake .. && \
    cmake --build . && \
    cmake --install . && \
    rm -rf /tmp/arbitration_graphs

