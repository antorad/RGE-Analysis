#!/bin/bash

#Step 1 of the analysis.
#This script runs simple_plots for each target.
#It creates simple plots for varaible checks and creates electron and hadron tuples. 

# # Array of files to read wich contain run numbers for each target
# files=("C_runs.txt" "Al_runs.txt" "Cu_runs.txt" "Sn_runs.txt" "Pb_runs.txt")
# 
# # Run simple_plots for each individual run
# for input_file in "runs/${files[@]}"
# do
#     # Check if the file exists
#     if [[ ! -f $input_file ]]; then
#         echo "File not found: $input_file"
#         continue
#     fi
# 
#     echo "Processing file: $input_file"
# 
#     # Read the file line by line
#     while IFS= read -r line
#     do
#         # Process the line
#         root -l -q "simple_plots.cxx($line)"
#     done < "$input_file"
# done

# Run simple_plots for each target
root -l -q 'simple_plots.cxx("runs/C_runs.txt" , "C" , "data")'
root -l -q 'simple_plots.cxx("runs/Al_runs.txt", "Al", "data")'
root -l -q 'simple_plots.cxx("runs/Cu_runs.txt", "Cu", "data")'
root -l -q 'simple_plots.cxx("runs/Sn_runs.txt", "Sn", "data")'
root -l -q 'simple_plots.cxx("runs/Pb_runs.txt", "Pb", "data")'

root -l -q 'simple_plots.cxx("runs/D2_simul_runs.txt" , "D2" , "simul")'
root -l -q 'simple_plots.cxx("runs/C_simul_runs.txt" , "C" , "simul")'