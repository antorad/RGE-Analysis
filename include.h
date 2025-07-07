//Cuts for the plots
TCut Beta_cut="(beta>0)&&(beta<1.2)";
TCut P_cut="(p>0)&&(p<12)";
// DIS cuts
TCut DIS_cut="(Q2>1)&&(sqrt(W2)>2)&&(y_bjorken<0.85)";
//Total cut
TCut Main_cut=Beta_cut&&P_cut&&DIS_cut;

//Binning (Check values later, these are testing for now)
const int N_Q2=5;
const int N_Nu=4;
const int N_Zh=10;
const int N_Pt2=5;
const int N_Phi=12; //Phi_PQ

float Q2_bins[N_Q2+1]={1.,2.6, 4.2, 5.8, 7.4, 9.};
float Nu_bins[N_Nu+1]={2., 3.75, 5.5, 7.25, 9.};
float Zh_bins[N_Zh+1]={0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.};
float Pt2_bins[N_Pt2+1]={0., 1., 2., 3., 4., 5.};
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

//Vertex_cuts
// Liquid data
float D2_mean_data = -6.19907;
float D2_sigma_data = 1.03201;
float D2_vz_min_data = -8.26308;
float D2_vz_max_data = -4.13506;

// Solid data
float C_mean_data = -1.22086;
float C_sigma_data = 0.622423;
float C_vz_min_data = -2.4657;
float C_vz_max_data = 0.0239888;

// Liquid simul
float D2_mean_simul = -6.45583;
float D2_sigma_simul = 0.840164;
float D2_vz_min_simul = -8.13616;
float D2_vz_max_simul = -4.7755;

// Solid simul
float C_mean_simul = -1.47382;
float C_sigma_simul = 0.513284;
float C_vz_min_simul = -2.50038;
float C_vz_max_simul = -0.447247;

//data
TCut vz_d2_data=Form("(v_z>%f)&&(v_z<%f)",D2_vz_min_data, D2_vz_max_data);
TCut vz_solid_data=Form("(v_z>%f)&&(v_z<%f)",C_vz_min_data, C_vz_max_data);
TCut vz_d2_h_data=Form("(v_z_elec>%f)&&(v_z_elec<%f)",D2_vz_min_data, D2_vz_max_data);
TCut vz_solid_h_data=Form("(v_z_elec>%f)&&(v_z_elec<%f)",C_vz_min_data, C_vz_max_data);
//simul
TCut vz_d2_acc=Form("(v_z>%f)&&(v_z<%f)",D2_vz_min_simul, D2_vz_max_simul);
TCut vz_solid_acc=Form("(v_z>%f)&&(v_z<%f)",C_vz_min_simul, C_vz_max_simul);
TCut vz_d2_h_acc=Form("(v_z_elec>%f)&&(v_z_elec<%f)",D2_vz_min_simul, D2_vz_max_simul);
TCut vz_solid_h_acc=Form("(v_z_elec>%f)&&(v_z_elec<%f)",C_vz_min_simul, C_vz_max_simul);