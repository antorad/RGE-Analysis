#!/bin/bash

#Step 1 of the analysis.
#This script runs simple_plots for each target.
#It creates simple plots for varaible checks and creates electron and hadron tuples. 

# Run simple_plots for each target in a new terminal for each one.
#gnome-terminal --wait -- bash -c "root -l -q 'simple_plots.cxx(\"runs/C_runs.txt\" , \"C\" , \"data\")'" &
#gnome-terminal --wait -- bash -c "root -l -q 'simple_plots.cxx(\"runs/Al_runs.txt\", \"Al\", \"data\")'" &
#gnome-terminal --wait -- bash -c "root -l -q 'simple_plots.cxx(\"runs/Cu_runs.txt\", \"Cu\", \"data\")'" &
#gnome-terminal --wait -- bash -c "root -l -q 'simple_plots.cxx(\"runs/Sn_runs.txt\", \"Sn\", \"data\")'" &
#gnome-terminal --wait -- bash -c "root -l -q 'simple_plots.cxx(\"runs/Pb_runs.txt\", \"Pb\", \"data\")'" &
#wait

while read -r RUN_NUMBER; do
    gnome-terminal --wait -- bash -c "root -l -q 'simple_plots.cxx($RUN_NUMBER, \"C\", \"data\")'" &
done < runs/C_runs.txt
wait

while read -r RUN_NUMBER; do
    gnome-terminal --wait -- bash -c "root -l -q 'simple_plots.cxx($RUN_NUMBER, \"Al\", \"data\")'" &
done < runs/Al_runs.txt
wait

while read -r RUN_NUMBER; do
    gnome-terminal --wait -- bash -c "root -l -q 'simple_plots.cxx($RUN_NUMBER, \"Cu\", \"data\")'" &
done < runs/Cu_runs.txt
wait

while read -r RUN_NUMBER; do
    gnome-terminal --wait -- bash -c "root -l -q 'simple_plots.cxx($RUN_NUMBER, \"Sn\", \"data\")'" &
done < runs/Sn_runs.txt
wait

while read -r RUN_NUMBER; do
    gnome-terminal --wait -- bash -c "root -l -q 'simple_plots.cxx($RUN_NUMBER, \"Pb\", \"data\")'" &
done < runs/Pb_runs.txt
wait

hadd -T -f output/data/C/out_clas12.root output/data/C/*/out_clas12.root
hadd -T -f output/data/Al/out_clas12.root output/data/Al/*/out_clas12.root
hadd -T -f output/data/Cu/out_clas12.root output/data/Cu/*/out_clas12.root
hadd -T -f output/data/Sn/out_clas12.root output/data/Sn/*/out_clas12.root
hadd -T -f output/data/Pb/out_clas12.root output/data/Pb/*/out_clas12.root

#root -l -q 'simple_plots.cxx("runs/D2_simul_runs.txt" , "D2" , "simul")'
#root -l -q 'simple_plots.cxx("runs/C_simul_runs.txt" , "C" , "simul")'
#root -l -q 'simple_plots.cxx("runs/Al_simul_runs.txt" , "Al" , "simul")'
#root -l -q 'simple_plots.cxx("runs/Cu_simul_runs.txt" , "Cu" , "simul")'
#root -l -q 'simple_plots.cxx("runs/Sn_simul_runs.txt" , "Sn" , "simul")'
#root -l -q 'simple_plots.cxx("runs/Pb_simul_runs.txt" , "Pb" , "simul")'

echo "run_simple plots finished."