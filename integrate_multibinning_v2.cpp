#include "include.h"

void integrate_multibinning_v2(TString Target="C", int Hadron_pid=211, TString mainVar="Zh"){
    ROOT::EnableImplicitMT(); //To run with multithreads (idk if it works)

////////////////////////////////////////////////////////////////////////
//////////                  SETTING VARIABLES                 //////////
////////////////////////////////////////////////////////////////////////
    cout<<"Setting variables"<<endl;

    //Variables to use. The calculation is for the first variable in the list, called MainVar
    int N_main;
    float *main_bins = nullptr;

    //N_main and main_bins correspond to the number of bins and their limits respectively
    if (mainVar == "Zh"){
        N_main = N_Zh;
        main_bins = Zh_bins;
    }
    else if (mainVar == "Pt2"){
        N_main = N_Pt2;
        main_bins = Pt2_bins;
    }
    else if (mainVar == "Phi_PQ"){
        N_main = N_Phi;
        main_bins = Phi_bins;
    }
    else if (mainVar == "Nu"){
        N_main = N_Nu;
        main_bins = Nu_bins;
    }
    else if (mainVar == "Q2"){
        N_main = N_Q2;
        main_bins = Q2_bins;
    }
    else{
        cout<<"Variable not valid"<<endl;
        return;
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
        return;
    }
    cout<<"Running MR scritpt for "<<hadron<<" variable "<<mainVar<<endl;

////////////////////////////////////////////////////////////////////////
//////////               INPUT AND OUTPUT FILES               //////////
////////////////////////////////////////////////////////////////////////
    cout<<"------------------------------------------------------------"<<endl;

    //DATA
    //Get hadron histograms from root file created with create_multibinning
    TFile *input_hadron_data = new TFile("output/data/"+Target+"/data_binned_"+hadron+".root","READ");
    //Get electron TNtuple input created from simple_plots
    TFile *input_elec_data = new TFile("output/data/"+Target+"/out_clas12.root","READ");
    TNtuple* elec_tuple_data = (TNtuple*)input_elec_data->Get("elec_tuple");

    //ACCEPTED
    //hadrons
    TFile *input_hadron_acc_sol = new TFile("output/simul/"+Target+"/data_binned_"+hadron+".root","READ");
    TFile *input_hadron_acc_liq = new TFile("output/simul/D2/data_binned_"+hadron+".root","READ");
    //electrons
    TFile *input_elec_acc_sol = new TFile("output/simul/"+Target+"/out_clas12.root","READ");
    TFile *input_elec_acc_liq = new TFile("output/simul/D2/out_clas12.root","READ");
    TNtuple* elec_tuple_acc_sol = (TNtuple*)input_elec_acc_sol->Get("elec_tuple");
    TNtuple* elec_tuple_acc_liq = (TNtuple*)input_elec_acc_liq->Get("elec_tuple");

    //THROWN
    //hadrons
    TFile *input_hadron_thr_sol = new TFile("output/simul/"+Target+"/thrown/data_binned_"+hadron+".root","READ");
    TFile *input_hadron_thr_liq = new TFile("output/simul/D2/thrown/data_binned_"+hadron+".root","READ");
    //electrons
    TFile *input_elec_thr_sol = new TFile("output/simul/"+Target+"/thrown/out_clas12.root","READ");
    TFile *input_elec_thr_liq = new TFile("output/simul/D2/thrown/out_clas12.root","READ");
    TNtuple* elec_tuple_thr_sol = (TNtuple*)input_elec_thr_sol->Get("elec_tuple");
    TNtuple* elec_tuple_thr_liq = (TNtuple*)input_elec_thr_liq->Get("elec_tuple");

    //OUTPUT file
    TFile *output = new TFile("output/"+Target+"/mr_"+mainVar+"_"+hadron+".root","RECREATE");

////////////////////////////////////////////////////////////////////////
//////////            HADRON ACEPTANCE CORRECTION             //////////
////////////////////////////////////////////////////////////////////////
    cout<<"------------------------------------------------------------"<<endl;
    cout<<"Calculating hadron acceptance correction"<<endl;

    //Array of histograms to integrate entries from input root file
    //Uncorrected
    TH2D* h2_integ_liq_data[N_main];
    TH2D* h2_integ_sol_data[N_main];
    //Aceptance Corrected
    TH2D* h2_integ_liq_corr[N_main];
    TH2D* h2_integ_sol_corr[N_main];
    //TH2D needed later
    TH2D *h2_liq_data, *h2_liq_acc, *h2_liq_thr, *h2_liq_corr, *h2_liq_correction;
    TH2D *h2_sol_data, *h2_sol_acc, *h2_sol_thr, *h2_sol_corr, *h2_sol_correction;

    //2D histogram to save total integrated histograms, needed for electron main var MR
    TH2D* h2_integ_liq_data_total = new TH2D("Integrated_histo_liq_data","", N_Nu, Nu_bins[0], Nu_bins[N_Nu], N_Q2, Q2_bins[0], Q2_bins[N_Q2]);
    TH2D* h2_integ_sol_data_total = new TH2D("Integrated_histo_sol_data","", N_Nu, Nu_bins[0], Nu_bins[N_Nu], N_Q2, Q2_bins[0], Q2_bins[N_Q2]);
    TH2D* h2_integ_liq_corr_total = new TH2D("Integrated_histo_liq_corr","", N_Nu, Nu_bins[0], Nu_bins[N_Nu], N_Q2, Q2_bins[0], Q2_bins[N_Q2]);
    TH2D* h2_integ_sol_corr_total = new TH2D("Integrated_histo_sol_corr","", N_Nu, Nu_bins[0], Nu_bins[N_Nu], N_Q2, Q2_bins[0], Q2_bins[N_Q2]);
    h2_integ_liq_data_total->Sumw2();
    h2_integ_sol_data_total->Sumw2();
    h2_integ_liq_corr_total->Sumw2();
    h2_integ_sol_corr_total->Sumw2();

    //Loop every bin for every variable
    if (mainVar =="Zh" || mainVar == "Pt2" || mainVar == "Phi_PQ"){
        for (int mainVarCounter = 0; mainVarCounter < N_main; mainVarCounter++) {
            //Create integration histogram for each main bin to save in the array
            h2_integ_liq_data[mainVarCounter] = new TH2D(Form("Integrated_histo_liq_data_%i",
                                    mainVarCounter), "Integrated histo liq uncorr",
                                    N_Nu, Nu_bins[0], Nu_bins[N_Nu],N_Q2, Q2_bins[0], Q2_bins[N_Q2]);

            h2_integ_sol_data[mainVarCounter] = new TH2D(Form("Integrated_histo_sol_data_%i",
                                    mainVarCounter), "Integrated histo sol uncorr",
                                    N_Nu, Nu_bins[0], Nu_bins[N_Nu],N_Q2, Q2_bins[0], Q2_bins[N_Q2]);

            h2_integ_liq_corr[mainVarCounter] = new TH2D(Form("Integrated_histo_liq_corr_%i",
                                    mainVarCounter), "Integrated histo liq corr",
                                    N_Nu, Nu_bins[0], Nu_bins[N_Nu],N_Q2, Q2_bins[0], Q2_bins[N_Q2]);

            h2_integ_sol_corr[mainVarCounter] = new TH2D(Form("Integrated_histo_sol_corr_%i",
                                    mainVarCounter), "Integrated histo sol corr",
                                    N_Nu, Nu_bins[0], Nu_bins[N_Nu],N_Q2, Q2_bins[0], Q2_bins[N_Q2]);
            //Error propagation
            h2_integ_liq_data[mainVarCounter]->Sumw2();
            h2_integ_sol_data[mainVarCounter]->Sumw2();
            h2_integ_liq_corr[mainVarCounter]->Sumw2();
            h2_integ_sol_corr[mainVarCounter]->Sumw2();
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
                h2_liq_data = (TH2D*)input_hadron_data->Get(Form("Data_liq_%i_%i_%i", ZhCounter, Pt2Counter, PhiCounter));
                h2_sol_data = (TH2D*)input_hadron_data->Get(Form("Data_sol_%i_%i_%i", ZhCounter, Pt2Counter, PhiCounter));
                //Accepted histos
                h2_liq_acc = (TH2D*)input_hadron_acc_liq->Get(Form("Data_liq_%i_%i_%i", ZhCounter, Pt2Counter, PhiCounter));
                h2_sol_acc = (TH2D*)input_hadron_acc_sol->Get(Form("Data_sol_%i_%i_%i", ZhCounter, Pt2Counter, PhiCounter));
                //Thrown histos
                h2_liq_thr = (TH2D*)input_hadron_thr_liq->Get(Form("Data_liq_%i_%i_%i", ZhCounter, Pt2Counter, PhiCounter));
                h2_sol_thr = (TH2D*)input_hadron_thr_sol->Get(Form("Data_sol_%i_%i_%i", ZhCounter, Pt2Counter, PhiCounter));

                //acceptance correction factors calculation
                //LIQUID TARGET
                h2_liq_corr = (TH2D*)h2_liq_data->Clone();
                h2_liq_correction = (TH2D*)h2_liq_thr->Clone();
                h2_liq_corr->Sumw2();
                h2_liq_correction->Sumw2();
                h2_liq_correction->Divide(h2_liq_acc);
                h2_liq_corr->Multiply(h2_liq_correction);

                cout<<"adding corrected liq histo with integral:"<<h2_liq_corr->Integral()<<endl;
                if (mainVar =="Zh" || mainVar == "Pt2" || mainVar == "Phi_PQ"){
                    h2_integ_liq_corr[mainVarCounter]->Add(h2_liq_corr);
                }
                h2_integ_liq_corr_total->Add(h2_liq_corr);

                //SOLID TARGET
                h2_sol_corr = (TH2D*)h2_sol_data->Clone();
                h2_sol_correction = (TH2D*)h2_sol_thr->Clone();
                h2_sol_corr->Sumw2();
                h2_sol_correction->Sumw2();
                h2_sol_correction->Divide(h2_sol_acc);
                h2_sol_corr->Multiply(h2_sol_correction);

                cout<<"adding corrected sol histo with integral:"<<h2_sol_corr->Integral()<<endl;
                if (mainVar =="Zh" || mainVar == "Pt2" || mainVar == "Phi_PQ"){
                    h2_integ_sol_corr[mainVarCounter]->Add(h2_sol_corr);
                }
                h2_integ_sol_corr_total->Add(h2_sol_corr);

                //*** check what to do if the histogram is empty ***
                // maybe just add the uncorrected into the corrected 

                //Add uncorrected histos to integrated histograms
                cout<<"adding data liq histo with integral:"<<h2_liq_data->Integral()<<endl;
                cout<<"adding data sol histo with integral:"<<h2_sol_data->Integral()<<endl;
                if (mainVar =="Zh" || mainVar == "Pt2" || mainVar == "Phi_PQ"){
                    h2_integ_liq_data[mainVarCounter]->Add(h2_liq_data);
                    h2_integ_sol_data[mainVarCounter]->Add(h2_sol_data);
                }
                h2_integ_liq_data_total->Add(h2_liq_data);
                h2_integ_sol_data_total->Add(h2_sol_data);

            }
        }
    }
    cout<<"------------------------------------------------------------"<<endl;
    cout<<"Ended looping over hadron bins"<<endl;

    //Open uptput file to save root stuff in it
    output->cd();

    //Final histograms for main var histograms NON corrected
    TH1D* h_liquid_data = new TH1D("histo_liq","histo liq", N_main, main_bins[0], main_bins[N_main]);
    TH1D* h_solid_data  = new TH1D("histo_sol","histo sol", N_main, main_bins[0], main_bins[N_main]);

    //Final histograms for main var histograms corrected
    TH1D* h_liquid_corr = new TH1D("histo_liq_corr","histo liq corr", N_main, main_bins[0], main_bins[N_main]);
    TH1D* h_solid_corr  = new TH1D("histo_sol_corr","histo sol corr", N_main, main_bins[0], main_bins[N_main]);

    //Error propagation
    h_liquid_data->Sumw2();
    h_solid_data->Sumw2();
    h_liquid_corr->Sumw2();
    h_solid_corr->Sumw2();

    //For bins in Nu and Q2
    TH1D* h_ebin_liquid_data[N_Nu][N_Q2];
    TH1D* h_ebin_solid_data [N_Nu][N_Q2];
    TH1D* h_ebin_liquid_corr[N_Nu][N_Q2];
    TH1D* h_ebin_solid_corr [N_Nu][N_Q2];

    if (mainVar =="Zh" || mainVar == "Pt2" || mainVar == "Phi_PQ"){
        for (int NuCounter = 0; NuCounter < N_Nu; ++NuCounter){
            for (int Q2Counter = 0; Q2Counter < N_Q2; ++Q2Counter){
                //Create integration histogram for each Nu and Q2 bin to save in the array
                h_ebin_liquid_data[NuCounter][Q2Counter] = new TH1D(Form("histo_liq_data_%i_%i", NuCounter,Q2Counter),"histo liq data", N_main, main_bins[0], main_bins[N_main]);
                h_ebin_solid_data[NuCounter][Q2Counter]  = new TH1D(Form("histo_sol_data_%i_%i", NuCounter,Q2Counter),"histo sol data", N_main, main_bins[0], main_bins[N_main]);
                h_ebin_liquid_corr[NuCounter][Q2Counter] = new TH1D(Form("histo_liq_corr_%i_%i", NuCounter,Q2Counter),"histo liq corr", N_main, main_bins[0], main_bins[N_main]);
                h_ebin_solid_corr[NuCounter][Q2Counter]  = new TH1D(Form("histo_sol_corr_%i_%i", NuCounter,Q2Counter),"histo sol corr", N_main, main_bins[0], main_bins[N_main]);

                //Error propagation
                h_ebin_liquid_data[NuCounter][Q2Counter]->Sumw2();
                h_ebin_solid_data[NuCounter][Q2Counter] ->Sumw2();
                h_ebin_liquid_corr[NuCounter][Q2Counter]->Sumw2();
                h_ebin_solid_corr[NuCounter][Q2Counter] ->Sumw2();
            }
        }
    }

    //Variables to save number of corrected hadrons and statistic errors
    Double_t error_liq_data, error_sol_data, error_sol_corr, error_liq_corr;

    //If main var is hadronic variable, set bin values and error using integrated histogram for each main var bin
    if (mainVar =="Zh" || mainVar == "Pt2" || mainVar == "Phi_PQ"){
        for (int mainVarCounter = 0; mainVarCounter < N_main; mainVarCounter++) {
            //Uncorrected
            h_liquid_data->SetBinContent(mainVarCounter+1, h2_integ_liq_data[mainVarCounter]->IntegralAndError(1,N_Nu,1, N_Q2, error_liq_data));
            h_solid_data ->SetBinContent(mainVarCounter+1, h2_integ_sol_data[mainVarCounter]->IntegralAndError(1,N_Nu,1, N_Q2, error_sol_data));
            h_liquid_data->SetBinError(mainVarCounter+1, error_liq_data);
            h_solid_data ->SetBinError(mainVarCounter+1, error_sol_data);
            h2_integ_liq_data[mainVarCounter]->Write(Form("NuxQ2_histo_liq_%i", mainVarCounter));
            h2_integ_sol_data[mainVarCounter]->Write(Form("NuxQ2_histo_sol_%i", mainVarCounter));

            //Corrected
            h_liquid_corr->SetBinContent(mainVarCounter+1, h2_integ_liq_corr[mainVarCounter]->IntegralAndError(1,N_Nu,1, N_Q2, error_liq_corr));
            h_solid_corr ->SetBinContent(mainVarCounter+1, h2_integ_sol_corr[mainVarCounter]->IntegralAndError(1,N_Nu,1, N_Q2, error_sol_corr));
            h_liquid_corr->SetBinError(mainVarCounter+1, error_liq_corr);
            h_solid_corr ->SetBinError(mainVarCounter+1, error_sol_corr);
            h2_integ_liq_corr[mainVarCounter]->Write(Form("NuxQ2_histo_liq_%i_corr", mainVarCounter));
            h2_integ_sol_corr[mainVarCounter]->Write(Form("NuxQ2_histo_sol_%i_corr", mainVarCounter));

            //Save content in specific Nu and Q2 bin value
            for (int NuCounter = 0; NuCounter < N_Nu; ++NuCounter){
                for (int Q2Counter = 0; Q2Counter < N_Q2; ++Q2Counter){
                    //Uncorrected
                    h_ebin_liquid_data[NuCounter][Q2Counter]->SetBinContent(mainVarCounter+1, h2_integ_liq_data[mainVarCounter]->IntegralAndError(NuCounter+1,NuCounter+1,Q2Counter+1, Q2Counter+1, error_liq_data));
                    h_ebin_solid_data[NuCounter][Q2Counter] ->SetBinContent(mainVarCounter+1, h2_integ_sol_data[mainVarCounter]->IntegralAndError(NuCounter+1,NuCounter+1,Q2Counter+1, Q2Counter+1, error_sol_data));
                    h_ebin_liquid_data[NuCounter][Q2Counter]->SetBinError(mainVarCounter+1, error_liq_data);
                    h_ebin_solid_data[NuCounter][Q2Counter] ->SetBinError(mainVarCounter+1, error_sol_data);

                    //Corrected
                    h_ebin_liquid_corr[NuCounter][Q2Counter]->SetBinContent(mainVarCounter+1, h2_integ_liq_corr[mainVarCounter]->IntegralAndError(NuCounter+1,NuCounter+1,Q2Counter+1, Q2Counter+1, error_liq_corr));
                    h_ebin_solid_corr[NuCounter][Q2Counter] ->SetBinContent(mainVarCounter+1, h2_integ_sol_corr[mainVarCounter]->IntegralAndError(NuCounter+1,NuCounter+1,Q2Counter+1, Q2Counter+1, error_sol_corr));
                    h_ebin_liquid_corr[NuCounter][Q2Counter]->SetBinError(mainVarCounter+1, error_liq_corr);
                    h_ebin_solid_corr[NuCounter][Q2Counter] ->SetBinError(mainVarCounter+1, error_sol_corr);
                }
            }
        }
        //DEBUG
        for (int NuCounter = 0; NuCounter < N_Nu; ++NuCounter){
            for (int Q2Counter = 0; Q2Counter < N_Q2; ++Q2Counter){
                h_ebin_liquid_data[NuCounter][Q2Counter]->Write();
                h_ebin_solid_data[NuCounter][Q2Counter] ->Write();
                h_ebin_liquid_corr[NuCounter][Q2Counter]->Write();
                h_ebin_solid_corr[NuCounter][Q2Counter] ->Write();
            }
        }
        //END OF DEBUG
    }

    //If main var is electron variable, project the total TH2 into TH1
    else if (mainVar == "Nu"){
        //Nu histo
        h_liquid_corr = (TH1D*)h2_integ_liq_corr_total->ProjectionX("histo_liq_corr", 1 , N_Q2, "e");
        h_solid_corr  = (TH1D*)h2_integ_sol_corr_total->ProjectionX("histo_sol_corr", 1 , N_Q2, "e");
        h_liquid_data = (TH1D*)h2_integ_liq_data_total->ProjectionX("histo_liq_data", 1 , N_Q2, "e");
        h_solid_data  = (TH1D*)h2_integ_sol_data_total->ProjectionX("histo_sol_data", 1 , N_Q2, "e");
    }
    else if (mainVar == "Q2"){
        //Q2 histo
        h_liquid_corr = (TH1D*)h2_integ_liq_corr_total->ProjectionY("histo_liq_corr", 1 , N_Nu, "e");
        h_solid_corr  = (TH1D*)h2_integ_sol_corr_total->ProjectionY("histo_sol_corr", 1 , N_Nu, "e");
        h_liquid_data = (TH1D*)h2_integ_liq_data_total->ProjectionY("histo_liq_data", 1 , N_Nu, "e");
        h_solid_data  = (TH1D*)h2_integ_sol_data_total->ProjectionY("histo_sol_data", 1 , N_Nu, "e");
    }

    h2_integ_liq_data_total->Write("NuxQ2_liq_total_data");
    h2_integ_liq_corr_total->Write("NuxQ2_liq_total_corr");
    h2_integ_sol_data_total->Write("NuxQ2_sol_total_data");
    h2_integ_sol_corr_total->Write("NuxQ2_sol_total_corr");

