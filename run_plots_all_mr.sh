#!/bin/bash

#Step 4 of the analysis.
#This script runs plot_mr_all for the specified paraticle
#It creates plots taht compare the MR for each target.

# Check if a PID argument is provided
if [ -z "$1" ]; then
  echo "Usage: $0 <pid>"
  exit 1
fi

PID=$1
VARS_ALL=("Zh" "Pt2" "Phi_PQ" "Nu" "Q2")
VARS_BIN=("Zh" "Pt2" "Phi_PQ")

# Loop over each target and launch a new terminal that closes after execution
for VAR in "${VARS_ALL[@]}"; do
  root -l -q 'plot_mr_all.cpp('$PID', "'$VAR'")'
done

for VAR in "${VARS_BIN[@]}"; do
  root -l -q 'plot_mr_bin.cpp('$PID', "'$VAR'")'
done

echo run_plots_all_mr finished.