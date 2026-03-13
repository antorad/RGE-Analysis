#!/bin/bash

#Second part to run interactively (or in personal computer)
INPUT_DIR="/volatile/clas12/antorad/rge/MR_analysis/data/pass0.10/dc"
OUTPUT_DIR="/work/clas12/rg-e/antorad/RGE-Analysis/output/data"

hadd -f $INPUT_DIR/C/data_binned_pion.root $INPUT_DIR/C/020*/data_binned_pion.root
hadd -f $INPUT_DIR/Al/data_binned_pion.root $INPUT_DIR/Al/020*/data_binned_pion.root
hadd -f $INPUT_DIR/Cu/data_binned_pion.root $INPUT_DIR/Cu/020*/data_binned_pion.root
hadd -f $INPUT_DIR/Sn/data_binned_pion.root $INPUT_DIR/Sn/020*/data_binned_pion.root
hadd -f $INPUT_DIR/Pb/data_binned_pion.root $INPUT_DIR/Pb/020*/data_binned_pion.root

mkdir -p $OUTPUT_DIR/C/ $OUTPUT_DIR/Al/ $OUTPUT_DIR/Cu/ $OUTPUT_DIR/Sn/ $OUTPUT_DIR/Pb/

cp $INPUT_DIR/C/data_binned_pion.root $OUTPUT_DIR/C/
cp $INPUT_DIR/Al/data_binned_pion.root $OUTPUT_DIR/Al/
cp $INPUT_DIR/Cu/data_binned_pion.root $OUTPUT_DIR/Cu/
cp $INPUT_DIR/Sn/data_binned_pion.root $OUTPUT_DIR/Sn/
cp $INPUT_DIR/Pb/data_binned_pion.root $OUTPUT_DIR/Pb/

#integrate multibinning
./run_integrate_multibinning.sh -tar C -pid 211
./run_integrate_multibinning.sh -tar Al -pid 211
./run_integrate_multibinning.sh -tar Cu -pid 211
./run_integrate_multibinning.sh -tar Sn -pid 211
./run_integrate_multibinning.sh -tar Pb -pid 211

# #plots
# ./run_plots_all_mr.sh 211