////////////////////////////////////////////////////////////////////////
//////////           ELECTRON ACEPTANCE CORRECTION            //////////
////////////////////////////////////////////////////////////////////////
    cout<<"------------------------------------------------------------"<<endl;
    cout<<"Calculating electron acceptance correction"<<endl;

    //Draw 2D electron plots
    elec_tuple_data->Draw(Form("Q2:nu>>h2_elec_sol_data(%i,%f,%f,%i,%f,%f)",N_Nu,Nu_bins[0],Nu_bins[N_Nu], N_Q2,Q2_bins[0],Q2_bins[N_Q2]), Main_cut&&vz_solid, "goff");
    elec_tuple_data->Draw(Form("Q2:nu>>h2_elec_liq_data(%i,%f,%f,%i,%f,%f)",N_Nu,Nu_bins[0],Nu_bins[N_Nu], N_Q2,Q2_bins[0],Q2_bins[N_Q2]), Main_cut&&vz_d2, "goff");
    elec_tuple_acc_sol ->Draw(Form("Q2:nu>>h2_elec_sol_acc(%i,%f,%f,%i,%f,%f)", N_Nu,Nu_bins[0],Nu_bins[N_Nu], N_Q2,Q2_bins[0],Q2_bins[N_Q2]), Main_cut&&vz_solid, "goff");
    elec_tuple_acc_liq ->Draw(Form("Q2:nu>>h2_elec_liq_acc(%i,%f,%f,%i,%f,%f)", N_Nu,Nu_bins[0],Nu_bins[N_Nu], N_Q2,Q2_bins[0],Q2_bins[N_Q2]), Main_cut&&vz_d2, "goff");
    elec_tuple_thr_sol ->Draw(Form("Q2:nu>>h2_elec_sol_thr(%i,%f,%f,%i,%f,%f)", N_Nu,Nu_bins[0],Nu_bins[N_Nu], N_Q2,Q2_bins[0],Q2_bins[N_Q2]), Main_cut, "goff");
    elec_tuple_thr_liq ->Draw(Form("Q2:nu>>h2_elec_liq_thr(%i,%f,%f,%i,%f,%f)", N_Nu,Nu_bins[0],Nu_bins[N_Nu], N_Q2,Q2_bins[0],Q2_bins[N_Q2]), Main_cut, "goff");

    //Save the plots into TH2D to access them later
    TH2D *h2_elec_sol_data = (TH2D*)gDirectory->Get("h2_elec_sol_data");
    TH2D *h2_elec_liq_data = (TH2D*)gDirectory->Get("h2_elec_liq_data");
    TH2D *h2_elec_sol_acc = (TH2D*)gDirectory->Get("h2_elec_sol_acc");
    TH2D *h2_elec_liq_acc = (TH2D*)gDirectory->Get("h2_elec_liq_acc");
    TH2D *h2_elec_sol_thr = (TH2D*)gDirectory->Get("h2_elec_sol_thr");
    TH2D *h2_elec_liq_thr = (TH2D*)gDirectory->Get("h2_elec_liq_thr");
    //Error propagation
    h2_elec_sol_data->Sumw2();
    h2_elec_liq_data->Sumw2();
    h2_elec_sol_acc->Sumw2();
    h2_elec_liq_acc->Sumw2();
    h2_elec_sol_thr->Sumw2();
    h2_elec_liq_thr->Sumw2();

    //CORRECTED
    //Calculate TH2 with corrected number of electrons
    TH2D *h2_elec_sol_corr = (TH2D*)h2_elec_sol_data->Clone();
    TH2D *h2_elec_liq_corr = (TH2D*)h2_elec_liq_data->Clone();
    h2_elec_sol_corr->Sumw2();
    h2_elec_liq_corr->Sumw2();

    //Calculation
    h2_elec_sol_corr->Multiply(h2_elec_sol_thr);
    h2_elec_sol_corr->Divide(h2_elec_sol_acc);
    h2_elec_liq_corr->Multiply(h2_elec_liq_thr);
    h2_elec_liq_corr->Divide(h2_elec_liq_acc);

    //For hadronic variable, count number of corrected electrons and statistic errors
    Double_t n_elec_sol_corr, n_elec_liq_corr, n_elec_error_liq_corr, n_elec_error_sol_corr;
    n_elec_sol_corr = h2_elec_sol_corr->IntegralAndError(1, N_Nu,1, N_Q2, n_elec_error_sol_corr);
    n_elec_liq_corr = h2_elec_liq_corr->IntegralAndError(1, N_Nu,1, N_Q2, n_elec_error_liq_corr);

    //1D electron histograms for the MR normalization
    TH1D* h_elec_liq_corr = new TH1D("h_elec_liq_corr", "", N_main, main_bins[0], main_bins[N_main]);
    TH1D* h_elec_sol_corr = new TH1D("h_elec_sol_corr", "", N_main, main_bins[0], main_bins[N_main]);
    h_elec_liq_corr->Sumw2();
    h_elec_sol_corr->Sumw2();

    //For hadron MainVar. Convert number of electron into flat histogram with errors.
    if (mainVar =="Zh" || mainVar == "Pt2" || mainVar == "Phi_PQ"){
        for (int i = 1; i <= N_main; i++) {
            h_elec_sol_corr->SetBinContent(i, n_elec_sol_corr);
            h_elec_liq_corr->SetBinContent(i, n_elec_liq_corr);
            h_elec_sol_corr->SetBinError(i, n_elec_error_sol_corr);
            h_elec_liq_corr->SetBinError(i, n_elec_error_liq_corr);
        }
    }

    //For electron varaible, project TH2 into TH1.
    //Nu histo
    else if (mainVar == "Nu"){
        h_elec_sol_corr = (TH1D*)h2_elec_sol_corr->ProjectionX("elec_nu_sol_corr", 1 , N_Q2, "e");
        h_elec_liq_corr = (TH1D*)h2_elec_liq_corr->ProjectionX("elec_nu_liq_corr", 1 , N_Q2, "e");
    }
    //Q2 histo
    else if (mainVar == "Q2"){
        h_elec_sol_corr = (TH1D*)h2_elec_sol_corr->ProjectionY("elec_q2_sol_corr", 1 , N_Nu, "e");
        h_elec_liq_corr = (TH1D*)h2_elec_liq_corr->ProjectionY("elec_q2_liq_corr", 1 , N_Nu, "e");
    }

    //Save TH2 electron plots into file
    h2_elec_sol_data->Write("elec_sol_data");
    h2_elec_sol_acc->Write("elec_sol_acc");
    h2_elec_sol_thr->Write("elec_sol_thr");
    h2_elec_sol_corr->Write("elec_sol_corr");
    h2_elec_liq_data->Write("elec_liq_data");
    h2_elec_liq_acc->Write("elec_liq_acc");
    h2_elec_liq_thr->Write("elec_liq_thr");
    h2_elec_liq_corr->Write("elec_liq_corr");

    //h_elec_sol_data->Write("elec_1d_sol_data");
    //h_elec_liq_data->Write("elec_1d_liq_data");

    //UNCORRECTED
    //Count number of corrected electrons and statistic errors
    Double_t n_elec_sol_data, n_elec_liq_data, n_elec_error_liq_data, n_elec_error_sol_data;
    n_elec_sol_data = h2_elec_sol_data->IntegralAndError(1, N_Nu,1, N_Q2, n_elec_error_sol_data);
    n_elec_liq_data = h2_elec_liq_data->IntegralAndError(1, N_Nu,1, N_Q2, n_elec_error_liq_data);

    //1D electron histograms for the MR normalization
    TH1D* h_elec_liq_data = new TH1D("h_elec_liq_data", "", N_main, main_bins[0], main_bins[N_main]);
    TH1D* h_elec_sol_data = new TH1D("h_elec_sol_data", "", N_main, main_bins[0], main_bins[N_main]);
    h_elec_liq_data->Sumw2();
    h_elec_sol_data->Sumw2();

    if (mainVar =="Zh" || mainVar == "Pt2" || mainVar == "Phi_PQ"){
        for (int i = 1; i <= N_main; i++) {
            h_elec_sol_data->SetBinContent(i, n_elec_sol_data);
            h_elec_liq_data->SetBinContent(i, n_elec_liq_data);
            h_elec_sol_data->SetBinError(i, n_elec_error_sol_data);
            h_elec_liq_data->SetBinError(i, n_elec_error_liq_data);
        }
    }

    //For electron varaible, project TH2 into TH1.
    //Nu histo
    else if (mainVar == "Nu"){
        h_elec_sol_data = (TH1D*)h2_elec_sol_data->ProjectionX("elec_nu_sol_data", 1 , N_Q2, "e");
        h_elec_liq_data = (TH1D*)h2_elec_liq_data->ProjectionX("elec_nu_liq_data", 1 , N_Q2, "e");
    }
    //Q2 histo
    else if (mainVar == "Q2"){
        h_elec_sol_data = (TH1D*)h2_elec_sol_data->ProjectionY("elec_q2_sol_data", 1 , N_Nu, "e");
        h_elec_liq_data = (TH1D*)h2_elec_liq_data->ProjectionY("elec_q2_liq_data", 1 , N_Nu, "e");
    }

    //Write both data and corrected electrons into output file
    h_elec_sol_data->Write("h1_elec_sol_data");
    h_elec_liq_data->Write("h1_elec_liq_data");
    h_elec_sol_corr->Write("h1_elec_sol_corr");
    h_elec_liq_corr->Write("h1_elec_liq_corr");

    TH1D* h_ebin_elec_liq_data[N_Nu][N_Q2];
    TH1D* h_ebin_elec_sol_data[N_Nu][N_Q2];
    TH1D* h_ebin_elec_liq_corr[N_Nu][N_Q2];
    TH1D* h_ebin_elec_sol_corr[N_Nu][N_Q2];   
    //For specific bin in Nu and Q2
    if (mainVar =="Zh" || mainVar == "Pt2" || mainVar == "Phi_PQ"){
        for (int NuCounter = 0; NuCounter < N_Nu; ++NuCounter){
            for (int Q2Counter = 0; Q2Counter < N_Q2; ++Q2Counter){
                //For hadronic variable, count number of corrected electrons and statistic errors
                n_elec_sol_data = h2_elec_sol_data->IntegralAndError(NuCounter+1,NuCounter+1,Q2Counter+1, Q2Counter+1, n_elec_error_sol_data);
                n_elec_liq_data = h2_elec_liq_data->IntegralAndError(NuCounter+1,NuCounter+1,Q2Counter+1, Q2Counter+1, n_elec_error_liq_data);
                n_elec_sol_corr = h2_elec_sol_data->IntegralAndError(NuCounter+1,NuCounter+1,Q2Counter+1, Q2Counter+1, n_elec_error_sol_corr);
                n_elec_liq_corr = h2_elec_liq_data->IntegralAndError(NuCounter+1,NuCounter+1,Q2Counter+1, Q2Counter+1, n_elec_error_liq_corr);

                //1D electron histograms for the MR normalization
                h_ebin_elec_liq_data[NuCounter][Q2Counter] = new TH1D(Form("h_elec_liq_data_%i_%i",NuCounter,Q2Counter), "", N_main, main_bins[0], main_bins[N_main]);
                h_ebin_elec_sol_data[NuCounter][Q2Counter] = new TH1D(Form("h_elec_sol_data_%i_%i",NuCounter,Q2Counter), "", N_main, main_bins[0], main_bins[N_main]);
                h_ebin_elec_liq_corr[NuCounter][Q2Counter] = new TH1D(Form("h_elec_liq_corr_%i_%i",NuCounter,Q2Counter), "", N_main, main_bins[0], main_bins[N_main]);
                h_ebin_elec_sol_corr[NuCounter][Q2Counter] = new TH1D(Form("h_elec_sol_corr_%i_%i",NuCounter,Q2Counter), "", N_main, main_bins[0], main_bins[N_main]);
                h_ebin_elec_liq_data[NuCounter][Q2Counter]->Sumw2();
                h_ebin_elec_sol_data[NuCounter][Q2Counter]->Sumw2();
                h_ebin_elec_liq_corr[NuCounter][Q2Counter]->Sumw2();
                h_ebin_elec_sol_corr[NuCounter][Q2Counter]->Sumw2();

                for (int i = 1; i <= N_main; i++) {
                    h_ebin_elec_sol_data[NuCounter][Q2Counter]->SetBinContent(i, n_elec_sol_data);
                    h_ebin_elec_liq_data[NuCounter][Q2Counter]->SetBinContent(i, n_elec_liq_data);
                    h_ebin_elec_sol_data[NuCounter][Q2Counter]->SetBinError(i, n_elec_error_sol_data);
                    h_ebin_elec_liq_data[NuCounter][Q2Counter]->SetBinError(i, n_elec_error_liq_data);
                    h_ebin_elec_sol_corr[NuCounter][Q2Counter]->SetBinContent(i, n_elec_sol_corr);
                    h_ebin_elec_liq_corr[NuCounter][Q2Counter]->SetBinContent(i, n_elec_liq_corr);
                    h_ebin_elec_sol_corr[NuCounter][Q2Counter]->SetBinError(i, n_elec_error_sol_corr);
                    h_ebin_elec_liq_corr[NuCounter][Q2Counter]->SetBinError(i, n_elec_error_liq_corr);
                }
            }
        }
        //DEBUG
        for (int NuCounter = 0; NuCounter < N_Nu; ++NuCounter){
            for (int Q2Counter = 0; Q2Counter < N_Q2; ++Q2Counter){
                h_ebin_elec_liq_data[NuCounter][Q2Counter]->Write();
                h_ebin_elec_sol_data[NuCounter][Q2Counter]->Write();
                h_ebin_elec_liq_corr[NuCounter][Q2Counter]->Write();
                h_ebin_elec_sol_corr[NuCounter][Q2Counter]->Write();
            }
        }
        //END OF DEBUG
    }

