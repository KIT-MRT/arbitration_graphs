ARG VERSION=latest
FROM ghcr.io/kit-mrt/arbitration_graphs:$VERSION AS tutorial

USER root

# Install EnTT-Pacman dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      build-essential \
      cmake \
      git \
      libsdl2-dev && \
    apt-get clean

# Build and install EnTT-Pacman
RUN git clone https://github.com/KIT-MRT/EnTT-Pacman.git --branch arbitration_graphs_demo /tmp/EnTT-Pacman && \
    cd /tmp/EnTT-Pacman/build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . -j8 && \
    cmake --install . && \
    rm -rf /tmp/EnTT-Pacman

RUN useradd --create-home --uid 1000 blinky
USER blinky
COPY --chown=blinky:blinky .motd /home/blinky/.motd
RUN echo "\n\nsource /home/blinky/.motd" >> /home/blinky/.bashrc

WORKDIR /home/blinky/demo

CMD [ "/bin/bash" ]



FROM tutorial AS unit_test

COPY --chown=blinky:blinky cmake /home/blinky/demo/cmake
COPY --chown=blinky:blinky include /home/blinky/demo/include
COPY --chown=blinky:blinky src /home/blinky/demo/src
COPY --chown=blinky:blinky test /home/blinky/demo/test
COPY --chown=blinky:blinky CMakeLists.txt /home/blinky/demo/CMakeLists.txt

WORKDIR /home/blinky/demo/build

RUN cmake -DBUILD_TESTS=true .. && \
    cmake --build . -j8

# Run unit tests
CMD ["cmake", "--build", ".", "--target", "test"]



FROM tutorial AS demo

COPY --chown=blinky:blinky cmake /home/blinky/demo/cmake
COPY --chown=blinky:blinky include /home/blinky/demo/include
COPY --chown=blinky:blinky src /home/blinky/demo/src
COPY --chown=blinky:blinky CMakeLists.txt /home/blinky/demo/CMakeLists.txt

WORKDIR /home/blinky/demo/build

RUN cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . -j8

CMD ["bash", "-c", "/home/blinky/.motd && /home/blinky/demo/build/arbitration_graphs_pacman_demo_exe"]

