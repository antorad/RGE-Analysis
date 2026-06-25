#!/bin/bash

#Second part to run interactively in farm
INPUT_DIR="/volatile/clas12/antorad/rge/MR_analysis/data/pass1/dc"
OUTPUT_DIR="/work/clas12/rg-e/antorad/RGE-Analysis/output/data"

#out_clas12
hadd -f $INPUT_DIR/C/out_clas12.root $INPUT_DIR/C/020*/out_clas12.root
hadd -f $INPUT_DIR/Al/out_clas12.root $INPUT_DIR/Al/020*/out_clas12.root
hadd -f $INPUT_DIR/Cu/out_clas12.root $INPUT_DIR/Cu/020*/out_clas12.root
hadd -f $INPUT_DIR/Sn/out_clas12.root $INPUT_DIR/Sn/020*/out_clas12.root
hadd -f $INPUT_DIR/Pb/out_clas12.root $INPUT_DIR/Pb/020*/out_clas12.root

#pion_plus
hadd -f $INPUT_DIR/C/data_binned_pion.root $INPUT_DIR/C/020*/data_binned_pion.root
hadd -f $INPUT_DIR/Al/data_binned_pion.root $INPUT_DIR/Al/020*/data_binned_pion.root
hadd -f $INPUT_DIR/Cu/data_binned_pion.root $INPUT_DIR/Cu/020*/data_binned_pion.root
hadd -f $INPUT_DIR/Sn/data_binned_pion.root $INPUT_DIR/Sn/020*/data_binned_pion.root
hadd -f $INPUT_DIR/Pb/data_binned_pion.root $INPUT_DIR/Pb/020*/data_binned_pion.root

#pion_minus
hadd -f $INPUT_DIR/C/data_binned_pion_minus.root $INPUT_DIR/C/020*/data_binned_pion_minus.root
hadd -f $INPUT_DIR/Al/data_binned_pion_minus.root $INPUT_DIR/Al/020*/data_binned_pion_minus.root
hadd -f $INPUT_DIR/Cu/data_binned_pion_minus.root $INPUT_DIR/Cu/020*/data_binned_pion_minus.root
hadd -f $INPUT_DIR/Sn/data_binned_pion_minus.root $INPUT_DIR/Sn/020*/data_binned_pion_minus.root
hadd -f $INPUT_DIR/Pb/data_binned_pion_minus.root $INPUT_DIR/Pb/020*/data_binned_pion_minus.root

#Create directories in /work dir
mkdir -p $OUTPUT_DIR/C/
mkdir -p $OUTPUT_DIR/Al/
mkdir -p $OUTPUT_DIR/Cu/
mkdir -p $OUTPUT_DIR/Sn/
mkdir -p $OUTPUT_DIR/Pb/

#copy outputs to /work 
cp $INPUT_DIR/C/out_clas12.root $OUTPUT_DIR/C/
cp $INPUT_DIR/Al/out_clas12.root $OUTPUT_DIR/Al/
cp $INPUT_DIR/Cu/out_clas12.root $OUTPUT_DIR/Cu/
cp $INPUT_DIR/Sn/out_clas12.root $OUTPUT_DIR/Sn/
cp $INPUT_DIR/Pb/out_clas12.root $OUTPUT_DIR/Pb/

cp $INPUT_DIR/C/data_binned_pion.root $OUTPUT_DIR/C/
cp $INPUT_DIR/Al/data_binned_pion.root $OUTPUT_DIR/Al/
cp $INPUT_DIR/Cu/data_binned_pion.root $OUTPUT_DIR/Cu/
cp $INPUT_DIR/Sn/data_binned_pion.root $OUTPUT_DIR/Sn/
cp $INPUT_DIR/Pb/data_binned_pion.root $OUTPUT_DIR/Pb/

cp $INPUT_DIR/C/data_binned_pion_minus.root $OUTPUT_DIR/C/
cp $INPUT_DIR/Al/data_binned_pion_minus.root $OUTPUT_DIR/Al/
cp $INPUT_DIR/Cu/data_binned_pion_minus.root $OUTPUT_DIR/Cu/
cp $INPUT_DIR/Sn/data_binned_pion_minus.root $OUTPUT_DIR/Sn/
cp $INPUT_DIR/Pb/data_binned_pion_minus.root $OUTPUT_DIR/Pb/

#simple MR
./run_simple_mr.sh 211
./run_simple_mr.sh -211

#integrate multibinning
./run_integrate_multibinning.sh -tar C -pid 211
./run_integrate_multibinning.sh -tar Al -pid 211
./run_integrate_multibinning.sh -tar Cu -pid 211
./run_integrate_multibinning.sh -tar Sn -pid 211
./run_integrate_multibinning.sh -tar Pb -pid 211

./run_integrate_multibinning.sh -tar C -pid -211
./run_integrate_multibinning.sh -tar Al -pid -211
./run_integrate_multibinning.sh -tar Cu -pid -211
./run_integrate_multibinning.sh -tar Sn -pid -211
./run_integrate_multibinning.sh -tar Pb -pid -211

#plots
./run_plots_all_mr.sh 211
./run_plots_all_mr.sh -211