import ROOT
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import FormatStrFormatter
import mplhep as hep
from include import output_directory, Save_Figure, Add_Clas_Pleliminary, var_label

plt.style.use(hep.style.ATLAS)  # or ATLAS/LHCb2

nPion = 1

yLimit = {"Q2": 0.042, "nu": 0.042, "z_h": 0.17}

xLowLimit = {"Q2": 0.8, "nu": 2.2, "z_h": 0.09}
xHighLimit = {"Q2": 7.5, "nu": 9.0, "z_h": 1.03}

NBins = {"Q2": 5, "nu": 5, "z_h": 10, "FullIntegrated": 1}

# var_label = {"Q2": r'$Q^2$[GeV^2]', "nu": r'$\nu$[GeV]', "z_h": r'$Z^{+}_h$'}

xShiftNPion = {"Q2": 0.02, "nu": 0.02, "z_h": 0.0075}
xShiftTarget = {"Q2": 0., "nu": 0., "z_h": 0.0075}


tarList = ["C", "Al", "Cu", "Sn", "Pb"]
colorList = ["red", "purple", "Blue", "green", "black"]
markerList = ["o", "s", "D"]
labelList = ["One $\pi +$", "Two $\pi+$", "Three $\pi +$"]


CLASPreliminaryFlag = False

# Function definition


def PtBroadVarTarSplit(variable):

    fig, axs = plt.subplots(1, 5, sharey='row', sharex='col')
    width = 16  # 7.056870070568701
    height = 4
    fig.set_size_inches(width, height)

    fig.subplots_adjust(left=None, bottom=None, right=None, top=None,
                        wspace=0.02, hspace=0.02)

    file = ROOT.TFile.Open(output_directory + "Pt_broad_" +
                           variable + ".root", "READ")
    yLimit = 0
    for i in range(5):  # Loops on the diffrent targets

        # axs[i].set_ylim(0, yLimit[variable])
        axs[i].set_xlim(xLowLimit[variable], xHighLimit[variable])
        # axs[i].set_xlim(xLowLimit[variable], 0.8)
        if CLASPreliminaryFlag:
            Add_Clas_Pleliminary(axs[i])

        for j in range(nPion):  # Loops on the number of pions

            # Get info from the TGraph
            graphName = "PtBroad_" + variable + "_" + tarList[i] + "_" + str(j)
            graph = file.Get(graphName)
            nPoints = graph.GetN()

            x = np.ndarray(nPoints, dtype=float, buffer=graph.GetX())
            y = np.ndarray(nPoints, dtype=float, buffer=graph.GetY())
            if variable == "z_h":
                # Shit the data for readability
                x = x + (-xShiftTarget[variable] + xShiftTarget[variable]*2*j)
                # Removing the las bin for the lack of statistics
                y[-1] = 0
            ey = np.ndarray(nPoints, dtype=float, buffer=graph.GetEY())
            # ey = np.zeros_like(ey)
            # print("Var Target: " + tarList[i]+" NPIon: " + str(j + 1))
            # print(y)
            # Plot with stat errors
            axs[i].errorbar(x, y, ey, marker="o", linestyle="",
                            markerfacecolor=colorList[j], color=colorList[j],
                            markersize=5, label=labelList[j])

        axs[i].set_xlabel(var_label[variable], fontsize=14)

    # Set the labels for the three plots

    # axs[0].set_ylim(0, yLimit)
    axs[0].set_ylabel(
        r'$\Delta P_\mathrm{T}^{2} [GeV^{2}]$', loc="center", fontsize=15)
    axs[0].annotate(r'Carbon', xy=(0.04, 1.04),
                    xycoords='axes fraction', fontsize=15)
    axs[1].annotate(r'Iron',   xy=(0.04, 1.04),
                    xycoords='axes fraction', fontsize=15)
    axs[2].annotate(r'Lead',   xy=(0.04, 1.04),
                    xycoords='axes fraction', fontsize=15)

    axs[0].legend(frameon=False, loc='upper left', fontsize=11)
    axs[0].yaxis.set_major_formatter(FormatStrFormatter('%.2f'))

    fig.align_ylabels(axs[:])
    for i in range(3):
        axs[i].grid(visible=None, axis='both', color='0.95')
        axs[i].set_axisbelow(True)

    Save_Figure(fig, output_directory + "Results/Broadening/",
                "PtBroad_" + variable + "_Target")


