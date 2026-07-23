#!/bin/bash

# Check polarity
if [ -z "$1" ]; then
  echo "Usage: $0 <polarity: inb or out>"
  exit 1
fi
TORUS_POL=$1

#simple plots
./run_simple_plots.sh $TORUS_POL

#simple MR
./run_simple_mr.sh -pid 211 -pol $TORUS_POL
./run_simple_mr.sh -pid -211 -pol $TORUS_POL

#1D AC MR
#./run_mr_ac_1d.sh 211

#make multibbining
./run_make_multibinning.sh -pid 211 -pol $TORUS_POL
./run_make_multibinning.sh -pid -211 -pol $TORUS_POL

#integrate multibinning
./run_integrate_multibinning.sh -tar C -pid 211
if [[ $TORUS_POL == "inb" ]]; then
./run_integrate_multibinning.sh -tar Al -pid 211
./run_integrate_multibinning.sh -tar Cu -pid 211
./run_integrate_multibinning.sh -tar Sn -pid 211
fi
./run_integrate_multibinning.sh -tar Pb -pid 211

./run_integrate_multibinning.sh -tar C -pid -211
if [[ $TORUS_POL == "inb" ]]; then
./run_integrate_multibinning.sh -tar Al -pid -211
./run_integrate_multibinning.sh -tar Cu -pid -211
./run_integrate_multibinning.sh -tar Sn -pid -211
fi
./run_integrate_multibinning.sh -tar Pb -pid -211

#plots
./run_plots_all_mr.sh 211
./run_plots_all_mr.sh -211