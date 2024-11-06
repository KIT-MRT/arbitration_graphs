#!/bin/bash

# This is a helper script for development, do not use in production!
#
# Usage: ./run_python_webserver.sh [port]
# Starts a minimalistic python webserver on the specified port (default: 8080)

PORT=${1:-8080}

# Change to the htmlroot (the python server makes the cwd accessible)
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd "$SCRIPT_DIR/../htmlroot"

# Start a minimalistic http server
# Note: only use it behind a firewall / in a protected network, see https://docs.python.org/3/library/http.server.html
python3 -m http.server $PORT

