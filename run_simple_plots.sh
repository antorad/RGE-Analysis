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

gnome-terminal --wait -- bash -c "root -l -q 'simple_plots.cxx(20156, \"C\", \"data\")'" &
gnome-terminal --wait -- bash -c "root -l -q 'simple_plots.cxx(20161, \"C\", \"data\")'" &
wait
gnome-terminal --wait -- bash -c "root -l -q 'simple_plots.cxx(20437, \"Al\", \"data\")'" &
gnome-terminal --wait -- bash -c "root -l -q 'simple_plots.cxx(20482, \"Al\", \"data\")'" &
wait
gnome-terminal --wait -- bash -c "root -l -q 'simple_plots.cxx(20178, \"Cu\", \"data\")'" &
gnome-terminal --wait -- bash -c "root -l -q 'simple_plots.cxx(20205, \"Cu\", \"data\")'" &
wait
gnome-terminal --wait -- bash -c "root -l -q 'simple_plots.cxx(20344, \"Sn\", \"data\")'" &
gnome-terminal --wait -- bash -c "root -l -q 'simple_plots.cxx(20383, \"Sn\", \"data\")'" &
wait
gnome-terminal --wait -- bash -c "root -l -q 'simple_plots.cxx(20265, \"Pb\", \"data\")'" &
gnome-terminal --wait -- bash -c "root -l -q 'simple_plots.cxx(20315, \"Pb\", \"data\")'" &
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