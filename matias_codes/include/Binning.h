#include "TString.h"

const TString dataDirectory =
    "/home/antonio/rge_analysis/RGE-Preliminary-Plots/output/";
const TString inputDirectory =
    "/home/antonio/rge_analysis/RGE-Preliminary-Plots/output/";
const TString outputDirectory =
    "/home/antonio/rge_analysis/RGE-Preliminary-Plots/output/";

// const TString inputDirectory =
//"/home/matias/proyecto/TwoPionAnalysis/Data/AnalysisNote/Systematic/LimitHigh/";
// const TString outputDirectory =
//"/home/matias/proyecto/TwoPionAnalysis/Data/AnalysisNote/Systematic/LimitHigh/";
const TString sysDirectory =
    "/home/matias/proyecto/TwoPionAnalysis/Data/AnalysisNote/Systematic/";

const int N_Pt2_Corr = 10;
const int N_Pt2 = 10;
const int N_Phi_Corr = 6;
const int N_Phi = 6;
float Pt2_BINS[N_Pt2 + 1];
float Phi_BINS[N_Phi + 1];

const int UseCutOff = 3; // Select 1 Use the Cutoff and the interpolation
                         // Select 2 Use the Cutoff
                         // Select 3 Dont use any processing
                         //
// Number of  bins
const int N_Q2 = 5;
const int N_Nu = 5;
const int N_Zh = 8;

// Bin of Zh to take in count in the integration
const int ZH_SUM = 1;

const float Delta_Pt2 = 3.0 / N_Pt2;
const float Delta_Phi = 6.28 / N_Phi;
const float Delta_Pt2_Corr = 3.0 / N_Pt2_Corr;
const float Delta_Phi_Corr = 6.28 / N_Phi_Corr;

// Number of solids targets an qd max number of pion
// const int N_PION = 3;
const int N_PION = 1;
const int N_STARGETS = 5;
const int N_VARS = 5;

// Limits
const float Q2_MIN = 1.0;
const float Q2_MAX = 7.5;
const float Nu_MIN = 2.2;
const float Nu_MAX = 9.00;
const float Zh_MIN = 0.0;
const float Zh_MAX = 1.0;
const float Pt2_MIN = 0.0;
const float Pt2_MAX = 3.0;
const float Phi_MIN = -3.14;
const float Phi_MAX = 3.14;

// B I N N I N G

// Stlip one pion events equaly
// const float Q2_BINS[N_Q2+1] = {1, 1.3, 1.8, 4.0};
// const float Nu_BINS[N_Nu+1] = {2.2, 3.2, 3.7, 4.26};

// Stlip Two pion events equaly
const float Q2_BINS[N_Q2 + 1] = {1, 1.65, 2., 2.45, 3.25, 7.9};
const float Nu_BINS[N_Nu + 1] = {2.2, 4.44, 5.7, 7.0, 8.20, 9.};

// Stlip Three pion events equaly
// const float Q2_BINS[N_Q2+1] = {1, 1.29, 1.68, 4.0};
// const float Nu_BINS[N_Nu+1] = {2.2, 3.59, 3.96, 4.26};

const float Zh_BINS[N_Zh + 1] = {0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.8, 1.0};

// const float Phi_BINS[N_Phi+1] = {-180, -90, 0, 90, 180};
// const float Phi_BINS[N_Phi+1] = {-180, -120, -60, 0, 60, 120, 180};
// const float Phi_BINS[N_Phi+1] = {-180, -150, -120, -90, -60, -30, 0, 30, 60,
// 90, 120, 150, 180};

// 70 bins Pt2
// const float Pt2_BINS[N_Pt2+1] = {0, 0.0428571, 0.0857143, 0.128571, 0.171429,
// 0.214286, 0.257143, 0.3, 0.342857, 0.385714, 0.428571, 0.471429, 0.514286,
// 0.557143, 0.6, 0.642857, 0.685714, 0.728571, 0.771429, 0.814286,
// 0.857143, 0.9, 0.942857, 0.985714, 1.02857, 1.07143, 1.11429,
// 1.15714, 1.2, 1.24286, 1.28571, 1.32857, 1.37143, 1.41429,
// 1.45714, 1.5, 1.54286, 1.58571, 1.62857, 1.67143, 1.71429,
// 1.75714, 1.8, 1.84286, 1.88571, 1.92857, 1.97143, 2.01429,
// 2.05714, 2.1, 2.14286, 2.18571, 2.22857, 2.27143, 2.31429,
// 2.35714, 2.4, 2.44286, 2.48571, 2.52857, 2.57143, 2.61429,
// 2.65714, 2.7, 2.74286, 2.78571, 2.82857, 2.87143, 2.91429,
// 2.95714, 3};

