#!/bin/bash

#Step 1.3 of the analysis.
#This script runs run_mr_ac_1d for each target.
#It calculates AC 1D MR and save them ina single root file per target.

# Parse command-line arguments
while getopts "m:p:" opt; do
  case $opt in
    m) TORUS_POL=$OPTARG;;
    p) PID=$OPTARG;;
    \?) echo "Invalid option: -$OPTARG" >&2;;
  esac
done

echo "Processing particle pid: $PID"

# Run the mr claculation macro

gnome-terminal --wait -- bash -c "root -l -q 'mr_ac_1d.cpp(\"C\" , $PID)'" &
gnome-terminal --wait -- bash -c "root -l -q 'mr_ac_1d.cpp(\"Al\", $PID)'" &
gnome-terminal --wait -- bash -c "root -l -q 'mr_ac_1d.cpp(\"Cu\", $PID)'" &
gnome-terminal --wait -- bash -c "root -l -q 'mr_ac_1d.cpp(\"Sn\", $PID)'" &
gnome-terminal --wait -- bash -c "root -l -q 'mr_ac_1d.cpp(\"Pb\", $PID)'" &
wait

echo "run_mr_ac_1d finished"

# Comparison between targets
echo "Plotiting MR"
for var in "${vars[@]}"
do
    # Run the mr plotting macro
    root -l -q "plot_simple_mr_all.cpp($PID, \"$var\")"
done
echo plot_simple_mr_all finished.

echo run_simple_mr finished.