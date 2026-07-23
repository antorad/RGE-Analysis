#!/bin/bash

#Step 1.2 of the analysis.
#This script runs simple_mr for each target.
#It calculates non corrected 1D MR and then plot all them for comparison.

# Parse command-line arguments
while [[ "$#" -gt 0 ]]; do
    case "$1" in
        -pol)
            TORUS_POL="$2"
            shift 2
            ;;
        -pid)
            PID="$2"
            shift 2
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Array of variable to compare between targets
vars=(z_h nu p_T2 Q2 phi_PQ)

echo "Processing PID pid: $PID"

#Run macros
root -l -q 'simple_mr.cpp("C" '", $PID"', "data")'
if [[ $TORUS_POL == "inb" ]]; then
  root -l -q 'simple_mr.cpp("Al"'", $PID"', "data")'
  root -l -q 'simple_mr.cpp("Cu"'", $PID"', "data")'
  root -l -q 'simple_mr.cpp("Sn"'", $PID"', "data")'
fi
root -l -q 'simple_mr.cpp("Pb"'", $PID"', "data")'

echo simple_mr finished.

# Comparison between targets
echo "Plotiting MR"
for var in "${vars[@]}"
do
    # Run the mr plotting macro
    root -l -q "plot_simple_mr_all.cpp($PID, \"$var\")"
done
echo plot_simple_mr_all finished.

echo run_simple_mr finished.