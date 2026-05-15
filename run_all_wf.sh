#!/bin/bash

# Parse command-line arguments
while [[ "$#" -gt 0 ]]; do
    case "$1" in
        -run)
            RUN="$2"
            shift 2
            ;;
        -tar)
            TARGET="$2"
            shift 2
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Ensure both arguments are provided
if [ -z "$RUN" ] || [ -z "$TARGET" ]; then
    echo "Usage: $0 -pid <pid> -tar <target>"
    exit 1
fi

#First part to run in cluster (in job)
#simple plots
root -l -q 'simple_plots.cxx('$RUN', "'$TARGET'" , "data")'

#make multibbining
root -l -q 'make_multibinning_v3.cpp('$RUN', "'$TARGET'" , 211, "data")'