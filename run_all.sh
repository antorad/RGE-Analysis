#!/bin/bash

#simple plots
./run_simple_plots.sh

#simple MR
./run_simple_mr.sh

#1D AC MR
./run_mr_ac_1d.sh

#make multibbining
./run_make_multibinning.sh 211

#integrate multibinning
./run_integrate_multibinning.sh -tar C -pid 211
./run_integrate_multibinning.sh -tar Al -pid 211
./run_integrate_multibinning.sh -tar Cu -pid 211
./run_integrate_multibinning.sh -tar Sn -pid 211
./run_integrate_multibinning.sh -tar Pb -pid 211

#plots
./run_plots_all_mr.sh 211