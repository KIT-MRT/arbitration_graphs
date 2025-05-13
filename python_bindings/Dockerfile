# ==========
# Base image
# ==========

ARG VERSION=latest
FROM ghcr.io/kit-mrt/arbitration_graphs:$VERSION AS base

RUN apt-get update && \
    apt-get install -y \
      ninja-build \
      pybind11-dev \
      python3-dev \
      python3-pip \
      python3-pybind11 \
      python3-yaml && \
    apt-get clean


# ===========
# Development
# ===========

FROM base AS devel

RUN useradd --create-home --uid 1000 blinky
USER blinky

WORKDIR /home/blinky/