// 90 bins Pt2
// const float Pt2_BINS[N_Pt2+1] = {0, 0.0333333, 0.0666667, 0.1, 0.133333,
// 0.166667, 0.2, 0.233333, 0.266667, 0.3, 0.333333, 0.366667, 0.4, 0.433333,
// 0.466667, 0.5, 0.533333, 0.566667, 0.6, 0.633333, 0.666667, 0.7, 0.733333,
// 0.766667, 0.8, 0.833333, 0.866667, 0.9, 0.933333, 0.966667, 1,
// 1.03333, 1.06667, 1.1, 1.13333, 1.16667, 1.2, 1.23333, 1.26667,
// 1.3, 1.33333, 1.36667, 1.4, 1.43333, 1.46667, 1.5, 1.53333,
// 1.56667, 1.6, 1.63333, 1.66667, 1.7, 1.73333, 1.76667, 1.8,
// 1.83333, 1.86667, 1.9, 1.93333, 1.96667, 2, 2.03333, 2.06667, 2.1,
// 2.13333, 2.16667, 2.2, 2.23333, 2.26667, 2.3, 2.33333, 2.36667,
// 2.4, 2.43333, 2.46667, 2.5, 2.53333, 2.56667, 2.6, 2.63333,
// 2.66667, 2.7, 2.73333, 2.76667, 2.8, 2.83333, 2.86667, 2.9,
// 2.93333, 2.96667, 3};

// 110 bins Pt2
// const float Pt2_BINS[N_Pt2+1] = {0, 0.0272727, 0.0545455, 0.0818182,
// 0.109091, 0.136364, 0.163636, 0.190909, 0.218182, 0.245455, 0.272727, 0.3,
// 0.327273, 0.354545, 0.381818, 0.409091, 0.436364, 0.463636, 0.490909,
// 0.518182, 0.545455, 0.572727, 0.6, 0.627273, 0.654545, 0.681818, 0.709091,
// 0.736364, 0.763636, 0.790909, 0.818182, 0.845455, 0.872727,
// 0.9, 0.927273, 0.954545, 0.981818, 1.00909, 1.03636, 1.06364,
// 1.09091, 1.11818, 1.14545, 1.17273, 1.2, 1.22727, 1.25455,
// 1.28182, 1.30909, 1.33636, 1.36364, 1.39091, 1.41818, 1.44545,
// 1.47273, 1.5, 1.52727, 1.55455, 1.58182, 1.60909, 1.63636,
// 1.66364, 1.69091, 1.71818, 1.74545, 1.77273, 1.8, 1.82727,
// 1.85455, 1.88182, 1.90909, 1.93636, 1.96364, 1.99091, 2.01818,
// 2.04545, 2.07273, 2.1, 2.12727, 2.15455, 2.18182, 2.20909,
// 2.23636, 2.26364, 2.29091, 2.31818, 2.34545, 2.37273, 2.4,
// 2.42727, 2.45455, 2.48182, 2.50909, 2.53636, 2.56364, 2.59091,
// 2.61818, 2.64545, 2.67273, 2.7, 2.72727, 2.75455, 2.78182,
// 2.80909, 2.83636, 2.86364, 2.89091, 2.91818, 2.94545, 2.97273,
// 3};

const char *Targets[2 * N_STARGETS] = {"C",  "Al",  "Cu",  "Sn",  "Pb",
                                       "DC", "DAl", "DCu", "DSn", "DPb"};
const char *SolTargets[N_STARGETS] = {"C", "Al", "Cu", "Sn", "Pb"};
const char *LiqTargets[N_STARGETS] = {"DC", "DAl", "DCu", "DSn", "DPb"};

char DC[3] = {'D', 'C', '\0'};
char DFe[4] = {'D', 'F', 'e', '\0'};
char DPb[4] = {'D', 'P', 'b', '\0'};
char C[2] = {'C', '\0'};
char Fe[3] = {'F', 'e', '\0'};
char Pb[3] = {'P', 'b', '\0'};
