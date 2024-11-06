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



FROM base AS devel

RUN useradd --create-home --uid 1000 blinky
USER blinky

WORKDIR /home/blinky/


FROM base AS unit_test

COPY CMakeLists.txt /tmp/arbitration_graphs/
COPY cmake /tmp/arbitration_graphs/cmake
COPY include /tmp/arbitration_graphs/include
COPY test /tmp/arbitration_graphs/test
COPY version /tmp/arbitration_graphs/version

WORKDIR /tmp/arbitration_graphs/build

RUN cmake -DBUILD_TESTS=true .. && \
    cmake --build . -j9

# Run unit tests
CMD ["cmake", "--build", ".", "--target", "test"]



FROM base AS install

# Install arbitration_graphs
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

