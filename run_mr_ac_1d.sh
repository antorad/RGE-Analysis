#!/bin/bash

#Step 1.3 of the analysis.
#This script runs run_mr_ac_1d for each target.
#It calculates AC 1D MR and save them ina single root file per target.

# Array particle pid to calculate multiplicity ratio
particles=(211 -211 2212)

# Calculate mr for individual targets
for particle in "${particles[@]}"
do
    echo "Processing particle pid: $particle"

    # Run the mr claculation macro
    #For now C only. When simulations are done, I'll add the rest of targets
    root -l -q 'mr_ac_1d.cpp("C" '", $particle"')'

done