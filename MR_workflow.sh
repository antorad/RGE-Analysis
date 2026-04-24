#!/bin/bash

WORKFLOW_BASE="rge_data_MR"
BANKS="dc"
TARGET="C"

WORKFLOW_NAME="${WORKFLOW_BASE}_${BANKS}_${TARGET}"

# Create workflow
swif2 create $WORKFLOW_NAME

# Loop over run numbers from file and add jobs to workflow
while read -r RUN_NUMBER; do
    swif2 add-job $WORKFLOW_NAME \
        -name run_$RUN_NUMBER \
        -partition production \
        -time 2h \
        -ram 1g \
        -disk 1g \
        -shell /bin/bash \
        "cd /work/clas12/rg-e/antorad/RGE-Analysis/ && ./job_wrapper.sh $BANKS $RUN_NUMBER $TARGET" 
done < run_list.txt

# Run workflow
swif2 run $WORKFLOW_NAME