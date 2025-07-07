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
VARS=("Zh" "Pt2" "Phi_PQ" "Nu" "Q2")

# Loop over each target and launch a new terminal that closes after execution
for VARS in "${VARS[@]}"; do
  gnome-terminal -- bash -c "root -l -q 'plot_mr_all.cpp($PID, \"$VARS\")'"
done