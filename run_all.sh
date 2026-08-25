#!/bin/bash

# Parse command-line arguments
while getopts "m:b:" opt; do
  case $opt in
    m) TORUS_POL=$OPTARG;;
    b) FWD_REC=$OPTARG;;
    \?) echo "Invalid option: -$OPTARG" >&2;;
  esac
done

#simple plots
./run_simple_plots.sh -m $TORUS_POL -b $FWD_REC

#simple MR
./run_simple_mr.sh -p 211 -m $TORUS_POL
./run_simple_mr.sh -p -211 -m $TORUS_POL

#1D AC MR
#./run_mr_ac_1d.sh 211

#make multibbining
./run_make_multibinning.sh -p 211 -m $TORUS_POL -b $FWD_REC
./run_make_multibinning.sh -p -211 -m $TORUS_POL -b $FWD_REC

#integrate multibinning
./run_integrate_multibinning.sh -t C -p 211
if [[ $TORUS_POL == "inb" ]]; then
./run_integrate_multibinning.sh -t Al -p 211
./run_integrate_multibinning.sh -t Cu -p 211
./run_integrate_multibinning.sh -t Sn -p 211
fi
./run_integrate_multibinning.sh -t Pb -p 211

./run_integrate_multibinning.sh -t C -p -211
if [[ $TORUS_POL == "inb" ]]; then
./run_integrate_multibinning.sh -t Al -p -211
./run_integrate_multibinning.sh -t Cu -p -211
./run_integrate_multibinning.sh -t Sn -p -211
fi
./run_integrate_multibinning.sh -t Pb -p -211

#plots
./run_plots_all_mr.sh -p 211
./run_plots_all_mr.sh -p -211