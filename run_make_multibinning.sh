#!/bin/bash

#Step 2 of the analysis.
#This script runs make_multibinning for each target.
#It divides the events in TH2 histos in a 5D binning.

# Check if a PID argument is provided
if [ -z "$1" ]; then
  echo "Usage: $0 <pid>"
  exit 1
fi

PID=$1

gnome-terminal --wait -- bash -c "root -l -q 'make_multibinning_v3.cpp(20156,\"C\",  $PID, \"data\")'" &
gnome-terminal --wait -- bash -c "root -l -q 'make_multibinning_v3.cpp(20161,\"C\",  $PID, \"data\")'" &
wait
gnome-terminal --wait -- bash -c "root -l -q 'make_multibinning_v3.cpp(20437,\"Al\", $PID, \"data\")'" &
gnome-terminal --wait -- bash -c "root -l -q 'make_multibinning_v3.cpp(20482,\"Al\", $PID, \"data\")'" &
wait
gnome-terminal --wait -- bash -c "root -l -q 'make_multibinning_v3.cpp(20178,\"Cu\", $PID, \"data\")'" &
gnome-terminal --wait -- bash -c "root -l -q 'make_multibinning_v3.cpp(20205,\"Cu\", $PID, \"data\")'" &
wait
gnome-terminal --wait -- bash -c "root -l -q 'make_multibinning_v3.cpp(20344,\"Sn\", $PID, \"data\")'" &
gnome-terminal --wait -- bash -c "root -l -q 'make_multibinning_v3.cpp(20383,\"Sn\", $PID, \"data\")'" &
wait
gnome-terminal --wait -- bash -c "root -l -q 'make_multibinning_v3.cpp(20265,\"Pb\", $PID, \"data\")'" &
gnome-terminal --wait -- bash -c "root -l -q 'make_multibinning_v3.cpp(20315,\"Pb\", $PID, \"data\")'" &
wait

hadd -T -f output/data/C/data_binned_pion.root output/data/C/*/data_binned_pion.root
hadd -T -f output/data/Al/data_binned_pion.root output/data/Al/*/data_binned_pion.root
hadd -T -f output/data/Cu/data_binned_pion.root output/data/Cu/*/data_binned_pion.root
hadd -T -f output/data/Sn/data_binned_pion.root output/data/Sn/*/data_binned_pion.root
hadd -T -f output/data/Pb/data_binned_pion.root output/data/Pb/*/data_binned_pion.root

hadd -T -f output/data/C/data_binned_pion_minus.root output/data/C/*/data_binned_pion_minus.root
hadd -T -f output/data/Al/data_binned_pion_minus.root output/data/Al/*/data_binned_pion_minus.root
hadd -T -f output/data/Cu/data_binned_pion_minus.root output/data/Cu/*/data_binned_pion_minus.root
hadd -T -f output/data/Sn/data_binned_pion_minus.root output/data/Sn/*/data_binned_pion_minus.root
hadd -T -f output/data/Pb/data_binned_pion_minus.root output/data/Pb/*/data_binned_pion_minus.root

#For now C and D2 only. When simulations are done, I'll add the rest of targets
#gnome-terminal -- bash -c "root -l -q 'make_multibinning_v3.cpp(\"D2\", $PID, \"acc\")'"
#gnome-terminal -- bash -c "root -l -q 'make_multibinning_v3.cpp(\"C\" , $PID, \"acc\")'"
#gnome-terminal -- bash -c "root -l -q 'make_multibinning_v3.cpp(\"Al\" , $PID, \"acc\")'"
#gnome-terminal -- bash -c "root -l -q 'make_multibinning_v3.cpp(\"Cu\" , $PID, \"acc\")'"
#gnome-terminal -- bash -c "root -l -q 'make_multibinning_v3.cpp(\"Sn\" , $PID, \"acc\")'"
#gnome-terminal -- bash -c "root -l -q 'make_multibinning_v3.cpp(\"Pb\" , $PID, \"acc\")'"
#
#gnome-terminal -- bash -c "root -l -q 'make_multibinning_v3.cpp(\"D2\", $PID, \"thrown\")'"
#gnome-terminal -- bash -c "root -l -q 'make_multibinning_v3.cpp(\"C\" , $PID, \"thrown\")'"
#gnome-terminal -- bash -c "root -l -q 'make_multibinning_v3.cpp(\"Al\" , $PID, \"thrown\")'"
#gnome-terminal -- bash -c "root -l -q 'make_multibinning_v3.cpp(\"Cu\" , $PID, \"thrown\")'"
#gnome-terminal -- bash -c "root -l -q 'make_multibinning_v3.cpp(\"Sn\" , $PID, \"thrown\")'"
#gnome-terminal -- bash -c "root -l -q 'make_multibinning_v3.cpp(\"Pb\" , $PID, \"thrown\")'"

echo run_make_multibinning finished.