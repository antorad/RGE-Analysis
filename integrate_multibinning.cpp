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
TString vars[5] = {"Q2", "Nu", "Zh", "Pt2", "phi_PQ"};
TString mainVar;

//Binning (Check values later, these are testing for now)
const int N_Q2=5;
const int N_Nu=4;
const int N_Zh=10;
const int N_Pt2=5;
const int N_Phi=12;
int N_main=N_Zh;

float Q2_bins[N_Q2+1]={1.,3., 5., 7., 9., 11.};
float Nu_bins[N_Nu+1]={2., 3.5, 5., 7.5, 9.};
float Zh_bins[N_Zh+1]={0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.};
float Pt2_bins[N_Pt2+1]={0., 2., 4., 6., 8., 10.};
float (*main_bins) = Zh_bins;

void integrate_multibinning(TString Target="C", int Hadron_pid=211){
    ROOT::EnableImplicitMT();

    //main variable. CHANGE IT LATER TO A LOOP FOR EACH VARIABLE
    mainVar = "Zh";
    N_main = N_Zh;

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

    //Creating histograms to integrate entries
    TH1F* Integ_liq[10];
    TH1F* Integ_sol[10];
    //Loop every bien for every variable (Zh first)
    for (int mainVarCounter = 0; mainVarCounter < N_main; mainVarCounter++) {
        //Create integration for each main bin
        Integ_liq[mainVarCounter] = new TH1F(Form("Integrated_histo_liq_%i", mainVarCounter), "Integrated histo liq", 12, -3.1416, 3.1416);
        Integ_sol[mainVarCounter] = new TH1F(Form("Integrated_histo_sol_%i", mainVarCounter), "Integrated histo sol", 12, -3.1416, 3.1416);
        //Loop over remaining variables
        for (int Q2Counter = 0; Q2Counter < N_Q2; Q2Counter++) {
            for (int NuCounter = 0; NuCounter < N_Nu; NuCounter++) {
                for (int Pt2Counter = 0; Pt2Counter < N_Pt2; Pt2Counter++) {
                    //Obtain histos from file
                    cout<<"Getting histo: "<< Form("Data_liq_%i_%i_%i_%i", Q2Counter, NuCounter, mainVarCounter, Pt2Counter)<<endl;
                    TH1F* h_liq = (TH1F*)output->Get(Form("Data_liq_%i_%i_%i_%i", Q2Counter, NuCounter, mainVarCounter, Pt2Counter));
                    TH1F* h_sol = (TH1F*)output->Get(Form("Data_sol_%i_%i_%i_%i", Q2Counter, NuCounter, mainVarCounter, Pt2Counter));

                    //Add histos to integrated histograms
                    Integ_liq[mainVarCounter]->Add(h_liq);
                    Integ_sol[mainVarCounter]->Add(h_sol);
                }
            }
        }
    }
    output->cd();
    //ADD VAR NAMES TO TITLES USING Form()
    //Main var histograms
    TH1F* histo_liquid = new TH1F(" histo liq"," histo liq", N_main, main_bins[0], main_bins[N_main]);
    TH1F* histo_solid = new TH1F(" histo sol"," histo sol", N_main, main_bins[0], main_bins[N_main]);
    TH1F* mr = new TH1F("MR","MR", N_main, main_bins[0], main_bins[N_main]);

    //Error propagation
    histo_liquid->Sumw2();
    histo_solid->Sumw2();
    mr->Sumw2();
    Double_t error_liq, error_sol;

    //Set bin values and error using integrated histogram for each main var bin
    for (int mainVarCounter = 0; mainVarCounter < N_main; mainVarCounter++) {
        histo_liquid->SetBinContent(mainVarCounter+1, Integ_liq[mainVarCounter]->IntegralAndError(1,10, error_liq));
        histo_solid->SetBinContent(mainVarCounter+1, Integ_sol[mainVarCounter]->IntegralAndError(1,10, error_sol));
        histo_liquid->SetBinError(mainVarCounter+1, error_liq);
        histo_solid->SetBinError(mainVarCounter+1, error_sol);
        Integ_liq[mainVarCounter]->Write(Form("phiPQ_histo_liq_%i", mainVarCounter), TObject::kOverwrite);
        Integ_sol[mainVarCounter]->Write(Form("phiPQ_histo_sol_%i", mainVarCounter), TObject::kOverwrite);
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
    histo_liquid->Divide(histo_liquid, elec_hist_liq);
    histo_solid->Divide(histo_solid, elec_hist_sol);
    mr->Divide(histo_solid, histo_liquid);
    mr->SetMarkerStyle(21);
    mr->Draw("COLZ");

    //Save plots
    histo_liquid->Write("ratio_liquid", TObject::kOverwrite);
    histo_solid->Write("ratio_solid", TObject::kOverwrite);
    mr->Write("MR", TObject::kOverwrite);

    //TODO
    //Once everything is integrated I'll have 10 phiPQ histos for each Zh bin --> DONE
    //I need to integrate each of those 10 histos and each integration number correspond to Zh bin value --> DONE
    //Add these values as estries for Zh histogram for solid and liquiud --> DONE
    //calculated MR --> DONE
    //Include electron info 
    //Include error --> DONE
    //compare with previus result

    //FOR AccpeCorr, Choose if make correction numbers first to a correction output and the apply to data
    //or do everything in the same scritp for data, acc and gen and divide histograms in situ.

	output->Close();
}