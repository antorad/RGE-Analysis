#include "include.h"

void make_multibinning_v2(TString Target="C", int Hadron_pid=211, TString type="data"){
    ROOT::EnableImplicitMT();

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
    TFile *output = new TFile("output/"+subdir+"/"+Target+thrown_dir+"/data_binned_"+hadron+".root","RECREATE");

    //Get TNtuple input created from simple_plots
    TFile *input = new TFile("output/"+subdir+"/"+Target+thrown_dir+"/out_clas12.root","READ");

    TNtuple* h_tuple = (TNtuple*)input->Get(hadron+"_ntuple");
    TNtuple* elec_tuple = (TNtuple*)input->Get("elec_tuple");

    //Create histogram to save the binning
    TH2D *hist_liq = new TH2D("data_liq", " ", N_Nu, Nu_bins[0], Nu_bins[N_Nu], N_Q2, Q2_bins[0], Q2_bins[N_Q2]);
    TH2D *hist_sol = new TH2D("data_sol", " ", N_Nu, Nu_bins[0], Nu_bins[N_Nu], N_Q2, Q2_bins[0], Q2_bins[N_Q2]);

    //Error propagation
    hist_liq->Sumw2();
    hist_sol->Sumw2();

    //Create var cuts
    TCut Q2_Cut, Nu_Cut, Zh_Cut, Pt2_Cut, Phi_Cut, total_cut;
    //Overwriting the z cut in case of thrown to a true statement (ugly but effective).
    if (type=="thrown" && Target=="D2"){vz_d2_h="1>0";}
    if (type=="thrown" && Target!="D2"){vz_solid_h="1>0";}

    // Cycle for each bin in Zh, Pt2 and Phi
    cout <<"Starting loop "<<endl;
    output->cd();
	for (int ZhCounter = 0; ZhCounter < N_Zh; ZhCounter++) {
		for (int Pt2Counter = 0; Pt2Counter < N_Pt2; Pt2Counter++) {
			for (int PhiCounter = 0; PhiCounter < N_Phi; PhiCounter++) {

				cout << "Working on bin: " << ZhCounter << Pt2Counter << PhiCounter << endl;

				// Select the cuts for each bin
				Zh_Cut = Form("z_h>%f&&z_h<%f", Zh_bins[ZhCounter], Zh_bins[ZhCounter + 1]);
				Pt2_Cut = Form("p_T2>%f&&p_T2<%f", Pt2_bins[Pt2Counter], Pt2_bins[Pt2Counter + 1]);
                Phi_Cut = Form("phi_PQ>%f&&phi_PQ<%f", Phi_bins[PhiCounter], Phi_bins[PhiCounter + 1]);

				//Combine cuts
				total_cut = Main_cut&&Zh_Cut&&Pt2_Cut&&Phi_Cut;

				//get TNtuple input created from simple_plots and apply cuts
                if (type=="data" || (type != "data" && Target=="D2")){
				    h_tuple->Draw(Form("Q2:nu>>hist_liq(%i,%f,%f,%i,%f,%f)", N_Nu,Nu_bins[0],Nu_bins[N_Nu],
                                        N_Q2,Q2_bins[0],Q2_bins[N_Q2]), total_cut&&vz_d2_h, "goff");

                    //Write histo to ouytput file and then set the histo values to 0
                    hist_liq = (TH2D*) gDirectory->GetList()->FindObject("hist_liq");
                    hist_liq->Write(Form("Data_liq_%i_%i_%i", ZhCounter, Pt2Counter, PhiCounter));
                    hist_liq->Reset();
                }

                if (type=="data" || (type != "data" && Target!="D2")){
                    h_tuple->Draw(Form("Q2:nu>>hist_sol(%i,%f,%f,%i,%f,%f)", N_Nu,Nu_bins[0],Nu_bins[N_Nu],
                                        N_Q2,Q2_bins[0],Q2_bins[N_Q2]), total_cut&&vz_solid_h,"goff");

                    //Write histo to ouytput file and then set the histo values to 0
                    hist_sol = (TH2D*) gDirectory->GetList()->FindObject("hist_sol");
                    hist_sol->Write(Form("Data_sol_%i_%i_%i", ZhCounter, Pt2Counter, PhiCounter));
                    hist_sol->Reset();
                }
			}
		}
	}
	output->Close();
}