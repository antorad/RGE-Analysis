#!/bin/bash

# Parse command-line arguments
while getopts "m:b:d:" opt; do
  case $opt in
    m) TORUS_POL=$OPTARG;;
    b) FWD_REC=$OPTARG;;
    d) DETECTOR=$OPTARG;;
    \?) echo "Invalid option: -$OPTARG" >&2;;
  esac
done

#Step 1 of the analysis.
#This script runs simple_plots for each target.
#It creates simple plots for varaible checks and creates electron and hadron tuples. 

if [[ $TORUS_POL == "inb" ]]; then
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20156, \"C\", \"data\")'" &
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20161, \"C\", \"data\")'" &

  wait
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20437, \"Al\", \"data\")'" &
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20482, \"Al\", \"data\")'" &

  wait
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20178, \"Cu\", \"data\")'" &
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20205, \"Cu\", \"data\")'" &

  wait
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20344, \"Sn\", \"data\")'" &
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20383, \"Sn\", \"data\")'" &

  wait
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20265, \"Pb\", \"data\")'" &
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20315, \"Pb\", \"data\")'" &

  wait

  hadd -T -f output_$FWD_REC/$TORUS_POL/data/$DETECTOR/C/out_clas12.root  output_$FWD_REC/$TORUS_POL/data/$DETECTOR/C/*/out_clas12.root
  hadd -T -f output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Al/out_clas12.root output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Al/*/out_clas12.root
  hadd -T -f output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Cu/out_clas12.root output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Cu/*/out_clas12.root
  hadd -T -f output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Sn/out_clas12.root output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Sn/*/out_clas12.root
  hadd -T -f output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Pb/out_clas12.root output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Pb/*/out_clas12.root

elif [[ $TORUS_POL == "out" ]]; then
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20517, \"C\", \"data\")'" &
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20519, \"C\", \"data\")'" &
  wait
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20521, \"Pb\", \"data\")'" &
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20523, \"Pb\", \"data\")'" &
  wait

  hadd -T -f output_$FWD_REC/$TORUS_POL/data/$DETECTOR/C/out_clas12.root  output_$FWD_REC/$TORUS_POL/data/$DETECTOR/C/*/out_clas12.root
  hadd -T -f output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Pb/out_clas12.root output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Pb/*/out_clas12.root
fi

echo "run_simple plots finished."