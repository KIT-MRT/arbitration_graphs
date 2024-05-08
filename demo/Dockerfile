FROM ghcr.io/kit-mrt/arbitration_graphs:latest

USER root

# Install EnTT-Pacman dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      build-essential \
      cmake \
      libsdl2-dev

# Build and install EnTT-Pacman
COPY EnTT-Pacman/animations /opt/pacman/animations
COPY EnTT-Pacman/cmake /opt/pacman/cmake
COPY EnTT-Pacman/include /opt/pacman/include
COPY EnTT-Pacman/src /opt/pacman/src
COPY EnTT-Pacman/third_party /opt/pacman/third_party
COPY EnTT-Pacman/CMakeLists.txt /opt/pacman/CMakeLists.txt

RUN mkdir -p /opt/pacman/build && \
    chdir /opt/pacman/build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . && \
    cmake --install . && \
    cp pacman_exe /usr/local/bin/pacman && \
    rm -rf /opt/pacman


# Build demo

RUN mkdir -p /opt/demo && chown 1000:1000 /opt/demo

COPY src /opt/demo/src
COPY CMakeLists.txt /opt/demo/CMakeLists.txt


USER blinky

WORKDIR /opt/demo/build
RUN cmake -DCMAKE_BUILD_TYPE=Release ..
RUN cmake --build .

CMD [ "./arbitration_graphs_pacman_demo" ]
