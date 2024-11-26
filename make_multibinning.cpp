//Cuts for the plots
TCut Beta_cut="(beta>0)&&(beta<1.2)";
TCut P_cut="(p>0)&&(p<12)";

// DIS cuts
TCut DIS_cut="(Q2>1)&&(sqrt(W2)>2)&&(y_bjorken<0.85)";

//Vertex_cuts
TCut vz_d2="(v_z>-8.01)&&(v_z<-3.62)";
TCut vz_solid="(v_z>-1.84)&&(v_z<0.09)";
TCut vz_d2_h="(v_z_elec>-8.01)&&(v_z_elec<-3.62)";
TCut vz_solid_h="(v_z_elec>-1.84)&&(v_z_elec<0.09)";

//Variables to use
double Q2, Nu, Zh, Pt2, phi_PQ;

//Binning (Check values later, these are testing for now)
const int N_Q2=5;
const int N_Nu=4;
const int N_Zh=10;
const int N_Pt2=5;
const int N_Phi=12;

const float Q2_bins[N_Q2+1]={1.,3., 5., 7., 9., 11.};
const float Nu_bins[N_Nu+1]={2., 3.5, 5., 7.5, 9.};
const float Zh_bins[N_Zh+1]={0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.};
const float Pt2_bins[N_Pt2+1]={0., 2., 4., 6., 8., 10.};

void make_multibinning(TString Target="C", int Hadron_pid=211){
    ROOT::EnableImplicitMT();
    //output directory
    TString output_location = "output/"+Target+"/";

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

    //Output root file for histograms
    TFile *output = new TFile("output/"+Target+"/data_binned.root","UPDATE");

    //Get TNtuple input created from simple_plots
    TFile *input = new TFile("output/"+Target+"/out_clas12.root","READ");
    TNtuple* h_tuple = (TNtuple*)input->Get(hadron+"_ntuple");
    TNtuple* elec_tuple = (TNtuple*)input->Get("elec_tuple");

    //Create histogram to save the binning
    TH1F *hist_liq = new TH1F("data_liq", " ", N_Phi, -3.1416, 3.1416);
    TH1F *hist_sol = new TH1F("data_sol", " ", N_Phi, -3.1416, 3.1416);

    //Error propagation
    hist_liq->Sumw2();
    hist_sol->Sumw2();

    //Create var cuts
    TCut Q2_Cut, Nu_Cut, Zh_Cut, Pt2_Cut, total_cut;

    // Cycle for each bin in Q2, Nu, Zh, and Pt2
    cout <<"Starting loop "<<endl;
    output->cd();
//	for (int Q2Counter = 0; Q2Counter < N_Q2; Q2Counter++) {
//		for (int NuCounter = 0; NuCounter < N_Nu; NuCounter++) {
//			for (int ZhCounter = 0; ZhCounter < N_Zh; ZhCounter++) {
//				for (int Pt2Counter = 0; Pt2Counter < N_Pt2; Pt2Counter++) {
//
//					cout << "Working on bin: " << Q2Counter << NuCounter << ZhCounter <<Pt2Counter <<endl;
//
//					// Select the cuts for each bin
//					Q2_Cut = Form("Q2>%f&&Q2<%f", Q2_bins[Q2Counter], Q2_bins[Q2Counter + 1]);
//					Nu_Cut = Form("nu>%f&&nu<%f", Nu_bins[NuCounter], Nu_bins[NuCounter + 1]);
//					Zh_Cut = Form("z_h>%f&&z_h<%f", Zh_bins[ZhCounter], Zh_bins[ZhCounter + 1]);
//					Pt2_Cut = Form("p_T2>%f&&p_T2<%f", Pt2_bins[Pt2Counter], Pt2_bins[Pt2Counter + 1]);
//
//					//Combine cuts
//					total_cut = Q2_Cut&&Nu_Cut&&Zh_Cut&&Pt2_Cut;
//
//					//get TNtuple input created from simple_plots and apply cuts
//					h_tuple->Draw("phi_PQ>>hist_liq(12,-3.1416,3.1416)", total_cut&&vz_d2_h);
//                    h_tuple->Draw("phi_PQ>>hist_sol(12,-3.1416,3.1416)", total_cut&&vz_solid_h);
//
//                    hist_liq = (TH1F*) gDirectory->GetList()->FindObject("hist_liq");
//                    hist_sol = (TH1F*) gDirectory->GetList()->FindObject("hist_sol");
//
//					//Write histogram to output file;
//                    if (hist_sol->GetEntries() != 0){
//                        hist_sol->Write(Form("Data_sol_%i_%i_%i_%i", Q2Counter, NuCounter, ZhCounter, Pt2Counter));
//                    }
//                    if (hist_liq->GetEntries() != 0){
//                        hist_liq->Write(Form("Data_liq_%i_%i_%i_%i", Q2Counter, NuCounter, ZhCounter, Pt2Counter));
//                    }
//                    cout<<"-----------------------------------------------------------------------"<<endl;
//                    // Set the histograms values to 0
//                    hist_liq->Reset();
//                    hist_sol->Reset();
//
//				}
//			}
//		}
//	}

  //CHECKING
    //Creating histograms to integrated entries
    TH1F* Integ_Zh_liq[10];
    TH1F* Integ_Zh_sol[10];
    //Loop every bien for every variable (Zh first)
    for (int ZhCounter = 0; ZhCounter < N_Zh; ZhCounter++) {
        //Create integration for each Zh bin
        Integ_Zh_liq[ZhCounter] = new TH1F(Form("Integrated_histo_liq_%i", ZhCounter), "Integrated histo liq", 12, -3.1416, 3.1416);
        Integ_Zh_sol[ZhCounter] = new TH1F(Form("Integrated_histo_sol_%i", ZhCounter), "Integrated histo sol", 12, -3.1416, 3.1416);
        //Loop over remaining variables
        for (int Q2Counter = 0; Q2Counter < N_Q2; Q2Counter++) {
            for (int NuCounter = 0; NuCounter < N_Nu; NuCounter++) {
                for (int Pt2Counter = 0; Pt2Counter < N_Pt2; Pt2Counter++) {
                    //Obtain histos from file
                    cout<<"Getting histo: "<< Form("Data_liq_%i_%i_%i_%i", Q2Counter, NuCounter, ZhCounter, Pt2Counter)<<endl;
                    TH1F* h_liq = (TH1F*)output->Get(Form("Data_liq_%i_%i_%i_%i", Q2Counter, NuCounter, ZhCounter, Pt2Counter));
                    TH1F* h_sol = (TH1F*)output->Get(Form("Data_sol_%i_%i_%i_%i", Q2Counter, NuCounter, ZhCounter, Pt2Counter));

                    //Add histos to integrated histograms
                    Integ_Zh_liq[ZhCounter]->Add(h_liq);
                    Integ_Zh_sol[ZhCounter]->Add(h_sol);
                }
            }
        }
    }
    for (int ZhCounter = 0; ZhCounter < N_Zh; ZhCounter++) {
        Integ_Zh_liq[ZhCounter]->Write(Form("phiPQ_histo_liq_%i", ZhCounter), TObject::kOverwrite);
        Integ_Zh_sol[ZhCounter]->Write(Form("phiPQ_histo_sol_%i", ZhCounter), TObject::kOverwrite);
    }
    //TODO
    //Once everything is integrated I'll have 10 phiPQ histos for each Zh bin --> DONE
    //I need to integrate each of those 10 histos and each integration number correspond to Zh bin value
    //Add these values as estries for Zh histogram for solid and liquiud
    //calculated MR
    //compare with previus result
	output->Close();
}