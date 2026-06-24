#!/bin/bash

echo "--- Running: run_all_wf ---"

# Environment setup
module use /scigroup/cvmfs/hallb/clas12/sw/modulefiles
module load clas12
export ROOT=/u/scigroup/cvmfs/hallb/clas12/sw/almalinux9-gcc11/local/root/6.36.04/

# Parse command-line arguments
while getopts "b:r:t:" opt; do
  case $opt in
    b) BANKS=$OPTARG;;
    r) RUN=$OPTARG;;
    t) TARGET=$OPTARG;;
    \?) echo "Invalid option: -$OPTARG" >&2;;
  esac
done

# Ensure both arguments are provided
if [ -z "$RUN" ] || [ -z "$TARGET" ]; then
    echo "Usage: $0 -b <banks> -r <run_n> -t <target>"
    exit 1
fi

#First part to run in cluster (in job)
#simple plots
root -l -q 'simple_plots.cxx('$RUN', "'$TARGET'" , "data")'

#make multibining
root -l -q 'make_multibinning_v3.cpp('$RUN', "'$TARGET'" , 211, "data")'

#make multibining
root -l -q 'make_multibinning_v3.cpp('$RUN', "'$TARGET'" , -211, "data")'