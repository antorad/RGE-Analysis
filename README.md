# RGE Analysis (Multiplicity Ratio)

## Inputs

### Data
Input data is located in the directory /data
Cooked data is located at JLab in /volatile/clas12/rg-e/production/pass0.10/mon/recon/[run_number]
The run number corresponding to each Target is listed in: https://docs.google.com/spreadsheets/d/1qwc4RBENN7k1v0S1YcncNFymR8TP1eyzxwXfJClIrvs/edit?usp=sharing and in /runs/[Target]\_runs.txt

### Simulations
Input simualtions are located in the directory /simul/[Target]
Cooked simulations are located st JLAB in /volatile/clas12/osg/antorad/job_[number]
[number] corresponds of each individual simulation job. The job numbers corresponding to each target is listed in runs/[Target]\_simul_runs.txt

### Tuples
Tuples from cooked data and simualtions are processed into root files by using: https://github.com/antorad/clas12-rge-analysis
Output tuples in root format are located in JLab in /volatile/clas12/antorad/rge/[type]/pass0.10/[dc/fmt2]/ntuple_files.
Raw banks information in root format are located in the same place. Instead of ntuple_files, the directory is called banks_root_files. 

## Analysis steps

1. Run simple_plots to make tuples for electrons and different hadrons.
2. Run simple_mr to make multiplicity ratio plots without corrections for Zh, Nu and Pt2.
3. Run plot_simple_mr_all to plot uncorrected multiplicity ratios from all different targets in the same canvas.
4. Run make_multibinning to divide the data (or simualtions) into 5D binning in Q2, Nu, Zh, Pt2 and PhiPQ.
5. Run integrate_multibinning to calculate and apply Accpetance Correction to each 5D bin and integrate to calculate the acceptance corrected Multiplicity Ratio. This script calculate MR in tye full kinematic range and in different Q2 and Nu bins simultaneously.
6. Run plot_all_mr to plot all corrected multiplicty ratios into the same canvas to compare different targets.
7. Run plot_mr_bin to plot all corrected multiplicity ratios into a gridded canvas to compare the multiplicity ratio between different bins in Q2 and Nu for all targets.

* All bash scripts named with run_*.sh run each of of these steps in its totality, for all targets, particles and varaibles required.
* The bash script named run_all.sh run all other run_*.sh in order, running the complete analysis.

## Directory explanation
* data: input data location
* simul: input simulations location
* runs: list of runs for each target data and simulations in txt format
* output: all root files, tuples and plots produced during the analysis
	* data: output from data alaysis only
	* simul output from simulations analysis only
	* AC_MR: acceptance corrected multiplicity ratios that combines data and simulations.

* C, Al, Cu, Sn, Pb, (D2): subdirectories that contain data or simulations coming from that specific target. In case of simulations, deuterium is included in its own subdirectory, with exception of multiplicity ratios, that requires both solid and liquid target.

* In case of simulations, generated results are within _thrown_ subdirectories.

## Macro explanations
* simple_plots(run_N, Target, type)/(inputFileName, Target, type):
	* Usage: Can be run with either a run number as argument or a text file with multiple run numbers. In the case of a run number, it will look in `type` subdirectory for a file correspond to the corresponding run. The other option is to give a text file name as argument and the output’s directory name. The text file must have run numbers, one per line, all runs will be combined for a single output.
	* Function: the macro takes the data files and creates separated root tuples with the necessary variables for the analysis for electrons, positive pions, negative pions, protons and all positive particles combined for further analysis. It also creates pdf plots for simple 1D and 2D plots for first checks of data. It applies DIS cuts for single hadron plots only. The tuples created and the plots are stored in the output file `out_clas12.root`
	* Output:
		* Root file: out_clas12.root containing tuples for electrons, pions, positives.
		* Pdf files: multiple 1D and 2D simple distribution plots.

* simple_mr(Target, Hadron_pid, type):
	* Usage: The macro takes the target name and the pid of the particle to calculate the multiplicity ratio. It requieres simple_plots to be run before.
	* Function: This code calculate 1D multiplicity ratio for Zh, Nu and Pt2 for the requested particle in a very simple way. It doesn’t apply any type of correction.
It takes the tuples generated from simple_plots.cxx and applies DIS and target cuts. It divides the hadron and electron data into binned TH1s and divide them.
	* Output:
		* Root file: mr_clas12.root cointaining variable distributions for hadron, electron for solid and liquid target, the multiplicities and multiplicity ratio ratio plot.

* plot_simple_mr_all(Hadron_pid, var):
	* Usage: It requieres the hadron and variable to be plotted. It requieres simple_mr to be run before for each target.
	* Function: It plots the non corrected multiplicity ratio from all tagets in the same canvas.
	* Output: 
		* Pdf files: Plot of multiplicity ratio for a specific variable with all targets.

