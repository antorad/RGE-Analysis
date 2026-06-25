#!/bin/bash

#Step 1.2 of the analysis.
#This script runs simple_mr for each target.
#It calculates non corrected 1D MR and then plot all them for comparison.

# Check if a PID argument is provided
if [ -z "$1" ]; then
  echo "Usage: $0 <pid>"
  exit 1
fi
particle=$1

# Array of variable to compare between targets
vars=(z_h nu p_T2 Q2)

echo "Processing particle pid: $particle"

# Run the mr claculation macro
#gnome-terminal --wait -- bash -c "root -l -q 'simple_mr.cpp(\"C\" , $particle , \"data\")'"  &
#gnome-terminal --wait -- bash -c "root -l -q 'simple_mr.cpp(\"Al\" , $particle , \"data\")'" &
#gnome-terminal --wait -- bash -c "root -l -q 'simple_mr.cpp(\"Cu\" , $particle , \"data\")'" &
#gnome-terminal --wait -- bash -c "root -l -q 'simple_mr.cpp(\"Sn\" , $particle , \"data\")'" &
#gnome-terminal --wait -- bash -c "root -l -q 'simple_mr.cpp(\"Pb\" , $particle , \"data\")'" &
#wait

root -l -q 'simple_mr.cpp("C" '", $particle"', "data")'
root -l -q 'simple_mr.cpp("Al"'", $particle"', "data")'
root -l -q 'simple_mr.cpp("Cu"'", $particle"', "data")'
root -l -q 'simple_mr.cpp("Sn"'", $particle"', "data")'
root -l -q 'simple_mr.cpp("Pb"'", $particle"', "data")'

#For now C only. When simulations are done, I'll add the rest of targets
#root -l -q 'simple_mr.cpp("C" '", $particle"', "acc")'
#root -l -q 'simple_mr.cpp("C" '", $particle"', "thrown")'
#root -l -q 'simple_mr.cpp("Al" '", $particle"', "acc")'
#root -l -q 'simple_mr.cpp("Al" '", $particle"', "thrown")'
#root -l -q 'simple_mr.cpp("Cu" '", $particle"', "acc")'
#root -l -q 'simple_mr.cpp("Cu" '", $particle"', "thrown")'
#root -l -q 'simple_mr.cpp("Sn" '", $particle"', "acc")'
#root -l -q 'simple_mr.cpp("Sn" '", $particle"', "thrown")'
#root -l -q 'simple_mr.cpp("Pb" '", $particle"', "acc")'
#root -l -q 'simple_mr.cpp("Pb" '", $particle"', "thrown")'

echo simple_mr finished.

# Comparison between targets
echo "Plotiting MR"
for var in "${vars[@]}"
do
    # Run the mr plotting macro
    root -l -q "plot_simple_mr_all.cpp($particle, \"$var\")"
done
echo plot_simple_mr_all finished.

echo run_simple_mr finished.