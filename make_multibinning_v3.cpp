#include "include.h"

void make_multibinning_v3(int run_N=000000, TString Target="C", int Hadron_pid=211, TString type="data"){
    ROOT::EnableImplicitMT();

    //Transform input run number to Tstring with correct number of digits
    char buffer [10];
    sprintf(buffer,"%0*d", 6, run_N);
    TString run_N_str=TString(buffer);

    //hadron selection
    TString hadron;
    if (Hadron_pid==211){hadron="pion";}
    else if (Hadron_pid==-211){hadron="pion_minus";}
    else if (Hadron_pid==2212){hadron="proton";}
    else if (Hadron_pid==321){hadron="kaon";}
    //in case a pid not valid in given, give a message and end macro
    else {
        cout<<"PID not valid"<<endl;
        return;}

    //Subdirectory name depending of type of input data
    TString subdir;
    TString thrown_dir = "";
    if (type=="data"){subdir="data";}
    if (type=="acc" || type =="thrown"){subdir="simul";}
    if (type=="thrown"){thrown_dir="/thrown";}

    //Output root file for histograms
    TFile *output = new TFile("/volatile/clas12/antorad/rge/MR_analysis/"+type+"/pass0.10/dc/"+Target+"/"+run_N_str+"/data_binned_"+hadron+".root","RECREATE");

    //Get TNtuple input created from simple_plots
    TFile *input = new TFile("/volatile/clas12/antorad/rge/MR_analysis/"+type+"/pass0.10/dc/"+Target+"/"+run_N_str+"/out_clas12.root","READ");
    TNtuple* h_tuple = (TNtuple*)input->Get(hadron+"_ntuple");

    output->cd();

    // Histogram array
    TH2F* hists_sol[N_Zh][N_Pt2][N_Phi];
    TH2F* hists_liq[N_Zh][N_Pt2][N_Phi];
    for (int i = 0; i < N_Zh; ++i) {
        for (int j = 0; j < N_Pt2; ++j) {
            for (int k = 0; k < N_Phi; ++k) {
                TString name_sol = Form("Data_sol_%i_%i_%i", i, j, k);
                TString name_liq = Form("Data_liq_%i_%i_%i", i, j, k);
                hists_sol[i][j][k] = new TH2F(name_sol, name_sol, N_Nu, Nu_bins[0], Nu_bins[N_Nu], N_Q2, Q2_bins[0], Q2_bins[N_Q2]);
                hists_liq[i][j][k] = new TH2F(name_liq, name_liq, N_Nu, Nu_bins[0], Nu_bins[N_Nu], N_Q2, Q2_bins[0], Q2_bins[N_Q2]);
            }
        }
    }

    //variables neccesaries for binning and cuts 
    Float_t Q2, nu, z_h, p_T2, phi_PQ, v_z_elec, p, y_bjorken, W2, beta;

    //------Read branches with variables needed for cuts and plots------
    h_tuple->SetBranchAddress("Q2",&Q2);
    h_tuple->SetBranchAddress("nu",&nu);
    h_tuple->SetBranchAddress("z_h",&z_h);
    h_tuple->SetBranchAddress("p_T2",&p_T2);
    h_tuple->SetBranchAddress("phi_PQ",&phi_PQ);
    h_tuple->SetBranchAddress("v_z_elec",&v_z_elec);
    h_tuple->SetBranchAddress("p",&p);
    h_tuple->SetBranchAddress("y_bjorken",&y_bjorken);
    h_tuple->SetBranchAddress("W2",&W2);
    h_tuple->SetBranchAddress("beta",&beta);

    //Ranges and step size per variable
    float Zh_range = Zh_bins[N_Zh]-Zh_bins[0];
    float Zh_step = Zh_range/N_Zh;
    float Pt2_range = Pt2_bins[N_Pt2]-Pt2_bins[0];
    float Pt2_step = Pt2_range/N_Pt2;
    float Phi_range = Phi_bins[N_Phi]-Phi_bins[0];
    float Phi_step = Phi_range/N_Phi;

    cout<<"Zh_range: "<<Zh_range<<" - Zh step: "<<Zh_step<<endl;
    cout<<"Pt2_range: "<<Pt2_range<<" - Pt2 step: "<<Pt2_step<<endl;
    cout<<"Phi_range: "<<Phi_range<<" - Phi step: "<<Phi_step<<endl;

    //Cycle the tuple once and fill histograms
    Long64_t n_entries = h_tuple->GetEntries();
    for (Long64_t i=0;i<n_entries;i++) {
        h_tuple->GetEntry(i);
        if ((beta>0)&&(beta<1.2)&&(p>0)&&(p<12)&&(Q2>1)&&(sqrt(W2)>2)&&(y_bjorken<0.85)){ //dis and checks cuts
            //dermine which bin the event belongs to
            int bin_Zh = int((z_h-Zh_bins[0])/Zh_step);
            int bin_Pt2 = int((p_T2-Pt2_bins[0])/Pt2_step);
            int bin_Phi = int((phi_PQ-Phi_bins[0])/Phi_step);

            //cuts
            bool solid_cut_data = (v_z_elec>C_vz_min_data)&&(v_z_elec<C_vz_max_data);
            bool liquid_cut_data = (v_z_elec>D2_vz_min_data)&&(v_z_elec<D2_vz_max_data);
            bool solid_cut_simul = (v_z_elec>C_vz_min_simul)&&(v_z_elec<C_vz_max_simul);
            bool liquid_cut_simul = (v_z_elec>D2_vz_min_simul)&&(v_z_elec<D2_vz_max_simul);
            bool valid_bin = (bin_Zh>=0 && bin_Zh<N_Zh && bin_Pt2>=0 && bin_Pt2<N_Pt2 && bin_Phi>=0 && bin_Phi<N_Phi);

            if (((type=="data" && liquid_cut_data)                      //check data is within vertex cut
                || (type == "acc" && Target=="D2" && liquid_cut_simul)  //or acc is within vertex cut
                || (type=="thrown" && Target=="D2"))                    // or if thrown is correct target
                && valid_bin){                                          //and the bin is within kinemetic range
                    hists_liq[bin_Zh][bin_Pt2][bin_Phi]->Fill(nu, Q2);
            }

            else if (((type=="data" && solid_cut_data)
                    || (type == "acc" && Target!="D2" && solid_cut_simul) //check data/acc is within vertex cut
                    || (type=="thrown" && Target!="D2"))                                   // or check if thrown is correctect target
                && valid_bin){                                                         //and the bin is within range
                    hists_sol[bin_Zh][bin_Pt2][bin_Phi]->Fill(nu, Q2);
            } 
        }
    }

    //Save histograms
    for (int i = 0; i < N_Zh; ++i) {
        for (int j = 0; j < N_Pt2; ++j) {
            for (int k = 0; k < N_Phi; ++k) {
                if (Target=="D2" || type=="data") {hists_liq[i][j][k]->Write();}
                if (Target!="D2") {hists_sol[i][j][k]->Write();}
            }
        }
    }

//ELECTRONS
    //Get electron tuple from file
    TNtuple* elec_tuple = (TNtuple*)input->Get("elec_tuple");
    //draw 2D Q2vsNu histogram to save on memory
    elec_tuple->Draw(Form("Q2:nu>>h2_elec_sol(%i,%f,%f,%i,%f,%f)",N_Nu,Nu_bins[0],Nu_bins[N_Nu], N_Q2,Q2_bins[0],Q2_bins[N_Q2]), Main_cut&&vz_solid_data, "goff");
    elec_tuple->Draw(Form("Q2:nu>>h2_elec_liq(%i,%f,%f,%i,%f,%f)",N_Nu,Nu_bins[0],Nu_bins[N_Nu], N_Q2,Q2_bins[0],Q2_bins[N_Q2]), Main_cut&&vz_d2_data, "goff");
    //Get 2D from memory an save them in TH2Ds
    TH2D *h2_elec_sol = (TH2D*)gDirectory->Get("h2_elec_sol");
    TH2D *h2_elec_liq = (TH2D*)gDirectory->Get("h2_elec_liq"); 
    //Write TH2D into output file
    h2_elec_sol->Write("h2_elec_sol");
    h2_elec_liq->Write("h2_elec_liq");

	output->Close();
}