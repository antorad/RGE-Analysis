#!/bin/bash

#Step 2 of the analysis.
#This script runs make_multibinning for each target.
#It divides the events in TH2 histos in a 5D binning.

# Parse command-line arguments
while getopts "p:m:b:d:" opt; do
  case $opt in
    m) TORUS_POL=$OPTARG;;
    p) PID=$OPTARG;;
    b) FWD_REC=$OPTARG;;
    d) DETECTOR=$OPTARG;;
    \?) echo "Invalid option: -$OPTARG" >&2;;
  esac
done

#Data
if [[ $TORUS_POL == "inb" ]]; then
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(20156,\"C\",  $PID, \"data\")'" &
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(20161,\"C\",  $PID, \"data\")'" &

    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(20437,\"Al\", $PID, \"data\")'" &
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(20482,\"Al\", $PID, \"data\")'" &

    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(20178,\"Cu\", $PID, \"data\")'" &
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(20205,\"Cu\", $PID, \"data\")'" &

    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(20344,\"Sn\", $PID, \"data\")'" &
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(20383,\"Sn\", $PID, \"data\")'" &

    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(20265,\"Pb\", $PID, \"data\")'" &
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(20315,\"Pb\", $PID, \"data\")'" &
    wait

    hadd -T -f output_$FWD_REC/$TORUS_POL/data/$DETECTOR/C/data_binned_pion.root  output_$FWD_REC/$TORUS_POL/data/$DETECTOR/C/*/data_binned_pion.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Al/data_binned_pion.root output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Al/*/data_binned_pion.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Cu/data_binned_pion.root output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Cu/*/data_binned_pion.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Sn/data_binned_pion.root output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Sn/*/data_binned_pion.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Pb/data_binned_pion.root output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Pb/*/data_binned_pion.root

    hadd -T -f output_$FWD_REC/$TORUS_POL/data/$DETECTOR/C/data_binned_pion_minus.root  output_$FWD_REC/$TORUS_POL/data/$DETECTOR/C/*/data_binned_pion_minus.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Al/data_binned_pion_minus.root output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Al/*/data_binned_pion_minus.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Cu/data_binned_pion_minus.root output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Cu/*/data_binned_pion_minus.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Sn/data_binned_pion_minus.root output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Sn/*/data_binned_pion_minus.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Pb/data_binned_pion_minus.root output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Pb/*/data_binned_pion_minus.root

elif [[ $TORUS_POL == "out" ]]; then
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(20517,\"C\",  $PID, \"data\")'" &
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(20519,\"C\",  $PID, \"data\")'" &
    wait
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(20521,\"Pb\", $PID, \"data\")'" &
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(20523,\"Pb\", $PID, \"data\")'" &
    wait

    hadd -T -f output_$FWD_REC/$TORUS_POL/data/$DETECTOR/C/data_binned_pion.root  output_$FWD_REC/$TORUS_POL/data/$DETECTOR/C/*/data_binned_pion.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Pb/data_binned_pion.root output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Pb/*/data_binned_pion.root

    hadd -T -f output_$FWD_REC/$TORUS_POL/data/$DETECTOR/C/data_binned_pion_minus.root  output_$FWD_REC/$TORUS_POL/data/$DETECTOR/C/*/data_binned_pion_minus.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Pb/data_binned_pion_minus.root output_$FWD_REC/$TORUS_POL/data/$DETECTOR/Pb/*/data_binned_pion_minus.root
fi

#Acc
if [[ $TORUS_POL == "inb" ]]; then
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(12,\"C\",  $PID, \"acc\")'" &
    #gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(,\"C\",  $PID, \"acc\")'" &
    
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(22,\"Al\", $PID, \"acc\")'" &
    #gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(,\"Al\", $PID, \"acc\")'" &
    
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(32,\"Cu\", $PID, \"acc\")'" &
    #gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(,\"Cu\", $PID, \"acc\")'" &
    
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(42,\"Sn\", $PID, \"acc\")'" &
    #gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(,\"Sn\", $PID, \"acc\")'" &
    
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(52,\"Pb\", $PID, \"acc\")'" &
    #gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(,\"Pb\", $PID, \"acc\")'" &
    
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(11,\"D2\", $PID, \"acc\")'" &
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(21,\"D2\", $PID, \"acc\")'" &
    wait

    hadd -T -f output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/C/data_binned_pion.root  output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/C/*/data_binned_pion.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Al/data_binned_pion.root output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Al/*/data_binned_pion.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Cu/data_binned_pion.root output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Cu/*/data_binned_pion.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Sn/data_binned_pion.root output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Sn/*/data_binned_pion.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Pb/data_binned_pion.root output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Pb/*/data_binned_pion.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/D2/data_binned_pion.root output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/D2/*/data_binned_pion.root

    hadd -T -f output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/C/data_binned_pion_minus.root  output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/C/*/data_binned_pion_minus.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Al/data_binned_pion_minus.root output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Al/*/data_binned_pion_minus.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Cu/data_binned_pion_minus.root output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Cu/*/data_binned_pion_minus.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Sn/data_binned_pion_minus.root output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Sn/*/data_binned_pion_minus.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Pb/data_binned_pion_minus.root output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Pb/*/data_binned_pion_minus.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/D2/data_binned_pion_minus.root output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/D2/*/data_binned_pion_minus.root

elif [[ $TORUS_POL == "out" ]]; then
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(,\"C\",  $PID, \"acc\")'" &
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(,\"C\",  $PID, \"acc\")'" &
    
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(,\"Pb\", $PID, \"acc\")'" &
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(,\"Pb\", $PID, \"acc\")'" &

    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(,\"D2\", $PID, \"acc\")'" &
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(,\"D2\", $PID, \"acc\")'" &
    wait

    hadd -T -f output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/C/data_binned_pion.root  output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/C/*/data_binned_pion.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Pb/data_binned_pion.root output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Pb/*/data_binned_pion.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/D2/data_binned_pion.root output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/D2/*/data_binned_pion.root

    hadd -T -f output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/C/data_binned_pion_minus.root  output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/C/*/data_binned_pion_minus.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Pb/data_binned_pion_minus.root output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/Pb/*/data_binned_pion_minus.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/D2/data_binned_pion_minus.root output_$FWD_REC/$TORUS_POL/acc/$DETECTOR/D2/*/data_binned_pion_minus.root
fi

#Thrown
if [[ $TORUS_POL == "inb" ]]; then
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(12,\"C\",  $PID, \"thrown\")'" &
    #gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(,\"C\",  $PID, \"thrown\")'" &
    
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(22,\"Al\", $PID, \"thrown\")'" &
    #gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(,\"Al\", $PID, \"thrown\")'" &
    
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(32,\"Cu\", $PID, \"thrown\")'" &
    #gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(,\"Cu\", $PID, \"thrown\")'" &
    
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(42,\"Sn\", $PID, \"thrown\")'" &
    #gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(,\"Sn\", $PID, \"thrown\")'" &
    
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(52,\"Pb\", $PID, \"thrown\")'" &
    #gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(,\"Pb\", $PID, \"thrown\")'" &
    
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(11,\"D2\", $PID, \"thrown\")'" &
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(21,\"D2\", $PID, \"thrown\")'" &
    wait

    hadd -T -f output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/C/data_binned_pion.root  output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/C/*/data_binned_pion.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Al/data_binned_pion.root output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Al/*/data_binned_pion.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Cu/data_binned_pion.root output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Cu/*/data_binned_pion.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Sn/data_binned_pion.root output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Sn/*/data_binned_pion.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Pb/data_binned_pion.root output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Pb/*/data_binned_pion.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/D2/data_binned_pion.root output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/D2/*/data_binned_pion.root

    hadd -T -f output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/C/data_binned_pion_minus.root  output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/C/*/data_binned_pion_minus.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Al/data_binned_pion_minus.root output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Al/*/data_binned_pion_minus.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Cu/data_binned_pion_minus.root output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Cu/*/data_binned_pion_minus.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Sn/data_binned_pion_minus.root output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Sn/*/data_binned_pion_minus.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Pb/data_binned_pion_minus.root output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Pb/*/data_binned_pion_minus.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/D2/data_binned_pion_minus.root output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/D2/*/data_binned_pion_minus.root

elif [[ $TORUS_POL == "out" ]]; then
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(,\"C\",  $PID, \"thrown\")'" &
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(,\"C\",  $PID, \"thrown\")'" &
    
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(,\"Pb\", $PID, \"thrown\")'" &
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(,\"Pb\", $PID, \"thrown\")'" &

    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(,\"D2\", $PID, \"thrown\")'" &
    gnome-terminal --wait -- bash -c "root -l -q -b 'make_multibinning_v3.cpp(,\"D2\", $PID, \"thrown\")'" &
    wait

    hadd -T -f output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/C/data_binned_pion.root  output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/C/*/data_binned_pion.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Pb/data_binned_pion.root output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Pb/*/data_binned_pion.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/D2/data_binned_pion.root output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/D2/*/data_binned_pion.root

    hadd -T -f output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/C/data_binned_pion_minus.root  output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/C/*/data_binned_pion_minus.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Pb/data_binned_pion_minus.root output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/Pb/*/data_binned_pion_minus.root
    hadd -T -f output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/D2/data_binned_pion_minus.root output_$FWD_REC/$TORUS_POL/thrown/$DETECTOR/D2/*/data_binned_pion_minus.root
fi

echo run_make_multibinning finished.