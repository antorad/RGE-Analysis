//Cuts for the plots
TCut Beta_cut="(beta>0)&&(beta<1.2)";
TCut P_cut="(p>0)&&(p<12)";
// DIS cuts
TCut DIS_cut="(Q2>1)&&(sqrt(W2)>2)&&(y_bjorken<0.85)";
TCut elec_cut = "Q2>1&&Q2<11&&nu>2&&nu<9";
//Total cut
TCut Main_cut=Beta_cut&&P_cut&&DIS_cut;

//Verex_cuts
TCut vz_d2="(v_z>-8.01)&&(v_z<-3.62)";
TCut vz_solid="(v_z>-1.84)&&(v_z<0.09)";
TCut vz_d2_h="(v_z_elec>-8.01)&&(v_z_elec<-3.62)";
TCut vz_solid_h="(v_z_elec>-1.84)&&(v_z_elec<0.09)";

//Variables to use
TString vars[5] = {"Q2", "nu", "Zh", "Pt2", "phi_PQ"};
TString mainVar;

//Binning (Check values later, these are testing for now)
const int N_Q2=5;
const int N_Nu=4;
const int N_Zh=10;
const int N_Pt2=5;
const int N_Phi=12; //Phi_PQ
int N_main=N_Zh;

float Q2_bins[N_Q2+1]={1.,3., 5., 7., 9., 11.};
float Nu_bins[N_Nu+1]={2., 3.5, 5., 7.5, 9.};
float Zh_bins[N_Zh+1]={0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.};
float Pt2_bins[N_Pt2+1]={0., 2., 4., 6., 8., 10.};
double Phi_bins[N_Phi+1]={-3.1416, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3.1416};
float (*main_bins) = Zh_bins;

