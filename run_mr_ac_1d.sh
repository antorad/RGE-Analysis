#!/bin/bash

#Step 1.3 of the analysis.
#This script runs run_mr_ac_1d for each target.
#It calculates AC 1D MR and save them ina single root file per target.

# Check if a PID argument is provided
if [ -z "$1" ]; then
  echo "Usage: $0 <pid>"
  exit 1
fi
particle=$1

echo "Processing particle pid: $particle"

# Run the mr claculation macro
#For now C only. When simulations are done, I'll add the rest of targets
root -l -q 'mr_ac_1d.cpp("C" '", $particle"')'

gnome-terminal --wait -- bash -c "root -l -q 'mr_ac_1d.cpp(\"C\" , $particle)'" &
gnome-terminal --wait -- bash -c "root -l -q 'mr_ac_1d.cpp(\"Al\", $particle)'" &
gnome-terminal --wait -- bash -c "root -l -q 'mr_ac_1d.cpp(\"Cu\", $particle)'" &
gnome-terminal --wait -- bash -c "root -l -q 'mr_ac_1d.cpp(\"Sn\", $particle)'" &
gnome-terminal --wait -- bash -c "root -l -q 'mr_ac_1d.cpp(\"Pb\", $particle)'" &
wait

echo "run_mr_ac_1d finished"