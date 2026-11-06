#!/bin/bash

#Step 1.2 of the analysis.
#This script runs simple_mr for each target.
#It calculates non corrected 1D MR and then plot all them for comparison.

# Parse command-line arguments
while getopts "m:p:" opt; do
  case $opt in
    m) TORUS_POL=$OPTARG;;
    p) PID=$OPTARG;;
    \?) echo "Invalid option: -$OPTARG" >&2;;
  esac
done

# Array of variable to compare between targets
vars=(z_h nu p_T2 Q2 phi_PQ)

echo "Processing PID pid: $PID"

#Run macros
#data
root -l -q 'simple_mr.cpp("C" '", $PID"', "data")'
if [[ $TORUS_POL == "inb" ]]; then
  root -l -q 'simple_mr.cpp("Al"'", $PID"', "data")'
  root -l -q 'simple_mr.cpp("Cu"'", $PID"', "data")'
  root -l -q 'simple_mr.cpp("Sn"'", $PID"', "data")'
fi
root -l -q 'simple_mr.cpp("Pb"'", $PID"', "data")'

echo simple_mr finished.

#acc
root -l -q 'simple_mr.cpp("C" '", $PID"', "acc")'
if [[ $TORUS_POL == "inb" ]]; then
  root -l -q 'simple_mr.cpp("Al"'", $PID"', "acc")'
  root -l -q 'simple_mr.cpp("Cu"'", $PID"', "acc")'
  root -l -q 'simple_mr.cpp("Sn"'", $PID"', "acc")'
fi
root -l -q 'simple_mr.cpp("Pb"'", $PID"', "acc")'

echo simple_mr finished.

#thrown
root -l -q 'simple_mr.cpp("C" '", $PID"', "thrown")'
if [[ $TORUS_POL == "inb" ]]; then
  root -l -q 'simple_mr.cpp("Al"'", $PID"', "thrown")'
  root -l -q 'simple_mr.cpp("Cu"'", $PID"', "thrown")'
  root -l -q 'simple_mr.cpp("Sn"'", $PID"', "thrown")'
fi
root -l -q 'simple_mr.cpp("Pb"'", $PID"', "thrown")'

echo simple_mr finished.