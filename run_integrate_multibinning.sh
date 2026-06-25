#!/bin/bash

#Step 3 of the analysis.
#This script runs integrate_multibinning fir a single target.
#It calculate 5D MR, with and without acceptance correction, and integrate all bins into a final 1D plot.

VARS=("Zh" "Pt2" "Phi_PQ" "Nu" "Q2")

# Parse command-line arguments
while [[ "$#" -gt 0 ]]; do
    case "$1" in
        -tar)
            TARGET="$2"
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

# Ensure both arguments are provided
if [ -z "$TARGET" ] || [ -z "$PID" ]; then
    echo "Usage: $0 -tar <target> -pid <pid>"
    exit 1
fi

# Iterate over each set of varaibles and open a new terminal
for VAR in "${VARS[@]}"; do
    #gnome-terminal --wait -- bash -c "root -l -q 'integrate_multibinning_v2.cpp(\"$TARGET\", $PID, \"$VAR\")'" &
    root -l -q "integrate_multibinning_v2.cpp(\"$TARGET\", $PID, \"$VAR\")"
done
wait

echo run_integrate_multibinning for $TARGET finished.