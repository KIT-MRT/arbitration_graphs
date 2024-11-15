ARG VERSION=latest

FROM ghcr.io/kit-mrt/arbitration_graphs_pacman_tutorial:$VERSION

USER root

# Install clangd for the VSCode extension to work out of the box
# Install zsh and tig as modern dev tools
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      clangd \
      curl \
      tig \
      zsh && \
    apt-get clean

# Install and use starship terminal prompt
RUN curl https://starship.rs/install.sh > /tmp/starship_install.sh && \
    chmod +x /tmp/starship_install.sh && \
    /tmp/starship_install.sh -y && \
    rm /tmp/starship_install.sh

COPY .devcontainer/.zshrc /home/blinky/.zshrc

USER blinky
