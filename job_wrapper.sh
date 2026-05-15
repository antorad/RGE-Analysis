#!/bin/bash

#I'll keep the dc vs fmt for now for future changes

echo "--- Running: job_wrapper ---"

module use /scigroup/cvmfs/hallb/clas12/sw/modulefiles
module load clas12
export ROOT=/u/scigroup/cvmfs/hallb/clas12/sw/almalinux9-gcc11/local/root/6.36.04/
cd /work/clas12/rg-e/antorad/RGE-Analysis/
echo "About to run run_all_wf.sh.sh with RUN_NUMBER=$2, $1 banks and $3 solid target"

if [ $1 == "dc" ]; then
    ./run_all_wf.sh -run $2 -tar $3
elif [ $1 == "fmt" ]; then
    ./run_all_wf.sh -run $2 -tar $3
else
    echo "Wrong bank name"
fi