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

void integrate_multibinning(TString Target="C", int Hadron_pid=211){
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

    //Get histgrams from output root file
    TFile *output = new TFile("output/"+Target+"/data_binned.root","UPDATE");

    //Get electron TNtuple input created from simple_plots
    TFile *input = new TFile("output/"+Target+"/out_clas12.root","READ");
    TNtuple* elec_tuple = (TNtuple*)input->Get("elec_tuple");

    //Create var cuts
    TCut Q2_Cut, Nu_Cut, Zh_Cut, Pt2_Cut, total_cut;

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
    //Zh histograms
    TH1F* Zh_liquid = new TH1F("Zh histo liq","Zh histo liq", 10, 0, 1);
    TH1F* Zh_solid = new TH1F("Zh histo sol","Zh histo sol", 10, 0, 1);
    TH1F* mr = new TH1F("Zh MR","Zh MR", 10, 0, 1);

    //Error propagation
    Zh_liquid->Sumw2();
    Zh_solid->Sumw2();
    mr->Sumw2();
    Double_t error_liq, error_sol;

    //Check each histogram for each Zh bin
    for (int ZhCounter = 0; ZhCounter < N_Zh; ZhCounter++) {
        Zh_liquid->SetBinContent(ZhCounter+1, Integ_Zh_liq[ZhCounter]->IntegralAndError(1,10, error_liq));
        Zh_solid->SetBinContent(ZhCounter+1, Integ_Zh_sol[ZhCounter]->IntegralAndError(1,10, error_sol));
        Zh_liquid->SetBinError(ZhCounter+1, error_liq);
        Zh_solid->SetBinError(ZhCounter+1, error_sol);
        Integ_Zh_liq[ZhCounter]->Write(Form("phiPQ_histo_liq_%i", ZhCounter), TObject::kOverwrite);
        Integ_Zh_sol[ZhCounter]->Write(Form("phiPQ_histo_sol_%i", ZhCounter), TObject::kOverwrite);
    }

    //Counting of the number of electron in each target by making an hist and counting entries
    int n_e_liq = elec_tuple->Draw("v_z>>h_e_liq", Main_cut&&vz_d2, "goff");
    int n_e_sol = elec_tuple->Draw("v_z>>h_e_sol", Main_cut&&vz_solid, "goff");

    //Convert number of electron into flat histogram.
    TH1F* elec_hist_liq = new TH1F("elec_hist_liq", "", 10, 0, 1);
    TH1F* elec_hist_sol = new TH1F("elec_hist_sol", "", 10, 0, 1);
    for (int i = 1; i <= 10; i++) {
        elec_hist_liq->SetBinContent(i, n_e_liq);
        elec_hist_sol->SetBinContent(i, n_e_sol);
    }

    //Calculate MR
    Zh_liquid->Divide(Zh_liquid, elec_hist_liq);
    Zh_solid->Divide(Zh_solid, elec_hist_sol);
    mr->Divide(Zh_solid, Zh_liquid);
    mr->SetMarkerStyle(21);
    mr->Draw("COLZ");

    //Save plots
    Zh_liquid->Write("ZH_liquid", TObject::kOverwrite);
    Zh_solid->Write("ZH_solid", TObject::kOverwrite);
    mr->Write("ZH_mr", TObject::kOverwrite);

    //TODO
    //Once everything is integrated I'll have 10 phiPQ histos for each Zh bin --> DONE
    //I need to integrate each of those 10 histos and each integration number correspond to Zh bin value --> DONE
    //Add these values as estries for Zh histogram for solid and liquiud --> DONE
    //calculated MR --> DONE
    //Include electron info 
    //Include error --> DONE
    //compare with previus result
	output->Close();
}