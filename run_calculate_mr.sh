#!/bin/bash

# Array particle pid to calculate multiplicity ratio
particles=(211 -211 2212)
# Array of variable to compare between targets
vars=(z_h nu p_T2)

# Calculate mr for individual targets
for particle in "${particles[@]}"
do
    echo "Processing particle pid: $particle"

    # Run the mr claculation macro
    root -l -q 'calculate_mr.cpp("C"'", $particle"')'
    root -l -q 'calculate_mr.cpp("Al"'", $particle"')'
    root -l -q 'calculate_mr.cpp("Cu"'", $particle"')'
    root -l -q 'calculate_mr.cpp("Sn"'", $particle"')'
    root -l -q 'calculate_mr.cpp("Pb"'", $particle"')'
done

# Comparison between targets
echo "Plotiting MR"
for particle in "${particles[@]}"
do
    for var in "${vars[@]}"
    do
        # Run the mr plotting macro
        root -l -q "plot_mr_all.cpp($particle, \"$var\")"
    done
done
