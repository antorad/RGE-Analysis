# RGE Analysis (Multiplicity Ratio)

## Inputs

### Data
Input data is located in the directory data
Cooked data is located in JLab in /volatile/clas12/rg-e/production/pass0.7/mon/recon/[run_number]
The run number corresponding to each Target is listed in: https://docs.google.com/spreadsheets/d/1qwc4RBENN7k1v0S1YcncNFymR8TP1eyzxwXfJClIrvs/edit?usp=sharing and in runs/[Target]\_runs.txt

### Simulations
Input simualtions are located in the directory simul/[Target]
Simulations are located in JLAB in /volatile/clas12/osg/antorad/job_[number]
[number] corresponds of each individual simulation job. The job numbers corresponding to each target is listed in runs/[Target]\_simul_runs.txt

### Tuples
Tuples from cooked data and simualtions are processed into root files by using: https://github.com/antorad/clas12-rge-analysis
To run the hipo2root converter, the scripts from the directory farm_codes are used from the JLab farm.

## Analysis steps

1. Run simple_plots to make tuples for electrons and different hadrons.
2. Run simple_mr to make multiplicity ratio plots without corrections for Zh, Nu and Pt2.
3. Run plot_simple_mr_all to plot uncorrected multiplicity ratios from all different targets in the same canvas.
4. Run make_multibinning to divide the data (or simualtions) into 5D binning in Q2, Nu, Zh, Pt2 and PhiPQ.
5. Run integrate_multibinning to calculate and apply Accpetance Correction to each 5D bin and integrate all to calculate the acceptance corrected Multiplicity Ratio.
6. Run plot_plots_all_mr to plot all corrected multiplicty ratios into the same canvas to compare different targets.
7. Run plot_mr_bin to plot all corrected multiplicity ratios into a gridded canvas to compare the multiplicity ratio between different bins in Q2 and Nu for all targets.

* All bash scripts named with run_*.sh run each of of these steps in its totatlity, for all targets, particles and varaibles required.
* The bash script named run_all.sh run all other run_*.sh in order, running the complete analysis.

## Directory explanation
* data: input data location
* simul: input simulations location
* farm_codes: script used in JLab farm only
* runs: list of runs for each target data and simulations
* output: all root files, tuples and plots produced during the analysis
	* data: output from data alaysis only
	* simul output from simulations analysis only
	* AC_MR: acceptance corrected multiplicity ratios that combines data and simulations.

* C, Al, Cu, Sn, Pb, (D2): subdirectories that contain data or simulations coming from that specific target. In case of simulations, deuterium is included in its own subdirectory, with exception of multiplicity ratios, that requires both solid and liquid target.

* In case of simulations, generated results are within _thrown_ subdirectories.

## Macro explanations
TODO

##  TODO
* Write macro explanations
* Change make_multibinning algorithm from bin to bin basis in selection to event by event.
* Change "run" macros to run in parallel when necessary and make the initian process to wait until all the parallel process are done to finish.