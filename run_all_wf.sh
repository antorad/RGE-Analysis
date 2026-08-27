#!/bin/bash

echo "--- Running: run_all_wf ---"

# Environment setup
module use /scigroup/cvmfs/hallb/clas12/sw/modulefiles
module load clas12/5.7
export ROOT=/u/scigroup/cvmfs/hallb/clas12/sw/almalinux9-gcc11/local/root/6.36.04/

# Parse command-line arguments
while getopts "r:t:" opt; do
  case $opt in
    r) RUN=$OPTARG;;
    t) TARGET=$OPTARG;;
    \?) echo "Invalid option: -$OPTARG" >&2;;
  esac
done

#First part to run in cluster (in job)
#simple plots
root -l -q -b 'simple_plots.cxx('$RUN', "'$TARGET'" , "data")'

#make multibining
root -l -q -b 'make_multibinning_v3.cpp('$RUN', "'$TARGET'" , 211, "data")'

#make multibining
root -l -q -b 'make_multibinning_v3.cpp('$RUN', "'$TARGET'" , -211, "data")'