#!/bin/bash

#Step 4 of the analysis.
#This script runs plot_mr_all for the specified paraticle
#It creates plots taht compare the MR for each target.

# Parse command-line arguments
while getopts "p:" opt; do
  case $opt in
    p) PID=$OPTARG;;
    \?) echo "Invalid option: -$OPTARG" >&2;;
  esac
done

VARS_ALL=("Zh" "Pt2" "Phi_PQ" "Nu" "Q2")
VARS_BIN=("Zh" "Pt2" "Phi_PQ")

# Loop over each target and launch a new terminal that closes after execution
for VAR in "${VARS_ALL[@]}"; do
  #gnome-terminal --wait -- bash -c "root -l -q 'plot_mr_all.cpp($PID, \"$VAR\")'" &
  root -l -q -b "plot_mr_all.cpp($PID, \"$VAR\")"
done
wait

for VAR in "${VARS_BIN[@]}"; do
  #gnome-terminal --wait -- bash -c "root -l -q 'plot_mr_bin.cpp($PID, \"$VAR\")'" &
  root -l -q -b "plot_mr_bin.cpp($PID, \"$VAR\")"
done
wait

echo run_plots_all_mr finished.