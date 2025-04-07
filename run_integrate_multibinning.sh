#!/bin/bash

# Define the ROOT macro
ROOT_MACRO="integrate_multibinning_v2.cpp"

# Parse command-line arguments
while [[ "$#" -gt 0 ]]; do
    case "$1" in
        -tar)
            FIRST_ARG="$2"
            shift 2
            ;;
        -pid)
            SECOND_ARG="$2"
            shift 2
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Ensure both arguments are provided
if [ -z "$FIRST_ARG" ] || [ -z "$SECOND_ARG" ]; then
    echo "Usage: $0 -tar <target> -pid <pid>"
    exit 1
fi

# Define the argument list dynamically
PARAMS=("Zh" "Pt2" "Phi_PQ" "Nu" "Q2")
ARGS_LIST=()
for PARAM in "${PARAMS[@]}"; do
    ARGS_LIST+=("\"$FIRST_ARG\", $SECOND_ARG, \"$PARAM\"")
done

# Iterate over each set of arguments and open a new terminal
for ARGS in "${ARGS_LIST[@]}"; do
        gnome-terminal -- bash -c "root -l -q '$ROOT_MACRO($ARGS)'"
done