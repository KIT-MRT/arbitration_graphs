#!/usr/bin/env bash

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
cd $SCRIPT_DIR/..

docker compose build --pull wheel_builder

# Temporarily replace the symlinked version with a copy as the link can not be resolved inside the docker container
rm version
cp ../version .

# Temporarily replace the relative image paths in the README with absolute URLs to be able to display them on PyPI
sed -i 's|\.\./docs/assets|https://raw.githubusercontent.com/KIT-MRT/arbitration_graphs/main/docs/assets|g' README.md

# Build the wheels
# Currently only supporting x86_64 linux on glibc
CIBW_SKIP="*-musllinux_*" CIBW_MANYLINUX_X86_64_IMAGE=arbitration_graphs_wheel_builder cibuildwheel --platform linux --archs x86_64

# Be a good citizen and undo the changes above
rm version
ln -s ../version .
sed -i 's|https://raw.githubusercontent.com/KIT-MRT/arbitration_graphs/main/docs/assets|\.\./docs/assets|g' README.md