void integrate_multibinning(TString Target="C", int Hadron_pid=211){
    ROOT::EnableImplicitMT(); //To run with multithreads (idk if it works)

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

////////////////////////////////////////////////////////////////////////
//////////               INPUT AND OUTPUT FILES               //////////
////////////////////////////////////////////////////////////////////////

    //DATA
    //Get hadron histograms from root file created with create_multibinning
    TFile *input_hadron_data = new TFile("output/"+Target+"/data_binned.root","READ");
    //Get electron TNtuple input created from simple_plots
    TFile *input_elec_data = new TFile("output/"+Target+"/out_clas12.root","READ");
    TNtuple* elec_tuple_data = (TNtuple*)input_elec_data->Get("elec_tuple");

    //ACCEPTED (Data from other target for now)
    //hadrons
    TFile *input_hadron_acc = new TFile("output/C/data_binned.root","READ");
    //electrons
    TFile *input_elec_acc = new TFile("output/C/out_clas12.root","READ");
    TNtuple* elec_tuple_acc = (TNtuple*)input_elec_acc->Get("elec_tuple");

    //THROWN (Data from another target for now)
    //hadrons
    TFile *input_hadron_thr = new TFile("output/C/data_binned.root","READ");
    //electrons
    TFile *input_elec_thr = new TFile("output/C/out_clas12.root","READ");
    TNtuple* elec_tuple_thr = (TNtuple*)input_elec_thr->Get("elec_tuple");

    //OUTPUT file
    TFile *output = new TFile("output/"+Target+"/mr.root","RECREATE");

////////////////////////////////////////////////////////////////////////
//////////            HADRON ACEPTANCE CORRECTION             //////////
////////////////////////////////////////////////////////////////////////

    //Create var cuts
    TCut Q2_Cut, Nu_Cut, Zh_Cut, Pt2_Cut, total_cut;

    //Array of histograms to integrate entries from input root file
    //Uncorrected
    TH1F* Integ_liq_uncorr[10];
    TH1F* Integ_sol_uncorr[10];
    //Aceptance Corrected
    TH1F* Integ_liq_corr[10];
    TH1F* Integ_sol_corr[10];
    //TH1F needed later
    TH1F *h_liq_data, *h_liq_acc, *h_liq_thr, *h_liq_corr, *liq_correction;
    TH1F *h_sol_data, *h_sol_acc, *h_sol_thr, *h_sol_corr, *sol_correction;

    //Loop every bin for every variable (main var first)
    for (int mainVarCounter = 0; mainVarCounter < N_main; mainVarCounter++) {
        //Create integration histogram for each main bin to save in the array
        Integ_liq_uncorr[mainVarCounter] = new TH1F(Form("Integrated_histo_liq_uncorr_%i", mainVarCounter), "Integrated histo liq uncorr", N_Phi, Phi_bins[0], Phi_bins[N_Phi]);
        Integ_sol_uncorr[mainVarCounter] = new TH1F(Form("Integrated_histo_sol_uncorr_%i", mainVarCounter), "Integrated histo sol uncorr", N_Phi, Phi_bins[0], Phi_bins[N_Phi]);
        Integ_liq_corr[mainVarCounter] = new TH1F(Form("Integrated_histo_liq_corr_%i", mainVarCounter), "Integrated histo liq corr", N_Phi, Phi_bins[0], Phi_bins[N_Phi]);
        Integ_sol_corr[mainVarCounter] = new TH1F(Form("Integrated_histo_sol_corr_%i", mainVarCounter), "Integrated histo sol corr", N_Phi, Phi_bins[0], Phi_bins[N_Phi]);
        //Loop over remaining variables
        for (int Q2Counter = 0; Q2Counter < N_Q2; Q2Counter++) {
            for (int NuCounter = 0; NuCounter < N_Nu; NuCounter++) {
                for (int Pt2Counter = 0; Pt2Counter < N_Pt2; Pt2Counter++) {
                    //Obtain histos from file
                    cout<<"********************* New hadron BIN *******************"<<endl;
                    cout<<"Getting histos: "<< Form("%i_%i_%i_%i", Q2Counter, NuCounter, mainVarCounter, Pt2Counter)<<endl;
                    //Data histos
                    h_liq_data = (TH1F*)input_hadron_data->Get(Form("Data_liq_%i_%i_%i_%i", Q2Counter, NuCounter, mainVarCounter, Pt2Counter));
                    h_sol_data = (TH1F*)input_hadron_data->Get(Form("Data_sol_%i_%i_%i_%i", Q2Counter, NuCounter, mainVarCounter, Pt2Counter));
                    //Accepted histos
                    h_liq_acc = (TH1F*)input_hadron_acc->Get(Form("Data_liq_%i_%i_%i_%i", Q2Counter, NuCounter, mainVarCounter, Pt2Counter));
                    h_sol_acc = (TH1F*)input_hadron_acc->Get(Form("Data_sol_%i_%i_%i_%i", Q2Counter, NuCounter, mainVarCounter, Pt2Counter));
                    //Thrown histos
                    h_liq_thr = (TH1F*)input_hadron_thr->Get(Form("Data_liq_%i_%i_%i_%i", Q2Counter, NuCounter, mainVarCounter, Pt2Counter));
                    h_sol_thr = (TH1F*)input_hadron_thr->Get(Form("Data_sol_%i_%i_%i_%i", Q2Counter, NuCounter, mainVarCounter, Pt2Counter));

                    //acceptance correction factors calculation
                    //LIQUID TARGET
                    h_liq_corr = (TH1F*)h_liq_data->Clone();
                    liq_correction = (TH1F*)h_liq_thr->Clone();
                    liq_correction->Divide(h_liq_acc);
                    h_liq_corr->Multiply(liq_correction);

                    cout<<"adding corrected liq histo with integral:"<<h_liq_corr->Integral()<<endl;
                    Integ_liq_corr[mainVarCounter]->Add(h_liq_corr);

                    //SOLID TARGET
                    h_sol_corr = (TH1F*)h_sol_data->Clone();
                    sol_correction = (TH1F*)h_sol_thr->Clone();
                    sol_correction->Divide(h_sol_acc);
                    h_sol_corr->Multiply(sol_correction);

                    cout<<"adding corrected sol histo with integral:"<<h_sol_corr->Integral()<<endl;
                    Integ_sol_corr[mainVarCounter]->Add(h_sol_corr);

                    //*** check what to do if the histogram is empty ***
                    // maybe just add the uncorrected into the corrected 

                    //Add uncorrected histos to integrated histograms
                    Integ_liq_uncorr[mainVarCounter]->Add(h_liq_data);
                    Integ_sol_uncorr[mainVarCounter]->Add(h_sol_data);
                }
            }
        }
    }
    //Open uptput file to save root stuff in it
    output->cd();

    //Final histograms for main var histograms NON corrected
    TH1F* histo_liquid_uncorr = new TH1F(" histo liq"," histo liq", N_main, main_bins[0], main_bins[N_main]);
    TH1F* histo_solid_uncorr = new TH1F(" histo sol"," histo sol", N_main, main_bins[0], main_bins[N_main]);

    //Final histograms for main var histograms corrected
    TH1F* histo_liquid_corr = new TH1F(" histo liq corr"," histo liq corr", N_main, main_bins[0], main_bins[N_main]);
    TH1F* histo_solid_corr = new TH1F(" histo sol corr"," histo sol corr", N_main, main_bins[0], main_bins[N_main]);

    //Error propagation
    histo_liquid_uncorr->Sumw2();
    histo_solid_uncorr->Sumw2();
    histo_liquid_corr->Sumw2();
    histo_solid_corr->Sumw2();

    //Variables to save number of corrected hadrons and statistic errors
    Double_t error_liq_uncorr, error_sol_uncorr, error_sol_corr, error_liq_corr;

    //Set bin values and error using integrated histogram for each main var bin
    for (int mainVarCounter = 0; mainVarCounter < N_main; mainVarCounter++) {
        //Uncorrected
        histo_liquid_uncorr->SetBinContent(mainVarCounter+1, Integ_liq_uncorr[mainVarCounter]->IntegralAndError(1,N_main, error_liq_uncorr));
        histo_solid_uncorr->SetBinContent(mainVarCounter+1, Integ_sol_uncorr[mainVarCounter]->IntegralAndError(1,N_main, error_sol_uncorr));
        histo_liquid_uncorr->SetBinError(mainVarCounter+1, error_liq_uncorr);
        histo_solid_uncorr->SetBinError(mainVarCounter+1, error_sol_uncorr);
        Integ_liq_uncorr[mainVarCounter]->Write(Form("phiPQ_histo_liq_%i", mainVarCounter));
        Integ_sol_uncorr[mainVarCounter]->Write(Form("phiPQ_histo_sol_%i", mainVarCounter));

        //Corrected
        histo_liquid_corr->SetBinContent(mainVarCounter+1, Integ_liq_corr[mainVarCounter]->IntegralAndError(1,N_main, error_liq_corr));
        histo_solid_corr->SetBinContent(mainVarCounter+1, Integ_sol_corr[mainVarCounter]->IntegralAndError(1,N_main, error_sol_corr));
        histo_liquid_corr->SetBinError(mainVarCounter+1, error_liq_corr);
        histo_solid_corr->SetBinError(mainVarCounter+1, error_sol_corr);
        Integ_liq_corr[mainVarCounter]->Write(Form("phiPQ_histo_liq_%i_corr", mainVarCounter));
        Integ_sol_corr[mainVarCounter]->Write(Form("phiPQ_histo_sol_%i_corr", mainVarCounter));
    }

////////////////////////////////////////////////////////////////////////
//////////           ELECTRON ACEPTANCE CORRECTION            //////////
////////////////////////////////////////////////////////////////////////

    //Draw the 2D electron plots
    elec_tuple_data->Draw(Form("nu:Q2>>h_elec_sol_data(%i,%f,%f,%i,%f,%f)",N_Q2,Q2_bins[0],Q2_bins[N_Q2],N_Nu,Nu_bins[0],Nu_bins[N_Nu]), Main_cut&&vz_solid, "goff");
    elec_tuple_data->Draw(Form("nu:Q2>>h_elec_liq_data(%i,%f,%f,%i,%f,%f)",N_Q2,Q2_bins[0],Q2_bins[N_Q2],N_Nu,Nu_bins[0],Nu_bins[N_Nu]), Main_cut&&vz_d2, "goff");
    elec_tuple_acc->Draw(Form("nu:Q2>>h_elec_sol_acc(%i,%f,%f,%i,%f,%f)",N_Q2,Q2_bins[0],Q2_bins[N_Q2],N_Nu,Nu_bins[0],Nu_bins[N_Nu]), Main_cut&&vz_solid, "goff");
    elec_tuple_acc->Draw(Form("nu:Q2>>h_elec_liq_acc(%i,%f,%f,%i,%f,%f)",N_Q2,Q2_bins[0],Q2_bins[N_Q2],N_Nu,Nu_bins[0],Nu_bins[N_Nu]), Main_cut&&vz_d2, "goff");
    elec_tuple_thr->Draw(Form("nu:Q2>>h_elec_sol_thr(%i,%f,%f,%i,%f,%f)",N_Q2,Q2_bins[0],Q2_bins[N_Q2],N_Nu,Nu_bins[0],Nu_bins[N_Nu]), Main_cut&&vz_solid, "goff");
    elec_tuple_thr->Draw(Form("nu:Q2>>h_elec_liq_thr(%i,%f,%f,%i,%f,%f)",N_Q2,Q2_bins[0],Q2_bins[N_Q2],N_Nu,Nu_bins[0],Nu_bins[N_Nu]), Main_cut&&vz_d2, "goff");

    //Save the plots into TH2F to access them later
    TH2F *h_elec_sol_data = (TH2F*)gDirectory->Get("h_elec_sol_data");
    TH2F *h_elec_liq_data = (TH2F*)gDirectory->Get("h_elec_liq_data");
    TH2F *h_elec_sol_acc = (TH2F*)gDirectory->Get("h_elec_sol_acc");
    TH2F *h_elec_liq_acc = (TH2F*)gDirectory->Get("h_elec_liq_acc");
    TH2F *h_elec_sol_thr = (TH2F*)gDirectory->Get("h_elec_sol_thr");
    TH2F *h_elec_liq_thr = (TH2F*)gDirectory->Get("h_elec_liq_thr");

    //TH2 with corrected number of electrons
    TH2F *h_elec_sol_corr = (TH2F*)h_elec_sol_data->Clone();
    TH2F *h_elec_liq_corr = (TH2F*)h_elec_liq_data->Clone();
    h_elec_sol_corr->Multiply(h_elec_sol_thr);
    h_elec_sol_corr->Divide(h_elec_sol_acc);
    h_elec_liq_corr->Multiply(h_elec_liq_thr);
    h_elec_liq_corr->Divide(h_elec_liq_acc);

    //Count number of corrected electrons and statistic errors
    Double_t n_elec_sol_corr, n_elec_liq_corr, n_elec_error_liq, n_elec_error_sol;
    n_elec_sol_corr = h_elec_sol_corr->IntegralAndError(1, N_Q2,1, N_Nu, n_elec_error_sol);
    n_elec_liq_corr = h_elec_liq_corr->IntegralAndError(1, N_Q2,1, N_Nu, n_elec_error_liq);

    //Convert number of electron into flat histogram with errors.
    TH1F* elec_hist_liq_corr = new TH1F("elec_hist_liq_corr", "", N_main, main_bins[0], main_bins[N_main]);
    TH1F* elec_hist_sol_corr = new TH1F("elec_hist_sol_corr", "", N_main, main_bins[0], main_bins[N_main]);
    for (int i = 1; i <= N_main; i++) {
        elec_hist_sol_corr->SetBinContent(i, n_elec_sol_corr);
        elec_hist_liq_corr->SetBinContent(i, n_elec_liq_corr);
        elec_hist_sol_corr->SetBinError(i, n_elec_error_sol);
        elec_hist_liq_corr->SetBinError(i, n_elec_error_liq);
    }
    //Save TH2 electron plots into file
    h_elec_sol_data->Write("elec_sol_data");
    h_elec_sol_acc->Write("elec_sol_acc");
    h_elec_sol_thr->Write("elec_sol_thr");
    h_elec_sol_corr->Write("elec_sol_corr");
    h_elec_liq_data->Write("elec_liq_data");
    h_elec_liq_acc->Write("elec_liq_acc");
    h_elec_liq_thr->Write("elec_liq_thr");
    h_elec_liq_corr->Write("elec_liq_corr");

    //Uncorrected number of electrons
    //Counting of the number of electron in each target by making an hist and counting entries
    float n_e_liq = elec_tuple_data->Draw("Q2>>h_e_liq", elec_cut&&Main_cut&&vz_d2, "goff");
    float n_e_sol = elec_tuple_data->Draw("Q2>>h_e_sol", elec_cut&&Main_cut&&vz_solid, "goff");

    float ratio= n_e_liq/n_e_sol;
    //Convert number of electron into flat histogram.
    TH1F* elec_hist_liq_uncorr = new TH1F("elec_hist_liq_uncorr", "", N_main, main_bins[0], main_bins[N_main]);
    TH1F* elec_hist_sol_uncorr = new TH1F("elec_hist_sol_uncorr", "", N_main, main_bins[0], main_bins[N_main]);
        for (int i = 1; i <= N_main; i++) {
        elec_hist_liq_uncorr->SetBinContent(i, n_e_liq);
        elec_hist_sol_uncorr->SetBinContent(i, n_e_sol);
    }

    //print electron info
    cout<<"*******************************************************************************"<<endl;
    cout<<"UNCORRECTED-> ne_sol:"<<n_e_sol<<", ne_liq:"<<n_e_liq<<", ratio:"<<n_e_sol/n_e_liq<<endl;
    cout<<"CORRECTED-> ne_sol_corr:"<<n_elec_sol_corr<<", ne_liq_corr:"<<n_elec_liq_corr<<", ratio:"<<n_elec_sol_corr/n_elec_liq_corr<<endl;

////////////////////////////////////////////////////////////////////////
//////////          MULTIPLICITY RATIO CALCULATION            //////////
////////////////////////////////////////////////////////////////////////

    //TH1 histograms for multiplicity ratio
    TH1F* mr_uncorr = new TH1F("MR_uncorr","MR_uncorr", N_main, main_bins[0], main_bins[N_main]);
    TH1F* mr_corr = new TH1F("MR_corr","MR_corr", N_main, main_bins[0], main_bins[N_main]);
    mr_uncorr->Sumw2();
    mr_corr->Sumw2();

    //Uncorrected MR
    histo_liquid_uncorr->Divide(histo_liquid_uncorr, elec_hist_liq_uncorr);
    histo_solid_uncorr->Divide(histo_solid_uncorr, elec_hist_sol_uncorr);
    mr_uncorr->Divide(histo_solid_uncorr, histo_liquid_uncorr);
    mr_uncorr->SetMarkerStyle(21);
    mr_uncorr->Draw("COLZ");

    //Corrected MR
    mr_corr->Divide(histo_solid_corr, histo_liquid_corr);
    mr_corr->Multiply(mr_corr, elec_hist_liq_corr);
    mr_corr->Divide(mr_corr, elec_hist_sol_corr);
    mr_corr->SetMarkerStyle(21);
    mr_corr->Draw("COLZ");

    //Save plots
    //data
    histo_liquid_uncorr->Write("ratio_liquid_uncorr");
    histo_solid_uncorr->Write("ratio_solid_uncorr");
    mr_uncorr->Write("MR_uncorr");
    //corrected
    histo_liquid_corr->Write("ratio_liquid_corr");
    histo_solid_corr->Write("ratio_solid_corr");
    mr_corr->Write("MR_corr");

	output->Close();
}

/*
TODO
-->Apply the aceptance correction for the 5 binned data usinf acc and thr --> DONE 
-->In make_multibinning, do something similar for electrons but only TH2Fs  --> DONE
-->After the first correction, make the integration in PHIPQ first, then the varaible not wanted to claculate for the final result,
leaving the main var, nu, and q2.
-->Then apply the acceptance correction using electron biined data in Q2 and nu.
-->Finally inegrate nu and Q2 to have the main varaible one dimensional plot for sol and liq separately for the final ratio
-->Use real simulations
-->Determine what to do with empty bins.
*/