//Variables to modify before running
TString run_location = "own_pc";    //LOCATION ("own_pc" or "farm")
TString torus_pol = "inb";          //TORUS ("inb" or "out")
TString fwd_rec ="dc_fmt";          //BANKS ("dc" or "dc_fmt")
TString detector ="fd";             //DETECTORS ("fd", "cd" or "all")

//Binning (Check values later, these are testing for now)
const int N_Q2=5;
const int N_Nu=5;
const int N_Zh=10;
const int N_Pt2=5;
const int N_Phi=12; //Phi_PQ

float Q2_bins[N_Q2+1]={1.,2.6, 4.2, 5.8, 7.4, 9.};
float Nu_bins[N_Nu+1]={2.5, 3.8, 5.1, 6.4, 7.7, 9.0};
float Zh_bins[N_Zh+1]={0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.};
float Pt2_1d_bins[N_Pt2+1]={0.0, 0.6, 1.2, 1.8, 2.4, 3.0};
float Pt2_bins[N_Pt2+1]={0.0, 0.3, 0.6, 0.9, 1.2, 1.5};
float Phi_bins[N_Phi+1]={-3.1416, -2.618, -2.0944, -1.5708, -1.0472, -0.5236,
                            0, 0.5236, 1.0472, 1.5708, 2.0944, 2.618, 3.1416};

//cuts based on vars
TCut Q2_cut=Form("Q2>%f&&Q2<%f",Q2_bins[0],Q2_bins[N_Q2]);
TCut Nu_cut=Form("nu>%f&&nu<%f",Nu_bins[0],Nu_bins[N_Nu]);
TCut Zh_cut=Form("z_h>%f&&z_h<%f",Zh_bins[0],Zh_bins[N_Zh]);
TCut Pt2_cut=Form("p_T2>%f&&p_T2<%f",Pt2_bins[0],Pt2_bins[N_Pt2]);
TCut Phi_cut=Form("phi_PQ>%f&&phi_PQ<%f",Phi_bins[0],Phi_bins[N_Phi]);

TCut elec_cut=Q2_cut&&Nu_cut;
TCut Var_cut=Q2_cut&&Nu_cut&&Zh_cut&&Pt2_cut&&Phi_cut;

//electron cuts
float Q2_min = 1.0;
float W2_min = 4.0; //W>2
float y_bjorken_max = 0.85;
float p_min = 2.0;
float p_max = 8.0;
float theta_min = 5.0;

//PCAL electron fiducial cuts
float PCAL_V_fc = 9.0;
float PCAL_W_fc = 9.0;

//DC electron fiducial cuts
float DC_R1_fc_e = 4.5;
float DC_R2_fc_e = 3.5;
float DC_R3_fc_e = 7.5;

//DC pion fiducial cuts
float DC_R1_fc_pi = 1.25;
float DC_R2_fc_pi = 1.35;
float DC_R3_fc_pi = 1.94;

//Partial sampling fraction {p_bin_1, p_bin_2, p_bin_3, p_bin_4, p_bin_5, p_bin_6, p_bin_7, p_bin_8, p_bin_9}
float psf_a[6][9] ={ //[sector][bin]
    {-1.164, -1.242, -1.217, -1.314, -1.459, -1.518, -1.526, -1.542, -1.327},
    {-1.165, -1.254, -1.211, -1.265, -1.345, -1.366, -1.365, -1.277, -1.378},
    {-1.174, -1.254, -1.222, -1.315, -1.458, -1.513, -1.508, -1.485, -1.350},
    {-1.163, -1.277, -1.244, -1.341, -1.494, -1.558, -1.577, -1.601, -1.580},
    {-1.190, -1.346, -1.307, -1.317, -1.332, -1.311, -1.299, -1.236, -1.218},
    {-1.240, -1.331, -1.349, -1.442, -1.586, -1.616, -1.596, -1.467, -1.076}
};

float psf_b[6][9] ={ //[sector][bin]
    { 0.183,  0.202,  0.205,  0.213,  0.223,  0.223,  0.217,  0.209,  0.182},
    { 0.186,  0.206,  0.207,  0.211,  0.214,  0.209,  0.201,  0.183,  0.176},
    { 0.188,  0.208,  0.210,  0.218,  0.227,  0.226,  0.217,  0.205,  0.181},
    { 0.181,  0.205,  0.208,  0.218,  0.229,  0.230,  0.225,  0.217,  0.202},
    { 0.184,  0.209,  0.211,  0.213,  0.214,  0.211,  0.205,  0.190,  0.172},
    { 0.191,  0.212,  0.219,  0.228,  0.236,  0.233,  0.223,  0.200,  0.156}
};

