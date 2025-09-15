#!/bin/bash

#Step 1.2 of the analysis.
#This script runs simple_mr for each target.
#It calculates non corrected 1D MR and then plot all them for comparison.

# Array particle pid to calculate multiplicity ratio
particles=(211 -211 2212)
# Array of variable to compare between targets
vars=(z_h nu p_T2)

# Calculate mr for individual targets
for particle in "${particles[@]}"
do
    echo "Processing particle pid: $particle"

    # Run the mr claculation macro
    root -l -q 'simple_mr.cpp("C" '", $particle"', "data")'
    root -l -q 'simple_mr.cpp("Al"'", $particle"', "data")'
    root -l -q 'simple_mr.cpp("Cu"'", $particle"', "data")'
    root -l -q 'simple_mr.cpp("Sn"'", $particle"', "data")'
    root -l -q 'simple_mr.cpp("Pb"'", $particle"', "data")'

    #For now C only. When simulations are done, I'll add the rest of targets
    root -l -q 'simple_mr.cpp("C" '", $particle"', "acc")'
    root -l -q 'simple_mr.cpp("C" '", $particle"', "thrown")'
    root -l -q 'simple_mr.cpp("Al" '", $particle"', "acc")'
    root -l -q 'simple_mr.cpp("Al" '", $particle"', "thrown")'
    root -l -q 'simple_mr.cpp("Cu" '", $particle"', "acc")'
    root -l -q 'simple_mr.cpp("Cu" '", $particle"', "thrown")'
    root -l -q 'simple_mr.cpp("Sn" '", $particle"', "acc")'
    root -l -q 'simple_mr.cpp("Sn" '", $particle"', "thrown")'
    root -l -q 'simple_mr.cpp("Pb" '", $particle"', "acc")'
    root -l -q 'simple_mr.cpp("Pb" '", $particle"', "thrown")'
done

# Comparison between targets
echo "Plotiting MR"
for particle in "${particles[@]}"
do
    for var in "${vars[@]}"
    do
        # Run the mr plotting macro
        root -l -q "plot_simple_mr_all.cpp($particle, \"$var\")"
    done
done