#!/bin/bash

set -e

if [[ $# -ne 2 ]]; then
  echo "Usage: $0 <version> <input_string>"
  exit 1
fi

initial_version=${1//[^0-9.v]/}
input_string=${2//[^a-zA-Z0-9#]/}

initial_major=$(echo "$initial_version" | cut -d'.' -f1 | cut -d'v' -f2)
initial_minor=$(echo "$initial_version" | cut -d'.' -f2)
initial_patch=$(echo "$initial_version" | cut -d'.' -f3)

# Determine the bump type based on PR description
if [[ "$input_string" == *"#major"* ]]; then
  new_major=$((initial_major + 1))
  new_minor=0
  new_patch=0
elif [[ "$input_string" == *"#minor"* ]]; then
  new_major=$initial_major
  new_minor=$((initial_minor + 1))
  new_patch=0
elif [[ "$input_string" == *"#patch"* ]]; then
  new_major=$initial_major
  new_minor=$initial_minor
  new_patch=$((initial_patch + 1))
else
  # Default to minor bump
  new_major=$initial_major
  new_minor=$((initial_minor + 1))
  new_patch=0
fi

new_version="v${new_major}.${new_minor}.${new_patch}"
echo "${new_version}"