//Samplig fraction {a_mu, b_mu, c_mu, a_sigma, b_sigma, c_sigma}
float sf_pars[6][6] = { //[sector][par]
    {0.230550, 0.004026, -0.001579, 0.011871, 0.004518, -0.000194},
    {0.236385, 0.002482, -0.001277, 0.012727, 0.003661, -0.000142},
    {0.239112, 0.003074, -0.001304, 0.012321, 0.004157, -0.000206},
    {0.236323, 0.001895, -0.001317, 0.011823, 0.004111, -0.000142},
    {0.235722, 0.002182, -0.001194, 0.010164, 0.004697, -0.000235},
    {0.236613, 0.004743, -0.001571, 0.011895, 0.003938, -0.000133}
};

//Vertex cuts by Udi {min_D2, max_D2, min_sol, max_sol}
float vertex_cut[5][6][4] = { //[target_config][sector][cut]
    {{-9.138, -4.725, -2.585, -1.124},//D2+C
     {-8.886, -4.481, -2.450, -0.818},
     {-8.667, -4.366, -2.216, -0.726},
     {-8.812, -4.396, -2.349, -0.700},
     {-8.409, -4.003, -1.996, -0.294},
     {-8.637, -4.275, -2.170, -0.587}},
    {{-9.081, -4.692, -2.559, -1.102},//D2+Al
     {-8.812, -4.496, -2.337, -0.910},
     {-8.666, -4.394, -2.208, -0.797},
     {-8.842, -4.453, -2.391, -0.788},
     {-8.397, -4.080, -1.958, -0.433},
     {-8.602, -4.290, -2.135, -0.644}},
    {{-9.128, -4.696, -2.637, -1.161},//D2+Cu
     {-8.884, -4.490, -2.487, -0.903},
     {-8.710, -4.358, -2.342, -0.743},
     {-8.836, -4.407, -2.427, -0.779},
     {-8.412, -4.008, -2.046, -0.374},
     {-8.648, -4.232, -2.268, -0.592}},
    {{-9.152, -4.673, -2.641, -1.111},//D2+Sn
     {-8.880, -4.468, -2.490, -0.860},
     {-8.736, -4.315, -2.403, -0.650},
     {-8.861, -4.411, -2.451, -0.768},
     {-8.449, -4.021, -2.069, -0.354},
     {-8.706, -4.206, -2.345, -0.499}},
    {{-9.134, -4.712, -2.621, -1.183},//D2+Pb
     {-8.904, -4.491, -2.502, -0.895},
     {-8.721, -4.360, -2.339, -0.746},
     {-8.833, -4.409, -2.397, -0.792},
     {-8.422, -3.996, -2.051, -0.346},
     {-8.662, -4.225, -2.270, -0.577}}
};

//Pion cuts parameters //{a_up, b_up, e_up, a_low, b_low, e_low,} //taken from Cu
float pion_cuts[6] = {-1.23004e-06, -0.350209, 0.401966, 0.197348, -0.428926, 0.387313};

//Pion minus cuts parameters //{a_up, b_up, e_up, a_low, b_low, e_low,} //taken from Cu
float pionm_cuts[6] = {1.56802e-05, -0.294942, 0.182439, 0.226125, -0.236775, 0.224926};

//Pion cuts parameters //{a_up, b_up, e_up, a_low, b_low, e_low,} //taken from Cu
float cd_pion_cuts[6] = {4.53685e-05, -0.329827, 0.538854, 0.338715, -0.201645, 0.821868};

//Pion minus cuts parameters //{a_up, b_up, e_up, a_low, b_low, e_low,} //taken from Cu
float cd_pionm_cuts[6] = {7.14749e-07, -0.31339, 0.456721, 0.34309, -0.193846, 0.663907};
;
////data
//TCut vz_d2_data=Form("(v_z>%f)&&(v_z<%f)",vertex_cut[][], D2_vz_max_data);
//TCut vz_solid_data=Form("(v_z>%f)&&(v_z<%f)",C_vz_min_data, C_vz_max_data);
//TCut vz_d2_h_data=Form("(v_z_elec>%f)&&(v_z_elec<%f)",D2_vz_min_data, D2_vz_max_data);
//TCut vz_solid_h_data=Form("(v_z_elec>%f)&&(v_z_elec<%f)",C_vz_min_data, C_vz_max_data);
////simul
//TCut vz_d2_acc=Form("(v_z>%f)&&(v_z<%f)",D2_vz_min_simul, D2_vz_max_simul);
//TCut vz_solid_acc=Form("(v_z>%f)&&(v_z<%f)",C_vz_min_simul, C_vz_max_simul);
//TCut vz_d2_h_acc=Form("(v_z_elec>%f)&&(v_z_elec<%f)",D2_vz_min_simul, D2_vz_max_simul);
//TCut vz_solid_h_acc=Form("(v_z_elec>%f)&&(v_z_elec<%f)",C_vz_min_simul, C_vz_max_simul);