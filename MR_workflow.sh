#!/bin/bash

TARGET=$1

WORKFLOW_BASE="rge_data_MR"

WORKFLOW_NAME="${WORKFLOW_BASE}_${BANKS}_${TARGET}"

# Create workflow
swif2 create $WORKFLOW_NAME

# Loop over run numbers from file and add jobs to workflow
while read -r RUN_NUMBER; do
    swif2 add-job $WORKFLOW_NAME \
        -name run_$RUN_NUMBER \
        -partition production \
        -time 4h \
        -ram 1g \
        -disk 1g \
        -shell /bin/bash \
        "cd /work/clas12/rg-e/antorad/RGE-Analysis/ && ./run_all_wf.sh -r $RUN_NUMBER -t $TARGET" 
done < runs/${TARGET}_runs.txt

# Run workflow
swif2 run $WORKFLOW_NAME