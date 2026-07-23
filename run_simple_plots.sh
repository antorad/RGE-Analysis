#!/bin/bash

# Check polarity
if [[ -z "$1" ]]; then
  echo "Usage: $0 <polarity: inb or out>"
  exit 1
fi
TORUS_POL=$1

#Step 1 of the analysis.
#This script runs simple_plots for each target.
#It creates simple plots for varaible checks and creates electron and hadron tuples. 

if [[ $TORUS_POL == "inb" ]]; then
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

  hadd -T -f output/$TORUS_POL/data/C/out_clas12.root  output/$TORUS_POL/data/C/*/out_clas12.root
  hadd -T -f output/$TORUS_POL/data/Al/out_clas12.root output/$TORUS_POL/data/Al/*/out_clas12.root
  hadd -T -f output/$TORUS_POL/data/Cu/out_clas12.root output/$TORUS_POL/data/Cu/*/out_clas12.root
  hadd -T -f output/$TORUS_POL/data/Sn/out_clas12.root output/$TORUS_POL/data/Sn/*/out_clas12.root
  hadd -T -f output/$TORUS_POL/data/Pb/out_clas12.root output/$TORUS_POL/data/Pb/*/out_clas12.root

elif [[ $TORUS_POL == "out" ]]; then
  gnome-terminal --wait -- bash -c "root -l -q 'simple_plots.cxx(20517, \"C\", \"data\")'" &
  gnome-terminal --wait -- bash -c "root -l -q 'simple_plots.cxx(20519, \"C\", \"data\")'" &
  wait
  gnome-terminal --wait -- bash -c "root -l -q 'simple_plots.cxx(20521, \"Pb\", \"data\")'" &
  gnome-terminal --wait -- bash -c "root -l -q 'simple_plots.cxx(20523, \"Pb\", \"data\")'" &
  wait

  hadd -T -f output/$TORUS_POL/data/C/out_clas12.root  output/$TORUS_POL/data/C/*/out_clas12.root
  hadd -T -f output/$TORUS_POL/data/Pb/out_clas12.root output/$TORUS_POL/data/Pb/*/out_clas12.root
fi

echo "run_simple plots finished."