import uproot as ur
import matplotlib.pyplot as plt
import numpy as np
import optparse
import awkward as ak
import mplhep as hep
import os
from include import Save_Figure, Add_Clas_Pleliminary, clas_preliminary_flag, var_unit, var_range, func_cut_style, data_directory, output_directory
plt.style.use(hep.style.ATLAS)  # or ATLAS/LHCb2


# Construct the argument parser
parser = optparse.OptionParser("usage: %prog [options]\n")
parser.add_option('-x', '--variableX', dest='variableX',
                  default="x_bjorken", help="Variable you want to Plot in the x axes")
parser.add_option('-y', '--variableY', dest='variableY',
                  default="Q2", help="Variable you want to Plot in the y axes")
parser.add_option('-r', '--runNumber', dest='runNumber',
                  default="Pb", help="Run number. Can be target name also")
options, args = parser.parse_args()

draw_DIS_Cut = False
just_electon = True
var_x = options.variableX
var_y = options.variableY
run_number = options.runNumber

nbins = 150

file_name = "ntuples_dc_" + run_number
bins_x = np.linspace(var_range[var_x][0], var_range[var_x][1], num=nbins)
bins_y = np.linspace(var_range[var_y][0], var_range[var_y][1], num=nbins)


def DIS_Cuts_Funct():

    # DIS Cuts
    Q2cut = 1.  # Q2 > 1
    WCut = 4   # W > 2
    Mass2 = .938*.938  # Proton mass squared
    EBeam = 10.5473
    YbCut = 0.85  # yb < 0.85
    x = bins_x
    y1 = np.linspace(Q2cut, Q2cut, nbins)
    y2 = ((WCut-Mass2)*x)/(1-x)
    y3 = 2*YbCut*EBeam*x*(.938)
    label = r'Cuts'

    return x, y1, y2, y3, label


def Plot_Variable_2d(var_x, var_y, just_electon=False):

    with ur.open("%s.root:data" % (data_directory+file_name)) as events:
        branches = events.arrays()

    fig, axs = plt.subplots(1, 1, constrained_layout=True)
    width = 9
    # height = width / 1.4
    height = 6 / 1.4  # ascpect ratio
    fig.set_size_inches(width, height)

    if clas_preliminary_flag:
        Add_CLas_Pleliminary(axs)

    axs.set_xlim(var_range[var_x][0], var_range[var_x][1])
    axs.set_ylim(var_range[var_y][0], var_range[var_y][1])

    # Set plot colors
    # cm = plt.cm.twilight_shifted.reversed()
    cm = plt.cm.viridis
    cmp = cm.copy()
    cmp.set_under(color='white')  # use white if there is no events

    if just_electon:
        #  electron_cuts = electron_cuts & branch
        electron_cuts = (branches["pid"] == 11) & (
            branches["trigger_status"] == 1)
        # Draw 2d map
        h = axs.hist2d(branches[var_x][electron_cuts], branches[var_y][electron_cuts], bins=(
            bins_x, bins_y), cmap=cmp, rasterized=True, vmin=10)

        # Draw DIS cuts
        if draw_DIS_Cut:
            axs.plot(DIS_Cuts_Funct()[0], DIS_Cuts_Funct()[1],
                     label=DIS_Cuts_Funct()[4], **func_cut_style)
            axs.plot(DIS_Cuts_Funct()[0], DIS_Cuts_Funct()[
                     2], **func_cut_style)
            axs.plot(DIS_Cuts_Funct()[0], DIS_Cuts_Funct()[
                     3], **func_cut_style)

    axs.legend(frameon=True, loc='upper left', fontsize=8)
    plt.colorbar(h[3])  # Draw color bar
    print(run_number + " events : " + str(len(branches[var_x][electron_cuts])))

    axs.set_xlabel(r'' + var_x + " " + var_unit[var_x], fontsize=14)
    axs.set_ylabel(r'' + var_y + " " +
                   var_unit[var_y], loc="center", fontsize=15)

    Save_Figure(fig, output_directory + run_number +
                "/", "Sector" + var_x + "_" + var_y)


Plot_Variable_2d(var_x, var_y, just_electon)
