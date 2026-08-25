#!/bin/bash

# Parse command-line arguments
while getopts "m:b:" opt; do
  case $opt in
    m) TORUS_POL=$OPTARG;;
    b) FWD_REC=$OPTARG;;
    \?) echo "Invalid option: -$OPTARG" >&2;;
  esac
done

#Second part to run interactively in farm
INPUT_DIR="/volatile/clas12/antorad/rge/MR_analysis/data/pass1/$TORUS_POL/$FWD_REC"
OUTPUT_DIR="/work/clas12/rg-e/antorad/RGE-Analysis/output_$FWD_REC/$TORUS_POL/data"
#torus: inbending or outbendig

#out_clas12
echo "Merging out_clas12 ROOT files"
hadd -T -f $INPUT_DIR/C/out_clas12.root  $INPUT_DIR/C/020*/out_clas12.root
if [[ $TORUS_POL == "inb" ]]; then
hadd -T -f $INPUT_DIR/Al/out_clas12.root $INPUT_DIR/Al/020*/out_clas12.root
hadd -T -f $INPUT_DIR/Cu/out_clas12.root $INPUT_DIR/Cu/020*/out_clas12.root
hadd -T -f $INPUT_DIR/Sn/out_clas12.root $INPUT_DIR/Sn/020*/out_clas12.root
fi
hadd -T -f $INPUT_DIR/Pb/out_clas12.root $INPUT_DIR/Pb/020*/out_clas12.root

#pion_plus
echo "Merging positive pion binned ROOT files"
hadd -T -f $INPUT_DIR/C/data_binned_pion.root  $INPUT_DIR/C/020*/data_binned_pion.root
if [[ $TORUS_POL == "inb" ]]; then
hadd -T -f $INPUT_DIR/Al/data_binned_pion.root $INPUT_DIR/Al/020*/data_binned_pion.root
hadd -T -f $INPUT_DIR/Cu/data_binned_pion.root $INPUT_DIR/Cu/020*/data_binned_pion.root
hadd -T -f $INPUT_DIR/Sn/data_binned_pion.root $INPUT_DIR/Sn/020*/data_binned_pion.root
fi
hadd -T -f $INPUT_DIR/Pb/data_binned_pion.root $INPUT_DIR/Pb/020*/data_binned_pion.root

#pion_minus
echo "Merging negative pion binned ROOT files"
hadd -T -f $INPUT_DIR/C/data_binned_pion_minus.root  $INPUT_DIR/C/020*/data_binned_pion_minus.root
if [[ $TORUS_POL == "inb" ]]; then
hadd -T -f $INPUT_DIR/Al/data_binned_pion_minus.root $INPUT_DIR/Al/020*/data_binned_pion_minus.root
hadd -T -f $INPUT_DIR/Cu/data_binned_pion_minus.root $INPUT_DIR/Cu/020*/data_binned_pion_minus.root
hadd -T -f $INPUT_DIR/Sn/data_binned_pion_minus.root $INPUT_DIR/Sn/020*/data_binned_pion_minus.root
fi
hadd -T -f $INPUT_DIR/Pb/data_binned_pion_minus.root $INPUT_DIR/Pb/020*/data_binned_pion_minus.root

#Create directories in /work dir
echo "Creating directories in work"
mkdir -p $OUTPUT_DIR/C/
if [[ $TORUS_POL == "inb" ]]; then
mkdir -p $OUTPUT_DIR/Al/
mkdir -p $OUTPUT_DIR/Cu/
mkdir -p $OUTPUT_DIR/Sn/
fi
mkdir -p $OUTPUT_DIR/Pb/

#copy outputs to /work
echo "Copying out_clas12 ROOT files to work"
cp $INPUT_DIR/C/out_clas12.root  $OUTPUT_DIR/C/
if [[ $TORUS_POL == "inb" ]]; then
cp $INPUT_DIR/Al/out_clas12.root $OUTPUT_DIR/Al/
cp $INPUT_DIR/Cu/out_clas12.root $OUTPUT_DIR/Cu/
cp $INPUT_DIR/Sn/out_clas12.root $OUTPUT_DIR/Sn/
fi
cp $INPUT_DIR/Pb/out_clas12.root $OUTPUT_DIR/Pb/

echo "Copying positive pion binned ROOT files to work"
cp $INPUT_DIR/C/data_binned_pion.root  $OUTPUT_DIR/C/
if [[ $TORUS_POL == "inb" ]]; then
cp $INPUT_DIR/Al/data_binned_pion.root $OUTPUT_DIR/Al/
cp $INPUT_DIR/Cu/data_binned_pion.root $OUTPUT_DIR/Cu/
cp $INPUT_DIR/Sn/data_binned_pion.root $OUTPUT_DIR/Sn/
fi
cp $INPUT_DIR/Pb/data_binned_pion.root $OUTPUT_DIR/Pb/

echo "Copying negative pion binned ROOT files to work"
cp $INPUT_DIR/C/data_binned_pion_minus.root  $OUTPUT_DIR/C/
if [[ $TORUS_POL == "inb" ]]; then
cp $INPUT_DIR/Al/data_binned_pion_minus.root $OUTPUT_DIR/Al/
cp $INPUT_DIR/Cu/data_binned_pion_minus.root $OUTPUT_DIR/Cu/
cp $INPUT_DIR/Sn/data_binned_pion_minus.root $OUTPUT_DIR/Sn/
fi
cp $INPUT_DIR/Pb/data_binned_pion_minus.root $OUTPUT_DIR/Pb/

#simple MR
echo "Running simple_mr"
./run_simple_mr.sh -pid 211 -pol $TORUS_POL
./run_simple_mr.sh -pid -211 -pol $TORUS_POL

#integrate multibinning
echo "Running integrate_multibinning"
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
echo "Running plots_all_mr"
./run_plots_all_mr.sh 211
./run_plots_all_mr.sh -211