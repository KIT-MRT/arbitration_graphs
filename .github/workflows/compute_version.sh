#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VERSION_FILE="$SCRIPT_DIR/../../version"

# Read the current version from the version file or default to v0.0.0
if [[ -f "$VERSION_FILE" ]]; then
  source "$VERSION_FILE"
else
  VERSION="v0.0.0"
fi

# Extract the current version components
major=$(echo $VERSION | cut -d'.' -f1 | cut -d'v' -f2)
minor=$(echo $VERSION | cut -d'.' -f2)
patch=$(echo $VERSION | cut -d'.' -f3)

# Read the PR description passed as an argument
input_string="$1"

# Determine the bump type based on PR description
if [[ "$input_string" == *"#major"* ]]; then
  new_major=$((major + 1))
  new_minor=0
  new_patch=0
elif [[ "$input_string" == *"#minor"* ]]; then
  new_major=$major
  new_minor=$((minor + 1))
  new_patch=0
elif [[ "$input_string" == *"#patch"* ]]; then
  new_major=$major
  new_minor=$minor
  new_patch=$((patch + 1))
else
  # Default to minor bump
  new_major=$major
  new_minor=$((minor + 1))
  new_patch=0
fi

# Construct the new version
new_version="v${new_major}.${new_minor}.${new_patch}"

# Output the new version
echo "${new_version}"

