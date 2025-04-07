#!/bin/bash

# Check if a PID argument is provided
if [ -z "$1" ]; then
  echo "Usage: $0 <pid>"
  exit 1
fi

PID=$1
TARGETS=("C" "Al" "Cu" "Sn" "Pb")

# Loop over each target and launch a new terminal that closes after execution
for TARGET in "${TARGETS[@]}"; do
  gnome-terminal -- bash -c "root -l -q 'make_multibinning_v2.cpp(\"$TARGET\", $PID)'"
done