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

gnome-terminal --wait -- bash -c "root -l -q 'make_multibinning_v3.cpp(\"C\", $PID)'" &
gnome-terminal --wait -- bash -c "root -l -q 'make_multibinning_v3.cpp(\"Al\", $PID)'" &
gnome-terminal --wait -- bash -c "root -l -q 'make_multibinning_v3.cpp(\"Cu\", $PID)'" &
gnome-terminal --wait -- bash -c "root -l -q 'make_multibinning_v3.cpp(\"Sn\", $PID)'" &
gnome-terminal --wait -- bash -c "root -l -q 'make_multibinning_v3.cpp(\"Pb\", $PID)'" &
wait

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