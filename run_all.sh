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
echo 'simple_plots.cxx('$RUN, "$TARGET"' , "data")'

#make multibbining
echo 'make_multibinning_v3.cpp('$RUN, "$TARGET"' , "data", 211)'


#Second part to run interactively (or in personal computer)

# #integrate multibinning
# ./run_integrate_multibinning.sh -tar C -pid 211
# ./run_integrate_multibinning.sh -tar Al -pid 211
# ./run_integrate_multibinning.sh -tar Cu -pid 211
# ./run_integrate_multibinning.sh -tar Sn -pid 211
# ./run_integrate_multibinning.sh -tar Pb -pid 211
# 
# #plots
# ./run_plots_all_mr.sh 211