import matplotlib.pyplot as plt
import os

# Functions


def Save_Figure(fig, output_directory, name):

    # Create the directory if doesn't exist
    os.makedirs(output_directory, exist_ok=True)
    fig.savefig(output_directory + name + ".pdf", bbox_inches='tight')
    fig.savefig(output_directory + name + ".png", bbox_inches='tight', dpi=300)
    print(output_directory + name + " Has been created")


def Add_Clas_Pleliminary(ax):
    ax.text(0.5, 0.5, 'CLAS Preliminary',
            horizontalalignment='center',
            verticalalignment='center',
            transform=ax.transAxes,
            color="lightgrey",
            fontsize="xx-large",
            fontweight="bold",
            alpha=0.7,
            zorder=1
            )


# Directories
data_directory = "data/"
output_directory = "output/"

# Flags
clas_preliminary_flag = False

# Variables information
var_unit = {"Q2": "$GeV^2$",
            "nu": "$GeV$",
            "z_h": "",
            "v_z": "cm",
            "phi": "(rad)",
            "p_t2": "$GeV^2$",
            # "phi": "(def)", # Deg
            "charge": "C",
            "x_bjorken": "",
            "sector": ""}

var_label = {"Q2":  r'$Q^2$ [GeV^2]',
             "nu":  r'$\nu$ [GeV]',
             "z_h": r'$Z_h$',
             "v_x": r'$v_x$ [cm]',
             "v_y": r'$v_x$ [cm]',
             "v_z": r'$v_z$ [cm]',
             "phi": r'$\phi$ [rad]',
             "p_t2": r'$P_t^2$ [GeV$^2$]',
             # "phi": "(def)", # Deg
             "charge": r'Charge [C]',
             "x_bjorken": r"X_b",
             "sector": r"Sector"}

var_range = {"Q2": [0, 7],
             "nu": [1, 10],
             "z_h": [0, 1],
             "v_x": [-4, 4],
             "v_y": [-4, 4],
             "v_z": [-20, 20],
             "phi": [-4.1415, 4.1415],
             "p_t2": [0, 1.5],
             # "phi": [-190, 190], # Deg
             "charge": [-2., 2],
             "x_bjorken": [0.02, 0.7],
             "sector": [0, 7]}

# PDG pid scheme
pid_scheme = {"211": "pi+",
              "-211": "pi-",
              "2212": "proton",
              "321": "Kaon+"
              }

# Style
func_cut_style = {'color': 'orange', 'linestyle': '--', 'linewidth': 1.6}
