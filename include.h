//Cuts for the plots
TCut Beta_cut="(beta>0)&&(beta<1.2)";
TCut P_cut="(p>0)&&(p<12)";
// DIS cuts
TCut DIS_cut="(Q2>1)&&(sqrt(W2)>2)&&(y_bjorken<0.85)";
//Total cut
TCut Main_cut=Beta_cut&&P_cut&&DIS_cut;

//Vertex_cuts
TCut vz_d2="(v_z>-8.01)&&(v_z<-3.62)";
TCut vz_solid="(v_z>-1.84)&&(v_z<0.09)";
TCut vz_d2_h="(v_z_elec>-8.01)&&(v_z_elec<-3.62)";
TCut vz_solid_h="(v_z_elec>-1.84)&&(v_z_elec<0.09)";

//Binning (Check values later, these are testing for now)
const int N_Q2=5;
const int N_Nu=4;
const int N_Zh=10;
const int N_Pt2=5;
const int N_Phi=12; //Phi_PQ

float Q2_bins[N_Q2+1]={1.,3., 5., 7., 9., 11.};
float Nu_bins[N_Nu+1]={2., 3.5, 5., 7.5, 9.};
float Zh_bins[N_Zh+1]={0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.};
float Pt2_bins[N_Pt2+1]={0., 2., 4., 6., 8., 10.};
float Phi_bins[N_Phi+1]={-3.1416, -2.218, -2.0944, -1.5708, -1.0472, -0.5236,
                            0, 0.5236, 1.0472, 1.5708, 2.0944, 2.218, 3.1416};

//cuts based on vars
TCut Q2_cut=Form("Q2>%f&&Q2<%f",Q2_bins[0],Q2_bins[N_Q2]);
TCut Nu_cut=Form("nu>%f&&nu<%f",Nu_bins[0],Nu_bins[N_Nu]);
TCut Zh_cut=Form("z_h>%f&&z_h<%f",Zh_bins[0],Zh_bins[N_Zh]);
TCut Pt2_cut=Form("p_T2>%f&&p_T2<%f",Pt2_bins[0],Pt2_bins[N_Pt2]);
TCut Phi_cut=Form("phi_PQ>%f&&phi_PQ<%f",Phi_bins[0],Phi_bins[N_Phi]);

TCut elec_cut=Q2_cut&&Nu_cut;
TCut Var_cut=Q2_cut&&Nu_cut&&Zh_cut&&Pt2_cut&&Phi_cut;