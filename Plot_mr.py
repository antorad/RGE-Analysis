import ROOT
import uproot as ur
import matplotlib.pyplot as plt
import optparse
import numpy as np
import awkward as ak
import mplhep as hep
import os
from include import Save_Figure, Add_Clas_Pleliminary, clas_preliminary_flag, var_unit, var_range, var_label, data_directory, output_directory, pid_scheme

plt.style.use(hep.style.ATLAS)  # or ATLAS/LHCb2

# Construct the argument parser
parser = optparse.OptionParser("usage: %prog [options]\n")
parser.add_option('-v', '--variable', dest='variable',
                  default="z_h", help="Variable you want to Plot")
parser.add_option('-p', '--pid', dest='pid',
                  default="211", help="Hadron pid")
options, args = parser.parse_args()

var = options.variable
hadron_pid = options.pid

nbins = 100
shift = 0.0  # shift in the x axes for better readability

run_number_list = ["C", "Al", "Cu", "Sn", "Pb"]
# run_number_list = ["020046", "020232"]

tarList = ["C", "Al", "Cu", "Sn", "Pb"]
colorList = ["red", "purple", "blue", "green", "black"]
markerList = ["o", "o", "o", "o", "o"]
y_range = {"211":  {"z_h": [0.3, 1.4], "nu": [0.6, 1.2], "p_t2": [0, 5], },
           "-211": {"z_h": [0.3, 1.4], "nu": [0.6, 1.2], "p_t2": [0, 5], },
           "2212": {"z_h": [0.3, 5.4], "nu": [0.7, 3.6], "p_t2": [0, 5], },
           "321":  {"z_h": [0.3, 4.4], "nu": [0.7, 2.6], "p_t2": [0, 5], },
           }
# tarList = ["Pb - First round", "Pb - Second round" ]
# markerList = ["<", ">"]
# colorList = ["blue", "black"]

binsLabel = {"Q2": ["1.00 - 1.32", "1.32 - 1.74", "1.74 - 4.00"],
             "nu": ["2.20 - 3.36", "3.36 - 3.82", "3.82 - 4.26"],
             "z_h": ["0.0 - 0.1", "0.1 - 0.2", "0.2 - 0.3", "0.3 - 0.4", "0.4 - 0.5",
                     "0.5 - 0.6", "0.6 - 0.7", "0.7 - 0.8",  "0.8 - 0.9", "0.9 - 1.0"],
             "FullIntegrated": "Target"
             }


def Plot_Mr():

    fig, axs = plt.subplots(1, 1, constrained_layout=True)
    width = 6
    height = width / 1.2
    fig.set_size_inches(width, height)

    if clas_preliminary_flag:
        Add_Clas_Pleliminary(axs)

    for i in range(len(run_number_list)):  # Loops on the diffrent targets

        axs.set_xlim(var_range[var][0], var_range[var][1])
        axs.set_ylim(y_range[hadron_pid][var][0], y_range[hadron_pid][var][1])

        # Read TGraph information
        file = ROOT.TFile.Open(
            output_directory + run_number_list[i] + "/out_clas12_" + hadron_pid + "_" + run_number_list[i] + ".root", "READ")

        graphName = "multiplicity_ratio_" + var
        graph = file.Get(graphName)
        nPoints = graph.GetN()
        x = np.ndarray(nPoints, dtype=float, buffer=graph.GetX())
        x = x + (-shift + shift*2*i)
        y = np.ndarray(nPoints, dtype=float, buffer=graph.GetY())
        ey = np.ndarray(nPoints, dtype=float, buffer=graph.GetEY())

        # ---- Print TGraph information - ----
        # print("Target :" + run_number_list[i])
        # print("pid:" + pid_scheme[hadron_pid])
        # print("Mr : ")
        # print(np.round(y, 2))
        # print("Error: ")
        # print(np.round(ey, 2))

        # Plot MR
        axs.errorbar(x, y, ey, marker=markerList[i], linestyle="",
                     markerfacecolor=colorList[i], color=colorList[i],
                     markersize=4.5, label=tarList[i])

        file.Close()

    axs.set_ylabel(
        r'$R$', loc="center", fontsize=15)

    axs.set_xlabel(var_label[var], loc="center", fontsize=14)
    axs.annotate(
        r'Hadron: ' + pid_scheme[hadron_pid], xy=(0.55, 0.92), xycoords="axes fraction", weight="bold")

    if var == "z_h":
        axs.legend(ncol=1, frameon=False, loc='upper right', fontsize=11)
    elif var == "nu":
        axs.legend(ncol=1, frameon=False, loc='upper left', fontsize=11)

    axs.grid(visible=None, axis='both', color='0.95')
    axs.set_axisbelow(True)

    Save_Figure(fig, output_directory + "Results/Mr/", "Multiplicity_Ratio_" +
                pid_scheme[hadron_pid] + "_" + var)


def Print_Mr_Table():

    files = [0, 0, 0, 0, 0]
    x = [0, 0, 0, 0, 0]
    y = [0, 0, 0, 0, 0]
    ey = [0, 0, 0, 0, 0]
    decimals = 2

    graphName = "multiplicity_ratio_" + var

    for i in range(len(run_number_list)):
        file = ROOT.TFile.Open(
            output_directory + run_number_list[i] + "/out_clas12_" + hadron_pid + "_" + run_number_list[i] + ".root", "READ")
        graph = file.Get(graphName)
        nPoints = graph.GetN()
        x[i] = np.ndarray(nPoints, dtype=float, buffer=graph.GetX())
        y[i] = np.ndarray(nPoints, dtype=float, buffer=graph.GetY())
        ey[i] = np.ndarray(nPoints, dtype=float, buffer=graph.GetEY())
        file.Close()
    print("Table for : " + hadron_pid)
    print("")
    print("               & C & C & Al & Al & Cu & Cu & Sn & Sn & Pb & Pb \\\\ \hline")
    print("    $Z_h$ & value & error & value & error & value & error & value & error & value & error \\\\ \hline")
    for i in range(len(binsLabel[var])):
        print("     ", end="")
        print(binsLabel[var][i] + " & ", end="")
        print(round(y[0][i], decimals), end="")
        print(" & ", end="")
        print(round(ey[0][i], decimals), end="")
        print(" & ", end="")
        print(round(y[1][i], decimals), end="")
        print(" & ", end="")
        print(round(ey[1][i], decimals), end="")
        print(" & ", end="")
        print(round(y[2][i], decimals), end="")
        print(" & ", end="")
        print(round(ey[2][i], decimals), end="")
        print(" & ", end="")
        print(round(y[3][i], decimals), end="")
        print(" & ", end="")
        print(round(ey[3][i], decimals), end="")
        print(" & ", end="")
        print(round(y[4][i], decimals), end="")
        print(" & ", end="")
        print(round(ey[4][i], decimals), end="")
        print("\\\\ \hline")


Plot_Mr()
# Print_Mr_Table()
