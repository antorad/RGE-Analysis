#include "include.h"

void integrate_multibinning_v2(TString Target="C", int Hadron_pid=211, TString mainVar="Zh"){
    ROOT::EnableImplicitMT(); //To run with multithreads (idk if it works)

////////////////////////////////////////////////////////////////////////
//////////                  SETTING VARIABLES                 //////////
////////////////////////////////////////////////////////////////////////

    //Variables to use. The calculation is for the first variable in the list, called MainVar
    int N_main;
    float *main_bins = nullptr;

    //N_main and main_bins correspond to the number of bins and their limits respectively
    if (mainVar == "Zh"){
        N_main = N_Zh;
        main_bins = Zh_bins;
    }
    if (mainVar == "Pt2"){
        N_main = N_Pt2;
        main_bins = Pt2_bins;
    }
    if (mainVar == "Phi_PQ"){
        N_main = N_Phi;
        main_bins = Phi_bins;
    }
    if (mainVar == "Nu"){
        N_main = N_Nu;
        main_bins = Nu_bins;
    }
    if (mainVar == "Q2"){
        N_main = N_Q2;
        main_bins = Q2_bins;
    }

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
    TFile *input_hadron_data = new TFile("output/"+Target+"/data_binned_v2.root","READ");
    //Get electron TNtuple input created from simple_plots
    TFile *input_elec_data = new TFile("output/"+Target+"/out_clas12.root","READ");
    TNtuple* elec_tuple_data = (TNtuple*)input_elec_data->Get("elec_tuple");

    //ACCEPTED (Data from other target for now)
    //hadrons
    TFile *input_hadron_acc = new TFile("output/C/data_binned_v2.root","READ");
    //electrons
    TFile *input_elec_acc = new TFile("output/C/out_clas12.root","READ");
    TNtuple* elec_tuple_acc = (TNtuple*)input_elec_acc->Get("elec_tuple");

    //THROWN (Data from another target for now)
    //hadrons
    TFile *input_hadron_thr = new TFile("output/C/data_binned_v2.root","READ");
    //electrons
    TFile *input_elec_thr = new TFile("output/C/out_clas12.root","READ");
    TNtuple* elec_tuple_thr = (TNtuple*)input_elec_thr->Get("elec_tuple");

    //OUTPUT file
    TFile *output = new TFile("output/"+Target+"/mr_"+mainVar+"_v2.root","RECREATE");

////////////////////////////////////////////////////////////////////////
//////////            HADRON ACEPTANCE CORRECTION             //////////
////////////////////////////////////////////////////////////////////////

    //Array of histograms to integrate entries from input root file
    //Uncorrected
    TH2D* Integ_liq_uncorr[N_main];
    TH2D* Integ_sol_uncorr[N_main];
    //Aceptance Corrected
    TH2D* Integ_liq_corr[N_main];
    TH2D* Integ_sol_corr[N_main];
    //TH2D needed later
    TH2D *h_liq_data, *h_liq_acc, *h_liq_thr, *h_liq_corr, *liq_correction;
    TH2D *h_sol_data, *h_sol_acc, *h_sol_thr, *h_sol_corr, *sol_correction;

    TH2D* Integ_liq_uncorr_total = new TH2D("Integrated_histo_liq_uncorr","", N_Nu, Nu_bins[0], Nu_bins[N_Nu], N_Q2, Q2_bins[0], Q2_bins[N_Q2]);
    TH2D* Integ_sol_uncorr_total = new TH2D("Integrated_histo_sol_uncorr","", N_Nu, Nu_bins[0], Nu_bins[N_Nu], N_Q2, Q2_bins[0], Q2_bins[N_Q2]);
    TH2D* Integ_liq_corr_total = new TH2D("Integrated_histo_liq_corr","", N_Nu, Nu_bins[0], Nu_bins[N_Nu], N_Q2, Q2_bins[0], Q2_bins[N_Q2]);
    TH2D* Integ_sol_corr_total = new TH2D("Integrated_histo_sol_corr","", N_Nu, Nu_bins[0], Nu_bins[N_Nu], N_Q2, Q2_bins[0], Q2_bins[N_Q2]);

    //Loop every bin for every variable
    if (mainVar =="Zh" || mainVar == "Pt2" || mainVar == "Phi_PQ"){
        for (int mainVarCounter = 0; mainVarCounter < N_main; mainVarCounter++) {
            //Create integration histogram for each main bin to save in the array
            Integ_liq_uncorr[mainVarCounter] = new TH2D(Form("Integrated_histo_liq_uncorr_%i",
                                    mainVarCounter), "Integrated histo liq uncorr",
                                    N_Nu, Nu_bins[0], Nu_bins[N_Nu],N_Q2, Q2_bins[0], Q2_bins[N_Q2]);

            Integ_sol_uncorr[mainVarCounter] = new TH2D(Form("Integrated_histo_sol_uncorr_%i",
                                    mainVarCounter), "Integrated histo sol uncorr",
                                    N_Nu, Nu_bins[0], Nu_bins[N_Nu],N_Q2, Q2_bins[0], Q2_bins[N_Q2]);

            Integ_liq_corr[mainVarCounter] = new TH2D(Form("Integrated_histo_liq_corr_%i",
                                    mainVarCounter), "Integrated histo liq corr",
                                    N_Nu, Nu_bins[0], Nu_bins[N_Nu],N_Q2, Q2_bins[0], Q2_bins[N_Q2]);

            Integ_sol_corr[mainVarCounter] = new TH2D(Form("Integrated_histo_sol_corr_%i",
                                    mainVarCounter), "Integrated histo sol corr",
                                    N_Nu, Nu_bins[0], Nu_bins[N_Nu],N_Q2, Q2_bins[0], Q2_bins[N_Q2]);
        }
    }

    //Loop over hadronic variables
    int mainVarCounter;
    for (int ZhCounter = 0; ZhCounter < N_Zh; ZhCounter++) {
        for (int Pt2Counter = 0; Pt2Counter < N_Pt2; Pt2Counter++) {
            for (int PhiCounter = 0; PhiCounter < N_Phi; PhiCounter++) {
                //Assign names for each var for getting the histo names
                if (mainVar == "Zh" || mainVar == "Nu" || mainVar == "Q2"){
                    mainVarCounter = ZhCounter;
                }
                if (mainVar == "Pt2"){
                    mainVarCounter = Pt2Counter;
                }
                if (mainVar == "Phi_PQ"){
                    mainVarCounter = PhiCounter;
                }

                //Obtain histos from file
                cout<<"********************* New hadron BIN *******************"<<endl;
                cout<<"Getting histos: "<<Form("%i_%i_%i", ZhCounter, Pt2Counter, PhiCounter)<<endl;
                //Data histos
                h_liq_data = (TH2D*)input_hadron_data->Get(Form("Data_liq_%i_%i_%i", ZhCounter, Pt2Counter, PhiCounter));
                h_sol_data = (TH2D*)input_hadron_data->Get(Form("Data_sol_%i_%i_%i", ZhCounter, Pt2Counter, PhiCounter));
                //Accepted histos
                h_liq_acc = (TH2D*)input_hadron_acc->Get(Form("Data_liq_%i_%i_%i", ZhCounter, Pt2Counter, PhiCounter));
                h_sol_acc = (TH2D*)input_hadron_acc->Get(Form("Data_sol_%i_%i_%i", ZhCounter, Pt2Counter, PhiCounter));
                //Thrown histos
                h_liq_thr = (TH2D*)input_hadron_thr->Get(Form("Data_liq_%i_%i_%i", ZhCounter, Pt2Counter, PhiCounter));
                h_sol_thr = (TH2D*)input_hadron_thr->Get(Form("Data_sol_%i_%i_%i", ZhCounter, Pt2Counter, PhiCounter));

                //acceptance correction factors calculation
                //LIQUID TARGET
                h_liq_corr = (TH2D*)h_liq_data->Clone();
                liq_correction = (TH2D*)h_liq_thr->Clone();
                liq_correction->Divide(h_liq_acc);
                h_liq_corr->Multiply(liq_correction);

                cout<<"adding corrected liq histo with integral:"<<h_liq_corr->Integral()<<endl;
                if (mainVar =="Zh" || mainVar == "Pt2" || mainVar == "Phi_PQ"){
                    Integ_liq_corr[mainVarCounter]->Add(h_liq_corr);
                }
                Integ_liq_corr_total->Add(h_liq_corr);

                //SOLID TARGET
                h_sol_corr = (TH2D*)h_sol_data->Clone();
                sol_correction = (TH2D*)h_sol_thr->Clone();
                sol_correction->Divide(h_sol_acc);
                h_sol_corr->Multiply(sol_correction);

                cout<<"adding corrected sol histo with integral:"<<h_sol_corr->Integral()<<endl;
                if (mainVar =="Zh" || mainVar == "Pt2" || mainVar == "Phi_PQ"){
                    Integ_sol_corr[mainVarCounter]->Add(h_sol_corr);
                }
                Integ_sol_corr_total->Add(h_sol_corr);

                //*** check what to do if the histogram is empty ***
                // maybe just add the uncorrected into the corrected 

                //Add uncorrected histos to integrated histograms
                if (mainVar =="Zh" || mainVar == "Pt2" || mainVar == "Phi_PQ"){
                    Integ_liq_uncorr[mainVarCounter]->Add(h_liq_data);
                    Integ_sol_uncorr[mainVarCounter]->Add(h_sol_data);
                }
                Integ_liq_uncorr_total->Add(h_liq_data);
                Integ_sol_uncorr_total->Add(h_sol_data);

            }
        }
    }
    //Open uptput file to save root stuff in it
    output->cd();

    //Final histograms for main var histograms NON corrected
    TH1D* histo_liquid_uncorr = new TH1D("histo_liq","histo liq", N_main, main_bins[0], main_bins[N_main]);
    TH1D* histo_solid_uncorr = new TH1D("histo_sol","histo sol", N_main, main_bins[0], main_bins[N_main]);

    //Final histograms for main var histograms corrected
    TH1D* histo_liquid_corr = new TH1D("histo_liq_corr","histo liq corr", N_main, main_bins[0], main_bins[N_main]);
    TH1D* histo_solid_corr = new TH1D("histo_sol_corr","histo sol corr", N_main, main_bins[0], main_bins[N_main]);

    //Error propagation
    histo_liquid_uncorr->Sumw2();
    histo_solid_uncorr->Sumw2();
    histo_liquid_corr->Sumw2();
    histo_solid_corr->Sumw2();

    //Variables to save number of corrected hadrons and statistic errors
    Double_t error_liq_uncorr, error_sol_uncorr, error_sol_corr, error_liq_corr;

    //If main var is hadronic variable, set bin values and error using integrated histogram for each main var bin
    if (mainVar =="Zh" || mainVar == "Pt2" || mainVar == "Phi_PQ"){
        for (int mainVarCounter = 0; mainVarCounter < N_main; mainVarCounter++) {
            //Uncorrected
            histo_liquid_uncorr->SetBinContent(mainVarCounter+1, Integ_liq_uncorr[mainVarCounter]->IntegralAndError(1,N_Nu,1, N_Q2, error_liq_uncorr));
            histo_solid_uncorr ->SetBinContent(mainVarCounter+1, Integ_sol_uncorr[mainVarCounter]->IntegralAndError(1,N_Nu,1, N_Q2, error_sol_uncorr));
            histo_liquid_uncorr->SetBinError(mainVarCounter+1, error_liq_uncorr);
            histo_solid_uncorr ->SetBinError(mainVarCounter+1, error_sol_uncorr);
            Integ_liq_uncorr[mainVarCounter]->Write(Form("Q2xNu_histo_liq_%i", mainVarCounter));
            Integ_sol_uncorr[mainVarCounter]->Write(Form("Q2xNu_histo_sol_%i", mainVarCounter));

            //Corrected
            histo_liquid_corr->SetBinContent(mainVarCounter+1, Integ_liq_corr[mainVarCounter]->IntegralAndError(1,N_Nu,1, N_Q2, error_liq_corr));
            histo_solid_corr ->SetBinContent(mainVarCounter+1, Integ_sol_corr[mainVarCounter]->IntegralAndError(1,N_Nu,1, N_Q2, error_sol_corr));
            histo_liquid_corr->SetBinError(mainVarCounter+1, error_liq_corr);
            histo_solid_corr ->SetBinError(mainVarCounter+1, error_sol_corr);
            Integ_liq_corr[mainVarCounter]->Write(Form("NuxQ2_histo_liq_%i_corr", mainVarCounter));
            Integ_sol_corr[mainVarCounter]->Write(Form("NuxQ2_histo_sol_%i_corr", mainVarCounter));

            //IF I NEED TO LOOK AT THE MR RATIO FOR SPECIFIC BIN OF NU AND Q2 JUST INSTEAD OF INTEGRATE, JUST TAKE THE VALUE AND ERROR OF THE SPECIFIC BIN
            //THE REST SHOULD BE THE SAME
            //COULD ADD A NEW CYCLE THAT CALCULATE THE MR FOR EACH Q2 AND NU BIN HERE AND IN THE FIANL CALCULATION
        }
    }
    //If main var is electron variable, project the total TH2 into TH1
    else if (mainVar == "Nu"){
        //Nu histo
        histo_liquid_corr = (TH1D*)Integ_liq_corr_total->ProjectionX("histo_liq_corr", 1 , N_Q2, "e");
        histo_solid_corr = (TH1D*)Integ_sol_corr_total->ProjectionX("histo_sol_corr", 1 , N_Q2, "e");
        histo_liquid_uncorr = (TH1D*)Integ_liq_uncorr_total->ProjectionX("histo_liq_uncorr", 1 , N_Q2, "e");
        histo_solid_uncorr = (TH1D*)Integ_sol_uncorr_total->ProjectionX("histo_sol_uncorr", 1 , N_Q2, "e");
    }
    else if (mainVar == "Q2"){
        //Q2 histo
        histo_liquid_corr = (TH1D*)Integ_liq_corr_total->ProjectionY("histo_liq_corr", 1 , N_Nu, "e");
        histo_solid_corr = (TH1D*)Integ_sol_corr_total->ProjectionY("histo_sol_corr", 1 , N_Nu, "e");
        histo_liquid_uncorr = (TH1D*)Integ_liq_uncorr_total->ProjectionY("histo_liq_uncorr", 1 , N_Nu, "e");
        histo_solid_uncorr = (TH1D*)Integ_sol_uncorr_total->ProjectionY("histo_sol_uncorr", 1 , N_Nu, "e");
    }

    Integ_liq_uncorr_total->Write("NuxQ2_liq_total_uncorr");
    Integ_liq_corr_total->Write("NuxQ2_liq_total_uncorr");
    Integ_sol_uncorr_total->Write("NuxQ2_sol_total_corr");
    Integ_sol_corr_total->Write("NuxQ2_sol_total_corr");

////////////////////////////////////////////////////////////////////////
//////////           ELECTRON ACEPTANCE CORRECTION            //////////
////////////////////////////////////////////////////////////////////////

    //Draw the 2D electron plots
    elec_tuple_data->Draw(Form("Q2:nu>>h_elec_sol_data(%i,%f,%f,%i,%f,%f)",N_Nu,Nu_bins[0],Nu_bins[N_Nu], N_Q2,Q2_bins[0],Q2_bins[N_Q2]), Main_cut&&vz_solid, "goff");
    elec_tuple_data->Draw(Form("Q2:nu>>h_elec_liq_data(%i,%f,%f,%i,%f,%f)",N_Nu,Nu_bins[0],Nu_bins[N_Nu], N_Q2,Q2_bins[0],Q2_bins[N_Q2]), Main_cut&&vz_d2, "goff");
    elec_tuple_acc ->Draw(Form("Q2:nu>>h_elec_sol_acc(%i,%f,%f,%i,%f,%f)", N_Nu,Nu_bins[0],Nu_bins[N_Nu], N_Q2,Q2_bins[0],Q2_bins[N_Q2]), Main_cut&&vz_solid, "goff");
    elec_tuple_acc ->Draw(Form("Q2:nu>>h_elec_liq_acc(%i,%f,%f,%i,%f,%f)", N_Nu,Nu_bins[0],Nu_bins[N_Nu], N_Q2,Q2_bins[0],Q2_bins[N_Q2]), Main_cut&&vz_d2, "goff");
    elec_tuple_thr ->Draw(Form("Q2:nu>>h_elec_sol_thr(%i,%f,%f,%i,%f,%f)", N_Nu,Nu_bins[0],Nu_bins[N_Nu], N_Q2,Q2_bins[0],Q2_bins[N_Q2]), Main_cut&&vz_solid, "goff");
    elec_tuple_thr ->Draw(Form("Q2:nu>>h_elec_liq_thr(%i,%f,%f,%i,%f,%f)", N_Nu,Nu_bins[0],Nu_bins[N_Nu], N_Q2,Q2_bins[0],Q2_bins[N_Q2]), Main_cut&&vz_d2, "goff");

    //Save the plots into TH2D to access them later
    TH2D *h_elec_sol_data = (TH2D*)gDirectory->Get("h_elec_sol_data");
    TH2D *h_elec_liq_data = (TH2D*)gDirectory->Get("h_elec_liq_data");
    TH2D *h_elec_sol_acc = (TH2D*)gDirectory->Get("h_elec_sol_acc");
    TH2D *h_elec_liq_acc = (TH2D*)gDirectory->Get("h_elec_liq_acc");
    TH2D *h_elec_sol_thr = (TH2D*)gDirectory->Get("h_elec_sol_thr");
    TH2D *h_elec_liq_thr = (TH2D*)gDirectory->Get("h_elec_liq_thr");

    //TH2 with corrected number of electrons
    TH2D *h_elec_sol_corr = (TH2D*)h_elec_sol_data->Clone();
    TH2D *h_elec_liq_corr = (TH2D*)h_elec_liq_data->Clone();
    h_elec_sol_corr->Multiply(h_elec_sol_thr);
    h_elec_sol_corr->Divide(h_elec_sol_acc);
    h_elec_liq_corr->Multiply(h_elec_liq_thr);
    h_elec_liq_corr->Divide(h_elec_liq_acc);

    //Count number of corrected electrons and statistic errors
    Double_t n_elec_sol_corr, n_elec_liq_corr, n_elec_error_liq, n_elec_error_sol;
    n_elec_sol_corr = h_elec_sol_corr->IntegralAndError(1, N_Nu,1, N_Q2, n_elec_error_sol);
    n_elec_liq_corr = h_elec_liq_corr->IntegralAndError(1, N_Nu,1, N_Q2, n_elec_error_liq);

    //1D electron histograms for the MR niormalization
    TH1D* elec_hist_liq_corr = new TH1D("elec_hist_liq_corr", "", N_main, main_bins[0], main_bins[N_main]);
    TH1D* elec_hist_sol_corr = new TH1D("elec_hist_sol_corr", "", N_main, main_bins[0], main_bins[N_main]);
    //For hadron MainVar. Convert number of electron into flat histogram with errors.
    if (mainVar =="Zh" || mainVar == "Pt2" || mainVar == "Phi_PQ"){
        for (int i = 1; i <= N_main; i++) {
            elec_hist_sol_corr->SetBinContent(i, n_elec_sol_corr);
            elec_hist_liq_corr->SetBinContent(i, n_elec_liq_corr);
            elec_hist_sol_corr->SetBinError(i, n_elec_error_sol);
            elec_hist_liq_corr->SetBinError(i, n_elec_error_liq);
        }
    }

    //For electron MainVar. Project TH2 into TH1.
    //Nu histo
    else if (mainVar == "Nu"){
        elec_hist_sol_corr = (TH1D*)h_elec_sol_corr->ProjectionX("elec_nu_sol_corr", 1 , N_Q2, "e");
        elec_hist_liq_corr = (TH1D*)h_elec_liq_corr->ProjectionX("elec_nu_liq_corr", 1 , N_Q2, "e");
    }
    //Q2 histo
    else if (mainVar == "Q2"){
        elec_hist_sol_corr = (TH1D*)h_elec_sol_corr->ProjectionY("elec_q2_sol_corr", 1 , N_Nu, "e");
        elec_hist_liq_corr = (TH1D*)h_elec_liq_corr->ProjectionY("elec_q2_liq_corr", 1 , N_Nu, "e");
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
    TH1D* elec_hist_liq_uncorr = new TH1D("elec_hist_liq_uncorr", "", N_main, main_bins[0], main_bins[N_main]);
    TH1D* elec_hist_sol_uncorr = new TH1D("elec_hist_sol_uncorr", "", N_main, main_bins[0], main_bins[N_main]);
        for (int i = 1; i <= N_main; i++) {
        elec_hist_liq_uncorr->SetBinContent(i, n_e_liq);
        elec_hist_sol_uncorr->SetBinContent(i, n_e_sol);
    }

    //print electron info
    cout<<"*******************************************************************************"<<endl;
    cout<<"UNCORRECTED-> ne_sol:"<<n_e_sol<<", ne_liq:"<<n_e_liq<<", ratio:"<<n_e_sol/n_e_liq<<endl;
    cout<<"CORRECTED-> ne_sol_corr:"<<n_elec_sol_corr<<", ne_liq_corr:"<<n_elec_liq_corr<<", ratio:"<<n_elec_sol_corr/n_elec_liq_corr<<endl;

    //DEBUG
    elec_tuple_data->Draw(Form("nu>>h_elec_nu_sol_data(%i,%f,%f)",N_Nu,Nu_bins[0],Nu_bins[N_Nu]), Main_cut&&vz_solid, "goff");
    elec_tuple_data->Draw(Form("nu>>h_elec_nu_liq_data(%i,%f,%f)",N_Nu,Nu_bins[0],Nu_bins[N_Nu]), Main_cut&&vz_d2, "goff");
    elec_tuple_data->Draw(Form("Q2>>h_elec_q2_sol_data(%i,%f,%f)",N_Q2,Q2_bins[0],Q2_bins[N_Q2]), Main_cut&&vz_solid, "goff");
    elec_tuple_data->Draw(Form("Q2>>h_elec_q2_liq_data(%i,%f,%f)",N_Q2,Q2_bins[0],Q2_bins[N_Q2]), Main_cut&&vz_d2, "goff");
    TH1D* h_elec_nu_sol_data = (TH1D*)gDirectory->Get("h_elec_nu_sol_data");
    TH1D* h_elec_nu_liq_data = (TH1D*)gDirectory->Get("h_elec_nu_liq_data");
    TH1D* h_elec_q2_sol_data = (TH1D*)gDirectory->Get("h_elec_q2_sol_data");
    TH1D* h_elec_q2_liq_data = (TH1D*)gDirectory->Get("h_elec_q2_liq_data");

    //Nu histo
    if (mainVar == "Nu"){
        elec_hist_sol_uncorr = h_elec_nu_sol_data;
        elec_hist_liq_uncorr = h_elec_nu_liq_data;
    }
    //Q2 histo
    else if (mainVar == "Q2"){
        elec_hist_sol_uncorr = h_elec_q2_sol_data;
        elec_hist_liq_uncorr = h_elec_q2_liq_data;
    }

////////////////////////////////////////////////////////////////////////
//////////          MULTIPLICITY RATIO CALCULATION            //////////
////////////////////////////////////////////////////////////////////////

    //TH1 histograms for multiplicity ratio
    TH1D* mr_uncorr = new TH1D("MR_uncorr","MR_uncorr", N_main, main_bins[0], main_bins[N_main]);
    TH1D* mr_corr = new TH1D("MR_corr","MR_corr", N_main, main_bins[0], main_bins[N_main]);
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
-->Make that the macro calculate all possible MR in one go
-->Make MR for each Nu and Q2 bin
-->Use real simulations
-->Determine what to do with empty bins
*/