* mr_ac_1d(Target, Hadron_pid):
	* Usage: It requieres the target and hadron to be plotted. It requieres simple_mr to be run first for data and simulations.
	* Function: Calculate hadron acceptance corrected 1D multiplicity ratio for Zh, Nu and Pt2.
	* Output:
		* Root file: mr_1d_[hadron].root cotaining hadron and electron acceptance corrected distributions for solid and liquid target, corrected multiplicities and multiuplicity ratios.

* make_multibinning(Target, Hadron_pid, type):
	* Usage: As arguments, the macro takes the target name, the pid of the particle to calculate and the type of data. It requieres simple_plots to be run before.
	* Function: This macro takes the ntuples created with simple_plots.cpp, applies DIS and target cuts and divides the data for a specific hadron into multiples bins, for solid and liquid target separately. It divides the data into 2D histograms with Q2 and Nu electron variables in x and y axis, for each bin in Zh, Pt2 and Phi_PQ variables, making it a 5 dimensional binning. Each TH2D histogram has the name format Data_[targ_type]_n1_n2_n3 where n1, n2 and n3 represents the bin number in Zh, Pt2 and Phi_PQ respectively.
	* Output:
		* Root file: It cointains n1*n2*n3 2D histograms in NuxQ2 for liquid and solid target.

* integrate_multibinning(Target, Hadron_pid, mainVar):
	* Usage: As arguments, the macro takes the target name, the pid of the particle to calculate and the type of data. It requieres make_multibinning to be run before for data and simualtions.
	* Function: - This macro takes the histograms created in `make_multibinnin.cpp`, for data and simulations, calculate the acceptance correction factor for each bin by using the simulations and then apply the correction factors to the data for electrons and hadrons. Then it calculate the corrected and uncorrected multiplicity ratio in the total kinematic range and for each Nu and Q2 bin.
	The order the script calculated the multiplicity ratio is:
    1. Get hadron histograms from root file for each of the target types, for data, acc and thr.
    2. Create histograms for integrated data for each bin of the final MR plot.
    3. Calculate the acceptance correction factor and apply it to the data for each Q2xNu histogram.
    4. The corrected histogram is added to the corresponding integrated histogram for each bin of the required variable.
    5. Once all bins all corrected and integrated into a final histogram in NuxQ2 for each bin of the required variable, it integrates all bin of the histogram into a final number that corresponds to the value of the bin and error in the required variable.
    6. It set these values for each bin in a histogram in the required variable.
    7. Take electron tuples created in `simple_plots.cxx` for data, acc and thr.
    8. Create NuxQ2 histograms for electrons.
    9. Calculate the acceptance correction by dividing NUxQ2 histos form throw and acc and apply it to the electron data.
    10. Create an electron histogram to apply into the MR calculation. In case of hadron variable, it creates a flat histogram by integrating the 2D NuxQ2 histogram and getting a single number and creating a TH1D. In case of an electron variable, it projects the 2D into a 1D histogram.
    11. Calculation of the MR for corrected and uncorrected data.
    12. Save histograms into root file `mr.root`.
    Depending if running the script for an electron or hadron variable, the script will calculate the ratios in different ways. 
    The script calculate the total and binned MR at the same time.
	* Output:
		* Root file: contains integrated 2D histograms for each of the final hadron variable plot, integrated 1D histogram distribution of the main varaible for each of the electron variable bins, total intregrated 2D histograms for hadrons and electrons, number of electrons for each electron bin, total multiplicites and multiplicity ratios, corrected and uncorrected and for each electron bin.

* plot_mr_all(Hadron_pid, mainVar):
	* Usage: It uses pid and varaible as arguments. It requieres integrate_multibinning to be run first.
	* Function: It plots the acceptance corrected  multiplicity ratio for all targets in the same canvas.
	* Output: A plot with the acceptance corrected multiplicity ratio plotted for all targets.

* plot_mr_bin(Hadron_pid, Var):
	* Usage: It uses pid and varaible as arguments. It requieres integrate_multibinning to be run first.
	* Function: It plots a canvas with multiples subplots, one for each electron bin, the multiplicity ratio for ll targets.
	* Output:
		* Pdf file: A plot with multiple subplots with the multiplicity ratio plotted for all targets in each electron bin.

##  TODO
* Write macro explanations DONE
* Change make_multibinning algorithm from bin to bin basis in selection to event by event. DONE
* In case of deciding on make individual vertex cuts, incurporate the fitting in the tuple making script and apply them at the moment instead of a global fit in include
* Change "run" macros to run in parallel when necessary and make the initial process to wait until all the parallel process are done to finish.
* Optimize the workflow for the cluster (run each run separately in different jobs and combine them after)