////////////////////////////////////////////////////////////////////////
//////////          MULTIPLICITY RATIO CALCULATION            //////////
////////////////////////////////////////////////////////////////////////
    cout<<"------------------------------------------------------------"<<endl;
    cout<<"Calculating multiplicity ratios"<<endl;

    //TH1 histograms for multiplicity ratio
    TH1D* h_mr_data = new TH1D("MR_data","MR_data", N_main, main_bins[0], main_bins[N_main]);
    TH1D* h_mr_corr = new TH1D("MR_corr","MR_corr", N_main, main_bins[0], main_bins[N_main]);
    h_mr_data->Sumw2();
    h_mr_corr->Sumw2();

    //Uncorrected MR
    h_liquid_data->Divide(h_liquid_data, h_elec_liq_data);
    h_solid_data->Divide(h_solid_data, h_elec_sol_data);
    h_mr_data->Divide(h_solid_data, h_liquid_data);
    h_mr_data->SetMarkerStyle(21);
    h_mr_data->Draw("COLZ");

    //Corrected MR
    h_liquid_corr->Divide(h_liquid_corr, h_elec_liq_corr);
    h_solid_corr->Divide(h_solid_corr, h_elec_sol_corr);
    h_mr_corr->Divide(h_solid_corr, h_liquid_corr);
    h_mr_corr->SetMarkerStyle(21);
    h_mr_corr->Draw("COLZ");

    //Save plots
    //data
    h_liquid_data->Write("ratio_liquid_data");
    h_solid_data->Write("ratio_solid_data");
    h_mr_data->Write("MR_data");
    //corrected
    h_liquid_corr->Write("ratio_liquid_corr");
    h_solid_corr->Write("ratio_solid_corr");
    h_mr_corr->Write("MR_corr");

    //For specific bin in Nu and Q2
    TH1D* h_ebin_mr_data[N_Nu][N_Q2];
    TH1D* h_ebin_mr_corr[N_Nu][N_Q2];

    if (mainVar =="Zh" || mainVar == "Pt2" || mainVar == "Phi_PQ"){
        for (int NuCounter = 0; NuCounter < N_Nu; ++NuCounter){
            for (int Q2Counter = 0; Q2Counter < N_Q2; ++Q2Counter){
                cout<<"Working on bin Nu:"<<NuCounter<<", Q2:"<<Q2Counter<<endl;

                h_ebin_mr_data[NuCounter][Q2Counter] = new TH1D(Form("MR_data_%i_%i",NuCounter,Q2Counter),"MR_data", N_main, main_bins[0], main_bins[N_main]);
                h_ebin_mr_corr[NuCounter][Q2Counter] = new TH1D(Form("MR_corr_%i_%i",NuCounter,Q2Counter),"MR_corr", N_main, main_bins[0], main_bins[N_main]);
                h_ebin_mr_data[NuCounter][Q2Counter]->Sumw2();
                h_ebin_mr_corr[NuCounter][Q2Counter]->Sumw2();

                //Uncorrected MR
                h_ebin_liquid_data[NuCounter][Q2Counter]->Divide(h_ebin_liquid_data[NuCounter][Q2Counter], h_ebin_elec_liq_data[NuCounter][Q2Counter]);
                h_ebin_solid_data[NuCounter][Q2Counter]->Divide(h_ebin_solid_data[NuCounter][Q2Counter], h_ebin_elec_sol_data[NuCounter][Q2Counter]);
                h_ebin_mr_data[NuCounter][Q2Counter]->Divide(h_ebin_solid_data[NuCounter][Q2Counter], h_ebin_liquid_data[NuCounter][Q2Counter]);
                h_ebin_mr_data[NuCounter][Q2Counter]->SetMarkerStyle(21);
                h_ebin_mr_data[NuCounter][Q2Counter]->Draw("COLZ");
//
                //Corrected MR
                h_ebin_liquid_corr[NuCounter][Q2Counter]->Divide(h_ebin_liquid_corr[NuCounter][Q2Counter], h_ebin_elec_liq_corr[NuCounter][Q2Counter]);
                h_ebin_solid_corr[NuCounter][Q2Counter]->Divide(h_ebin_solid_corr[NuCounter][Q2Counter], h_ebin_elec_sol_corr[NuCounter][Q2Counter]);
                h_ebin_mr_corr[NuCounter][Q2Counter]->Divide(h_ebin_solid_corr[NuCounter][Q2Counter], h_ebin_liquid_corr[NuCounter][Q2Counter]);
                h_ebin_mr_corr[NuCounter][Q2Counter]->SetMarkerStyle(21);
                h_ebin_mr_corr[NuCounter][Q2Counter]->Draw("COLZ");
//
                //Save plots
                //data
                h_ebin_liquid_data[NuCounter][Q2Counter]->Write(Form("ratio_liquid_data_%i_%i",NuCounter,Q2Counter));
                h_ebin_solid_data[NuCounter][Q2Counter]->Write(Form("ratio_solid_data_%i_%i",NuCounter,Q2Counter));
                h_ebin_mr_data[NuCounter][Q2Counter]->Write(Form("MR_data_%i_%i",NuCounter,Q2Counter));
                //corrected
                h_ebin_liquid_corr[NuCounter][Q2Counter]->Write(Form("ratio_liquid_corr_%i_%i",NuCounter,Q2Counter));
                h_ebin_solid_corr[NuCounter][Q2Counter]->Write(Form("ratio_solid_corr_%i_%i",NuCounter,Q2Counter));
                h_ebin_mr_corr[NuCounter][Q2Counter]->Write(Form("MR_corr_%i_%i",NuCounter,Q2Counter));
            }
        }
    }

	output->Close();
}

/*
TODO
-->Determine what to do with empty bins
-->Use real simulations
*/