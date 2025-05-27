#!/usr/bin/env bash
set -Euo pipefail

# explicitly allow continuing the script on exit codes != 0
set +e

# run the given command, send a SIGINT after 1s
# if it doesn't shut down, kill it 1s later
timeout --signal=SIGINT --kill-after=1 1 $1

# Return 0, if exit code was 124 (timeout on SIGINT succeeded), otherwise use the timeout exit code
exit "${?/124/0}"