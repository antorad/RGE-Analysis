#!/bin/bash

#Step 3 of the analysis.
#This script runs integrate_multibinning fir a single target.
#It calculate 5D MR, with and without acceptance correction, and integrate all bins into a final 1D plot.

VARS=("Zh" "Pt2" "Phi_PQ" "Nu" "Q2")

# Parse command-line arguments
while getopts "t:p:" opt; do
  case $opt in
    t) TARGET=$OPTARG;;
    p) PID=$OPTARG;;
    \?) echo "Invalid option: -$OPTARG" >&2;;
  esac
done

# Iterate over each set of varaibles and open a new terminal
for VAR in "${VARS[@]}"; do
    #gnome-terminal --wait -- bash -c "root -l -q 'integrate_multibinning_v2.cpp(\"$TARGET\", $PID, \"$VAR\")'" &
    root -l -q -b "integrate_multibinning_v2.cpp(\"$TARGET\", $PID, \"$VAR\")"
done
wait

echo run_integrate_multibinning for $TARGET finished.