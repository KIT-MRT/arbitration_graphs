ARG VERSION
FROM ghcr.io/kit-mrt/arbitration_graphs:$VERSION AS tutorial

USER root

# Install EnTT-Pacman dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      build-essential \
      cmake \
      libsdl2-dev

# Build and install EnTT-Pacman
RUN git clone https://github.com/KIT-MRT/EnTT-Pacman.git --branch arbitration_graphs_demo /tmp/EnTT-Pacman && \
    cd /tmp/EnTT-Pacman/build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . && \
    cmake --install . && \
    rm -rf /tmp/EnTT-Pacman

USER blinky

WORKDIR /home/blinky/demo

CMD [ "/bin/bash" ]


FROM tutorial AS demo

COPY --chown=blinky:blinky cmake /home/blinky/demo/cmake
COPY --chown=blinky:blinky include /home/blinky/demo/include
COPY --chown=blinky:blinky src /home/blinky/demo/src
COPY --chown=blinky:blinky CMakeLists.txt /home/blinky/demo/CMakeLists.txt

WORKDIR /home/blinky/demo/build

RUN cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . -j8

CMD [ "/home/blinky/demo/build/arbitration_graphs_pacman_demo_exe" ]

