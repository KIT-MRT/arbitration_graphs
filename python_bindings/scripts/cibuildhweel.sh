#!/bin/env bash

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)
cd $SCRIPT_DIR/..

docker compose build wheel-builder

# Temporary replace the symlinks with copies of the files as they can not be resolved inside the docker container
rm version .README.md
cp ../version .
cp ../README.md .README.md

# Build the wheels
# Currently only supporting x86_64 linux on glibc
CIBW_SKIP="*-musllinux_*" CIBW_MANYLINUX_X86_64_IMAGE=arbitration-graphs-wheel-builder cibuildwheel --platform linux --archs x86_64

# Fix the symlinks again
rm version .README.md
ln -s ../version .
ln -s ../README.md .README.md
