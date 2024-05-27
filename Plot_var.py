import uproot as ur
import matplotlib.pyplot as plt
import numpy as np
import optparse
import awkward as ak
import mplhep as hep
import os
from include import Save_Figure, Add_Clas_Pleliminary, clas_preliminary_flag, var_unit, var_range, var_label, data_directory, output_directory

plt.style.use(hep.style.ATLAS)  # or ATLAS/LHCb2

# Construct the argument parser
parser = optparse.OptionParser("usage: %prog [options]\n")
parser.add_option('-v', '--variable', dest='variable',
                  default="v_z", help="Variable you want to Plot")
parser.add_option('-r', '--runNumber', dest='runNumber',
                  default="Pb", help="Run number. Can be target name also")
options, args = parser.parse_args()

just_electon = True
var = options.variable
run_number = options.runNumber

nbins = 100

file_name = "ntuples_dc_" + run_number


def Plot_Variable(var, just_electon=False):

    hist_color = "#301437"
    hist_color_ec = "#301437"

    # open data file
    with ur.open("%s.root:data" % (data_directory+file_name)) as events:
        branches = events.arrays()

    fig, axs = plt.subplots(1, 1, constrained_layout=True)
    width = 6
    height = width / 1.2
    fig.set_size_inches(width, height)

    if clas_preliminary_flag:
        Add_Clas_Pleliminary(axs)

    axs.set_xlim(var_range[var][0], var_range[var][1])
    bins = np.linspace(var_range[var][0], var_range[var][1], num=nbins)

    # to plot just electrons
    if just_electon:
        #  electron_cuts = electron_cuts & branch & DIS cuts
        electron_cuts = (branches["pid"] == 11) & (branches["trigger_status"] == 1) & (
            branches["Q2"] > 1) & (branches["W2"] > 2)
        axs.hist(branches[var][electron_cuts], bins,
                 color=hist_color, ec=hist_color_ec)
        print(run_number + " events : " +
              str(len(branches[var][electron_cuts])))
    # To plot all the particles or an hadron variable
    else:
        axs.hist(branches[var][branches["pid"] == 321], bins,
                 color=hist_color, ec=hist_color_ec)

    axs.set_xlabel(var_label[var], fontsize=14)
    axs.set_ylabel(r'$dN/d%s$' % var, loc="center", fontsize=15)

    Save_Figure(fig, "./output/" + run_number + "/", file_name + "_" + var)


def Plot_Variable_Sector(var, just_electon=False):

    hist_color = "#301437"
    hist_color_ec = "#301437"

    with ur.open("%s.root:data" % (data_directory+file_name)) as events:
        branches = events.arrays()

    fig, axs = plt.subplots(2, 3, sharey='row', sharex='col')
    width = 16  # 7.056870070568701
    height = 8
    fig.set_size_inches(width, height)

    fig.subplots_adjust(left=None, bottom=None, right=None,
                        top=None, wspace=0.02, hspace=0.2)

    # Loops in clas sectors
    for i in range(2):
        for j in range(3):

            if clas_preliminary_flag:
                Add_Clas_Pleliminary(axs[i][j])

            sector = 3*i+j + 1  # sector number
            axs[i][j].set_xlim(var_range[var][0], var_range[var][1])
            bins = np.linspace(var_range[var][0], var_range[var][1], num=nbins)

            if just_electon:
                #  electron_cuts = electron_cuts & branch & DIS cuts
                electron_cuts = (branches["pid"] == 11) & (branches["trigger_status"] == 1) & (
                    branches["Q2"] > 1) & (branches["sector"] == sector)
                axs[i][j].hist(branches[var][electron_cuts],
                               bins, color=hist_color, ec=hist_color_ec, fill=True, histtype='step',
                               label="$Q^2$ > 1 GeV^2")

                # if you want to compare histograms with diferent cuts uncomment this

                # electron_cuts = (branches["pid"] == 11) & (branches["trigger_status"] == 1) & (
                # branches["Q2"] > 2.0) & (branches["sector"] == sector)
                # axs[i][j].hist(branches[var][electron_cuts], bins,
                # color="orange", ec="orange", label="$Q^2 > 2 GeV^2$")

            # To plot all the particles or an hadron variable
            else:
                general_cuts = (branches[var] != 0) & (
                    branches["sector"] == sector)
                axs[i][j].hist(branches[var][general_cuts], bins,
                               color=hist_color, ec=hist_color_ec)

            axs[i][j].annotate(r'Sector ' + str(sector), xy=(0.42, 1.04),
                               xycoords='axes fraction', fontsize=15, weight="bold")

            # axs[i][j].set_yscale('log') ## uncomment to use log scale
            axs[i][j].set_xlabel(var_label[var], fontsize=14)
        axs[i][0].set_ylabel(r'$dN/d%s$' % var, loc="center", fontsize=15)
    fig.align_ylabels(axs[:])
    # axs[0][0].legend(frameon=False, loc='upper left', fontsize=11) ## Draw legend

    Save_Figure(fig, output_directory + run_number + "/", "Sectors_" + var)


Plot_Variable(var, just_electon)
Plot_Variable_Sector(var, just_electon)
