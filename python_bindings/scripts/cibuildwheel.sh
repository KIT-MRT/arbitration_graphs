#!/bin/env bash

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
cd $SCRIPT_DIR/..

docker compose build --pull wheel_builder

# Temporary replace the symlinked version with a copy as the link can not be resolved inside the docker container
rm version
cp ../version .

# Build the wheels
# Currently only supporting x86_64 linux on glibc
CIBW_SKIP="*-musllinux_*" CIBW_MANYLINUX_X86_64_IMAGE=arbitration_graphs_wheel_builder cibuildwheel --platform linux --archs x86_64

# Fix the symlink again
rm version
ln -s ../version .
