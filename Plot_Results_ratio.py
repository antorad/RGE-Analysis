import ROOT
import uproot as ur
import matplotlib.pyplot as plt
import optparse
import numpy as np
import awkward as ak
import mplhep as hep
import os
from include import Save_Figure, Add_Clas_Pleliminary, clas_preliminary_flag, var_unit, var_range, data_directory, output_directory, pid_scheme, func_cut_style

plt.style.use(hep.style.ATLAS)  # or ATLAS/LHCb2

# Construct the argument parser
parser = optparse.OptionParser("usage: %prog [options]\n")
parser.add_option('-v', '--variable', dest='variable',
                  default="z_h", help="Variable you want to Plot")
parser.add_option('-p', '--pid', dest='pid',
                  default="-211", help="Hadron pid")
options, args = parser.parse_args()

var = options.variable
hadron_pid = options.pid

shift = 0.0  # shift in the x axes for better readability

run_number_list = ["C", "Cu", "Sn", "Pb"]
# run_number_list = ["020046", "020232"]

tarList = ["C", "Cu", "Sn", "Pb"]
colorList = ["red", "Blue", "green", "black"]
markerList = ["o", "o", "o", "o"]
y_range = {"211":  {"z_h": [0.3, 1.4], "nu": [0.6, 1.2], },
           "-211": {"z_h": [0.3, 1.4], "nu": [0.6, 1.2], },
           "2212": {"z_h": [0.3, 5.4], "nu": [0.7, 3.6], },
           "321":  {"z_h": [0.3, 4.4], "nu": [0.7, 2.6], },
           }
# tarList = ["Pb - First round", "Pb - Second round" ]
# markerList = ["<", ">"]
# colorList = ["blue", "black"]

y_pana_dic = [np.array([1.03, .97, .93, .88, .88, .84, .82, .88, .76, .72]),
              np.array([1.18, .96, .84, .74, .74, .69, .65, .66, .57, .53]),
              np.array([1.24, .95, .78, .72, .65, .64, .59, .51, .53, .52]),
              np.array([1.21, .90, .72, .63, .58, .58, .59, .53, .49, .35]),
              ]

ey_pana_dic = [np.array([.01, .01, .01, .02, .02, .03, .04, .05, .06, .07]),
               np.array([.01, .01, .01, .02, .03, .04, .05, .05, .07, .08]),
               np.array([.01, .01, .01, .02, .03, .04, .04, .06, .07, .08]),
               np.array([.01, .01, .02, .02, .03, .04, .05, .06, .08, .11]),
               ]


def Plot_Mr():

    fig, axs = plt.subplots(1, 1, constrained_layout=True)
    width = 6
    height = width / 1.2
    fig.set_size_inches(width, height)

    if clas_preliminary_flag:
        Add_Clas_Pleliminary(axs)

    for i in range(len(run_number_list)):  # Loops on the diffrent targets

        axs.set_xlim(var_range[var][0], var_range[var][1])
        axs.set_ylim(.5, 1.5)

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
        bins_x = np.linspace(var_range[var][0], var_range[var][1], num=200)
        expected = np.linspace(1, 1, 200)

        y_pana = y_pana_dic[i]
        ey_pana = ey_pana_dic[i]
        r = y/y_pana
        er = (np.abs(y))*(np.sqrt(((ey*ey)/(y*y)) +
                                  ((ey_pana*ey_pana)/(y_pana*y_pana))))
        # Plot MR
        print(r)
        axs.errorbar(x, r, er, marker=markerList[i], linestyle="",
                     markerfacecolor=colorList[i], color=colorList[i],
                     markersize=4.5, label=tarList[i])
        axs.plot(bins_x, expected, **func_cut_style)

        file.Close()

    axs.set_ylabel(
        r'$R$', loc="center", fontsize=15)

    axs.set_xlabel(r'' + var + " " + var_unit[var], loc="center", fontsize=14)
    axs.annotate(
        r'Hadron: ' + pid_scheme[hadron_pid], xy=(0.55, 0.92), xycoords="axes fraction", weight="bold")

    if var == "z_h":
        axs.legend(ncol=1, frameon=False, loc='upper right', fontsize=11)
    elif var == "nu":
        axs.legend(ncol=1, frameon=False, loc='upper left', fontsize=11)

    axs.grid(visible=None, axis='both', color='0.95')
    axs.set_axisbelow(True)

    Save_Figure(fig, output_directory, "Multiplicity_Ratio_Results_Ratio_" +
                pid_scheme[hadron_pid] + "_" + var)


Plot_Mr()
