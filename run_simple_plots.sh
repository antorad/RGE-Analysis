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

echo "Running data."
if [[ $TORUS_POL == "inb" ]]; then
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20156, \"C\", \"data\")'" &
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20161, \"C\", \"data\")'" &

  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20437, \"Al\", \"data\")'" &
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20482, \"Al\", \"data\")'" &

  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20178, \"Cu\", \"data\")'" &
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20205, \"Cu\", \"data\")'" &

  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20344, \"Sn\", \"data\")'" &
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20383, \"Sn\", \"data\")'" &

  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20265, \"Pb\", \"data\")'" &
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20315, \"Pb\", \"data\")'" &

  wait

  hadd -T -f output_$FWD_REC/$TORUS_POL/data/$DETECTOR/C/out_clas12.root  output_$FWD_REC/$TORUS_POL/data/$DETECTOR/C/020156/out_clas12.root  output_$FWD_REC/$TORUS_POL/data/$DETECTOR/C/020161/out_clas12.root
  hadd -T -f output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Al/out_clas12.root output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Al/020437/out_clas12.root output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Al/020482/out_clas12.root
  hadd -T -f output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Cu/out_clas12.root output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Cu/020178/out_clas12.root output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Cu/020205/out_clas12.root
  hadd -T -f output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Sn/out_clas12.root output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Sn/020344/out_clas12.root output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Sn/020383/out_clas12.root
  hadd -T -f output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Pb/out_clas12.root output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Pb/020265/out_clas12.root output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Pb/020315/out_clas12.root

elif [[ $TORUS_POL == "out" ]]; then
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20517, \"C\", \"data\")'" &
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20519, \"C\", \"data\")'" &

  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20521, \"Pb\", \"data\")'" &
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20523, \"Pb\", \"data\")'" &
  wait

  hadd -T -f output_$FWD_REC/$TORUS_POL/data/$DETECTOR/C/out_clas12.root  output_$FWD_REC/$TORUS_POL/data/$DETECTOR/C/*/out_clas12.root
  hadd -T -f output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Pb/out_clas12.root output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Pb/*/out_clas12.root
fi

echo "Running acc."
if [[ $TORUS_POL == "inb" ]]; then
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(12, \"C\", \"acc\")'" &
  #gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20161, \"C\", \"acc\")'" &

  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(22, \"Al\", \"acc\")'" &
  #gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20482, \"Al\", \"acc\")'" &

  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(32, \"Cu\", \"acc\")'" &
  #gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20205, \"Cu\", \"acc\")'" &

  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(42, \"Sn\", \"acc\")'" &
  #gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20383, \"Sn\", \"acc\")'" &

  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(52, \"Pb\", \"acc\")'" &
  #gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20315, \"Pb\", \"acc\")'" &

  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(11, \"D2\", \"acc\")'" &
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(21, \"D2\", \"acc\")'" &

  wait

  hadd -T -f output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/C/out_clas12.root  output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/C/*/out_clas12.root
  hadd -T -f output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Al/out_clas12.root output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Al/*/out_clas12.root
  hadd -T -f output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Cu/out_clas12.root output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Cu/*/out_clas12.root
  hadd -T -f output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Sn/out_clas12.root output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Sn/*/out_clas12.root
  hadd -T -f output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Pb/out_clas12.root output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Pb/*/out_clas12.root
  hadd -T -f output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/D2/out_clas12.root output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/D2/*/out_clas12.root

elif [[ $TORUS_POL == "out" ]]; then
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(, \"C\", \"acc\")'" &
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(, \"C\", \"acc\")'" &

  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(, \"Pb\", \"acc\")'" &
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(, \"Pb\", \"acc\")'" &
  wait

  hadd -T -f output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/C/out_clas12.root  output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/C/*/out_clas12.root
  hadd -T -f output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Pb/out_clas12.root output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Pb/*/out_clas12.root
  hadd -T -f output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/D2/out_clas12.root output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/D2/*/out_clas12.root
fi

echo "Running thrown."
if [[ $TORUS_POL == "inb" ]]; then
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(12, \"C\", \"thrown\")'" &
  #gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20161, \"C\", \"acc\")'" &

  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(22, \"Al\", \"thrown\")'" &
  #gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20482, \"Al\", \"acc\")'" &

  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(32, \"Cu\", \"thrown\")'" &
  #gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20205, \"Cu\", \"acc\")'" &

  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(42, \"Sn\", \"thrown\")'" &
  #gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20383, \"Sn\", \"acc\")'" &

  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(52, \"Pb\", \"thrown\")'" &
  #gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(20315, \"Pb\", \"acc\")'" &

  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(11, \"D2\", \"thrown\")'" &
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(21, \"D2\", \"thrown\")'" &

  wait

  hadd -T -f output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/C/out_clas12.root  output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/C/*/out_clas12.root
  hadd -T -f output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Al/out_clas12.root output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Al/*/out_clas12.root
  hadd -T -f output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Cu/out_clas12.root output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Cu/*/out_clas12.root
  hadd -T -f output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Sn/out_clas12.root output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Sn/*/out_clas12.root
  hadd -T -f output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Pb/out_clas12.root output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Pb/*/out_clas12.root
  hadd -T -f output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/D2/out_clas12.root output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/D2/*/out_clas12.root

elif [[ $TORUS_POL == "out" ]]; then
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(, \"C\", \"thrown\")'" &
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(, \"C\", \"thrown\")'" &

  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(, \"Pb\", \"thrown\")'" &
  gnome-terminal --wait -- bash -c "root -l -q -b 'simple_plots.cxx(, \"Pb\", \"thrown\")'" &
  wait

  hadd -T -f output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/C/out_clas12.root  output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/C/*/out_clas12.root
  hadd -T -f output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Pb/out_clas12.root output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Pb/*/out_clas12.root
  hadd -T -f output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/D2/out_clas12.root output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/D2/*/out_clas12.root
fi

echo "run_simple plots finished."