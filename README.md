# RGE Preliminary Plots

## Tuples

Tuples were generated using https://github.com/sebouh137/clas12-rge-analysis

## Data directory

Add those outputs to the data directory. By default is ./data/
if you want to change it go to include.py, include/Binning.h, Calculate_mr.cpp

## This was tested in my local computer not in the cluster

## Plots

To generate the mupliplicity ratio and broadening plots run 
bash Run_Everything.sh

## Codes

The cpp codes make the analysis and the python codes the plots.
./Calculate_mr.cpp calculate the mr and generated a tuples for hadrons and electrons separatly.

broadening codes are really ineficent because I modify my clas6 codes which include acceptance correction (now do not apply that correction).

You can easily compare diffent cuts in a distribution uncommenting a few lines in Plot_var.py (look at Plot_Variable_Sector function)