def PtBroadVarNPionSplit(variable):

    fig, axs = plt.subplots(1, nPion, sharey='row', sharex='col')
    width = (16/3)*nPion
    height = 4
    fig.set_size_inches(width, height)

    fig.subplots_adjust(left=None, bottom=None, right=None, top=None, wspace=0.02,
                        hspace=0.02)

    file = ROOT.TFile.Open(output_directory + "Pt_broad_" +
                           variable + ".root", "READ")
    yLimit = 0

    for i in range(5):  # Loops on the diffrent targets
        for j in range(nPion):  # Loops on the number of pions

            if CLASPreliminaryFlag:
                Add_Clas_Pleliminary(axs)

            # axs[j].set_ylim(0, yLimit[variable])
            axs.set_xlim(xLowLimit[variable], xHighLimit[variable])
            # Get info from the TGraph
            graphName = "PtBroad_" + variable + "_" + tarList[i] + "_" + str(j)
            graph = file.Get(graphName)
            nPoints = graph.GetN()
            x = np.ndarray(nPoints, dtype=float, buffer=graph.GetX())
            x = x + (-1*xShiftNPion[variable] + 1*xShiftNPion[variable]*i)
            y = np.ndarray(nPoints, dtype=float, buffer=graph.GetY())
            ey = np.ndarray(nPoints, dtype=float, buffer=graph.GetEY())

            # print("NPion Target: " + tarList[i] + " N Pion = " + str(j))
            # print(y)
            # Plot with stat errors
            axs.errorbar(x, y, ey, marker="o", linestyle="", label=tarList[i],
                         color=colorList[i], markersize=5, markerfacecolor=colorList[i])

            axs.set_xlabel(var_label[variable], fontsize=14)

    # axs[0].set_ylim(0, yLimit + 0.01)
    axs.set_ylabel(
        r'$\Delta P_\mathrm{T}^{2} [GeV^{2}]$', loc="center", fontsize=15)

    # fig.align_ylabels(axs[:])

    # axs[0].annotate(r'One Pion', xy=(0.04, 1.04),
    # xycoords='axes fraction', fontsize=15)
    # axs[1].annotate(r'Two Pion', xy=(0.04, 1.04),
    # xycoords='axes fraction', fontsize=15)

    axs.legend(frameon=False, loc='upper left', fontsize=11)
    axs.yaxis.set_major_formatter(FormatStrFormatter('%.2f'))

    for i in range(nPion):
        axs.grid(visible=None, axis='both', color='0.95')
        axs.set_axisbelow(True)

    file.Close()
    Save_Figure(fig, output_directory + "Results/Broadening/",
                "PtBroad_" + variable + "_NPion")


def PtBroadFullIntegrated(ZhCut=False):

    fig, axs = plt.subplots(1, 1, constrained_layout=True)
    # For one column
    width = 6
    height = width / 1.2
    fig.set_size_inches(width, height)

    inputName = "Pt_broad_FullIntegrated.root"
    outputName = "PtBroad_FullIntegrated"

    if ZhCut:
        inputName = "Pt_broad_FullIntegratedZhCut.root"
        outputName = "PtBroad_FullIntegratedZhCut"

    file = ROOT.TFile.Open(output_directory + inputName, "READ")

    if CLASPreliminaryFlag:
        Add_Clas_Pleliminary(axs)

    for i in range(5):  # Loops on the diffrent targets
        for j in range(nPion):  # Loops on the number of pions

            # axs.set_ylim(0, FullYlimit)
            axs.set_xlim(1.5, 6.5)

            graphName = "PtBroad_FullIntegrated_" + tarList[i] + "_" + str(j+1)
            graph = file.Get(graphName)
            nPoints = graph.GetN()
            x = np.ndarray(nPoints, dtype=float, buffer=graph.GetX())
            # x  = x + (-FullShft + FullShft*2*j)
            y = np.ndarray(nPoints, dtype=float, buffer=graph.GetY())
            ey = np.ndarray(nPoints, dtype=float, buffer=graph.GetEY())
            print(x)
            # if j == 0:
            # axs.errorbar(x, y, ey, marker=markerList[j], linestyle="",
            # markerfacecolor=colorList[i], color=colorList[i],
            # markersize=4.5, label=tarList[i])
            # if i == 2:
            # axs.errorbar(x, y, ey, marker=markerList[j], linestyle="",
            # markerfacecolor="grey", color=colorList[i], markersize=4.5,
            # label=labelList[j])

            axs.errorbar(x, y, ey, marker=markerList[j], linestyle="",
                         markerfacecolor=colorList[i], color=colorList[i], markersize=4.5,
                         label=tarList[i])

    # axs.set_ylim(0, 0.05)
    axs.set_ylabel(
        r'$\Delta P_\mathrm{T}^{2} [GeV^{2}]$', loc="center", fontsize=15)
    axs.yaxis.set_major_formatter(FormatStrFormatter('%.3f'))

    axs.set_xlabel(r'$A^\mathrm{\frac{1}{3}}$', loc="center", fontsize=14)

    if CLASPreliminaryFlag:
        Add_Clas_Pleliminary(axs)

    axs.legend(ncol=2, frameon=False, loc='upper left', fontsize=11)

    axs.grid(visible=None, axis='both', color='0.95')
    axs.set_axisbelow(True)

    Save_Figure(fig, output_directory + "Results/Broadening/", outputName)

    file.Close()


# Call funtions
PtBroadFullIntegrated()
# PtBroadFullIntegrated(True)
PtBroadVarTarSplit("Q2")
PtBroadVarTarSplit("nu")
PtBroadVarTarSplit("z_h")
PtBroadVarNPionSplit("Q2")
PtBroadVarNPionSplit("nu")
PtBroadVarNPionSplit("z_h")
