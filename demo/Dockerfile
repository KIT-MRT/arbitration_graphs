FROM ghcr.io/kit-mrt/arbitration_graphs:latest

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

WORKDIR /home/blinky

CMD [ "/bin/bash" ]
