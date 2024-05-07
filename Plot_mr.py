import ROOT
import uproot as ur
import matplotlib.pyplot as plt
import optparse
import numpy as np
import awkward as ak
import mplhep as hep
import os
from include import Save_Figure, Add_Clas_Pleliminary, clas_preliminary_flag, var_unit, var_range, data_directory, output_directory

plt.style.use(hep.style.ATLAS)  # or ATLAS/LHCb2

# Construct the argument parser
parser = optparse.OptionParser("usage: %prog [options]\n")
parser.add_option('-v', '--variable', dest='variable',
                  default="z_h", help="Variable you want to Plot")
options, args = parser.parse_args()

var = options.variable

nbins = 100
shift = 0.0  # shift in the x axes for better readability

run_number_list = ["C", "Cu", "Sn", "Pb"]
# run_number_list = ["020046", "020232"]

tarList = ["C", "Cu", "Sn", "Pb"]
colorList = ["red", "Blue", "green", "black"]
markerList = ["o", "o", "o", "o"]
y_range = {"z_h": [0.3, 1.4],
           "nu": [0.7, 1.2],
           }
# tarList = ["Pb - First round", "Pb - Second round" ]
# markerList = ["<", ">"]
# colorList = ["blue", "black"]


def Plot_Mr():

    fig, axs = plt.subplots(1, 1, constrained_layout=True)
    width = 6
    height = width / 1.2
    fig.set_size_inches(width, height)

    if clas_preliminary_flag:
        Add_Clas_Pleliminary(axs)

    for i in range(len(run_number_list)):  # Loops on the diffrent targets

        axs.set_xlim(var_range[var][0], var_range[var][1])
        axs.set_ylim(y_range[var][0], y_range[var][1])

        # Read TGraph information
        file = ROOT.TFile.Open(
            output_directory + run_number_list[i] + "/out_clas12_" + run_number_list[i] + ".root", "READ")

        graphName = "multiplicity_ratio_" + var
        graph = file.Get(graphName)
        nPoints = graph.GetN()
        x = np.ndarray(nPoints, dtype=float, buffer=graph.GetX())
        x = x + (-shift + shift*2*i)
        y = np.ndarray(nPoints, dtype=float, buffer=graph.GetY())
        ey = np.ndarray(nPoints, dtype=float, buffer=graph.GetEY())

        # ---- Print TGraph information -----
        # print("Target :" + run_number_list[i])
        # print("Error: ")
        # print(ey)
        # print("Mr : ")
        # print(y)
        # print("Error %: ")
        # print((ey/y)*100)

        # Plot MR
        axs.errorbar(x, y, ey, marker=markerList[i], linestyle="",
                     markerfacecolor=colorList[i], color=colorList[i],
                     markersize=4.5, label=tarList[i])

        file.Close()

    axs.set_ylabel(
        r'$R$', loc="center", fontsize=15)

    axs.set_xlabel(r'' + var + var_unit[var], loc="center", fontsize=14)

    axs.legend(ncol=1, frameon=False, loc='upper right', fontsize=11)

    axs.grid(visible=None, axis='both', color='0.95')
    axs.set_axisbelow(True)

    Save_Figure(fig, output_directory, "Multiplicity_Ratio_" + var)


Plot_Mr()
