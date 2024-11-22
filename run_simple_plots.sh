#!/bin/bash

# Array of files to read wich contain run numbers for each target
files=("C_runs.txt" "Al_runs.txt" "Cu_runs.txt" "Sn_runs.txt" "Pb_runs.txt")

# Run simple_plots for each individual run
for input_file in "${files[@]}"
do
    # Check if the file exists
    if [[ ! -f $input_file ]]; then
        echo "File not found: $input_file"
        continue
    fi

    echo "Processing file: $input_file"

    # Read the file line by line
    while IFS= read -r line
    do
        # Process the line
        root -l -q "simple_plots.cxx($line)"
    done < "$input_file"
done

# Run simple_plots for each target
root -l -q 'simple_plots.cxx("C_runs.txt", "C")'
root -l -q 'simple_plots.cxx("Al_runs.txt", "Al")'
root -l -q 'simple_plots.cxx("Cu_runs.txt", "Cu")'
root -l -q 'simple_plots.cxx("Sn_runs.txt", "Sn")'
root -l -q 'simple_plots.cxx("Pb_runs.txt", "Pb")'