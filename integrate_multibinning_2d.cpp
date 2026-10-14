#include "include.h"

void integrate_multibinning_2d(TString Target="C", int Hadron_pid=211, TString mainVar="Zh", TString secVar="Pt2"){
    ROOT::EnableImplicitMT(); //To run with multithreads (idk if it works)

////////////////////////////////////////////////////////////////////////
//////////                  SETTING VARIABLES                 //////////
////////////////////////////////////////////////////////////////////////
    cout<<"Setting variables"<<endl;

    //Variables to use. The calculation is for the first variable in the list, called MainVar
    int N_main, N_sec;
    float *main_bins = nullptr;
    float *sec_bins = nullptr;

    //N_main and main_bins correspond to the number of bins and their limits respectively
    if (mainVar == "Zh" && secVar=="Pt2"){
        N_main = N_Zh;
        main_bins = Zh_bins;
        N_sec = N_Pt2;
        sec_bins = Pt2_bins;
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
    //Get hadron and electron histograms from root file created with create_multibinning
    TFile *input_data = new TFile("output_"+fwd_rec+"/"+torus_pol+"/data/"+detector+"/"+Target+"/data_binned_"+hadron+".root","READ");

/*
    //ACCEPTED
    //hadrons
    TFile *input_acc_sol = new TFile("output_"+fwd_rec+"/simul/"+detector+"/"+Target+"/data_binned_"+hadron+".root","READ");
    TFile *input_acc_liq = new TFile("output_"+fwd_rec+"/simul/D2/data_binned_"+hadron+".root","READ");

    //THROWN
    //hadrons
    TFile *input_thr_sol = new TFile("output_"+fwd_rec+"/simul/"+detector+"/"+Target+"/thrown/data_binned_"+hadron+".root","READ");
    TFile *input_thr_liq = new TFile("output_"+fwd_rec+"/simul/D2/thrown/data_binned_"+hadron+".root","READ");
*/
    
//ONLY FOR TESTING, USING DATA FILES AS SIMULATIONS
//DELETE THIS BLOCK WHEN USING REAL SIMULATIONS AND UNCOMMENT LAST BLOCK

    //ACCEPTED
    //hadrons
    TFile *input_acc_sol = new TFile("output_"+fwd_rec+"/"+torus_pol+"/data/"+detector+"/"+Target+"/data_binned_"+hadron+".root","READ");
    TFile *input_acc_liq = new TFile("output_"+fwd_rec+"/"+torus_pol+"/data/"+detector+"/"+Target+"/data_binned_"+hadron+".root","READ");

    //THROWN
    //hadrons
    TFile *input_thr_sol = new TFile("output_"+fwd_rec+"/"+torus_pol+"/data/"+detector+"/"+Target+"/data_binned_"+hadron+".root","READ");
    TFile *input_thr_liq = new TFile("output_"+fwd_rec+"/"+torus_pol+"/data/"+detector+"/"+Target+"/data_binned_"+hadron+".root","READ");

//END OF BLOCK TO BE DELETED WHEN SIMUALTIONS ARE COMPLETED

    //OUTPUT file
    TString output_dir = "output_"+fwd_rec+"/"+torus_pol+"/AC_MR/"+detector+"/"+Target;
    std::string command = std::string("mkdir -p ") + output_dir.Data();
    gSystem->Exec(command.c_str());
    TFile *output = new TFile(output_dir+"/mr_"+mainVar+"_"+secVar+"_"+hadron+".root","RECREATE");

////////////////////////////////////////////////////////////////////////
//////////            HADRON ACEPTANCE CORRECTION             //////////
////////////////////////////////////////////////////////////////////////
    cout<<"------------------------------------------------------------"<<endl;
    cout<<"Calculating hadron acceptance correction"<<endl;

    //Array of histograms to integrate entries from input root file
    //Uncorrected
    TH2D* h2_integ_liq_data[N_main][N_sec];
    TH2D* h2_integ_sol_data[N_main][N_sec];
    //Aceptance Corrected
    TH2D* h2_integ_liq_corr[N_main][N_sec];
    TH2D* h2_integ_sol_corr[N_main][N_sec];

    //TH2D needed later
    TH2D *h2_liq_data, *h2_liq_acc, *h2_liq_thr, *h2_liq_corr, *h2_liq_correction;
    TH2D *h2_sol_data, *h2_sol_acc, *h2_sol_thr, *h2_sol_corr, *h2_sol_correction;


    //Loop every bin for every variable
    for (int mainVarCounter = 0; mainVarCounter < N_main; mainVarCounter++) {
        for (int secVarCounter = 0; secVarCounter < N_sec; secVarCounter++) {
            //Create integration histogram for each main bin to save in the array
            h2_integ_liq_data[mainVarCounter][secVarCounter] = new TH2D(Form("Integrated_histo_liq_data_%i_%i",
                                    mainVarCounter, secVarCounter), "Integrated histo liq uncorr",
                                    N_Nu, Nu_bins[0], Nu_bins[N_Nu],N_Q2, Q2_bins[0], Q2_bins[N_Q2]);

            h2_integ_sol_data[mainVarCounter][secVarCounter] = new TH2D(Form("Integrated_histo_sol_data_%i_%i",
                                    mainVarCounter, secVarCounter), "Integrated histo sol uncorr",
                                    N_Nu, Nu_bins[0], Nu_bins[N_Nu],N_Q2, Q2_bins[0], Q2_bins[N_Q2]);

            h2_integ_liq_corr[mainVarCounter][secVarCounter] = new TH2D(Form("Integrated_histo_liq_corr_%i_%i",
                                    mainVarCounter, secVarCounter), "Integrated histo liq corr",
                                    N_Nu, Nu_bins[0], Nu_bins[N_Nu],N_Q2, Q2_bins[0], Q2_bins[N_Q2]);

            h2_integ_sol_corr[mainVarCounter][secVarCounter] = new TH2D(Form("Integrated_histo_sol_corr_%i_%i",
                                    mainVarCounter, secVarCounter), "Integrated histo sol corr",
                                    N_Nu, Nu_bins[0], Nu_bins[N_Nu],N_Q2, Q2_bins[0], Q2_bins[N_Q2]);
            //Error propagation
            h2_integ_liq_data[mainVarCounter][secVarCounter]->Sumw2();
            h2_integ_sol_data[mainVarCounter][secVarCounter]->Sumw2();
            h2_integ_liq_corr[mainVarCounter][secVarCounter]->Sumw2();
            h2_integ_sol_corr[mainVarCounter][secVarCounter]->Sumw2();
        }
    }

    //Loop over hadronic variables
    int mainVarCounter, secVarCounter;
    for (int ZhCounter = 0; ZhCounter < N_Zh; ZhCounter++) {
        for (int Pt2Counter = 0; Pt2Counter < N_Pt2; Pt2Counter++) {
            for (int PhiCounter = 0; PhiCounter < N_Phi; PhiCounter++) {
                //Assign names for each var for getting the histo names
                if (mainVar == "Zh" && secVar=="Pt2"){
                    mainVarCounter = ZhCounter;
                    secVarCounter = Pt2Counter;
                }

                //Obtain histos from file
                cout<<"********************* New hadron BIN *******************"<<endl;
                cout<<"Getting histos: "<<Form("%i_%i_%i", ZhCounter, Pt2Counter, PhiCounter)<<endl;
                //Data histos
                h2_liq_data = (TH2D*)input_data->Get(Form("Data_liq_%i_%i_%i", ZhCounter, Pt2Counter, PhiCounter));
                h2_sol_data = (TH2D*)input_data->Get(Form("Data_sol_%i_%i_%i", ZhCounter, Pt2Counter, PhiCounter));
                //Accepted histos
                h2_liq_acc = (TH2D*)input_acc_liq->Get(Form("Data_liq_%i_%i_%i", ZhCounter, Pt2Counter, PhiCounter));
                h2_sol_acc = (TH2D*)input_acc_sol->Get(Form("Data_sol_%i_%i_%i", ZhCounter, Pt2Counter, PhiCounter));
                //Thrown histos
                h2_liq_thr = (TH2D*)input_thr_liq->Get(Form("Data_liq_%i_%i_%i", ZhCounter, Pt2Counter, PhiCounter));
                h2_sol_thr = (TH2D*)input_thr_sol->Get(Form("Data_sol_%i_%i_%i", ZhCounter, Pt2Counter, PhiCounter));

                //acceptance correction factors calculation
                //LIQUID TARGET
                h2_liq_corr = (TH2D*)h2_liq_data->Clone();
                h2_liq_correction = (TH2D*)h2_liq_thr->Clone();
                h2_liq_corr->Sumw2();
                h2_liq_correction->Sumw2();
                h2_liq_correction->Divide(h2_liq_acc);
                h2_liq_corr->Multiply(h2_liq_correction);

                h2_integ_liq_corr[mainVarCounter][secVarCounter]->Add(h2_liq_corr);

                //SOLID TARGET
                h2_sol_corr = (TH2D*)h2_sol_data->Clone();
                h2_sol_correction = (TH2D*)h2_sol_thr->Clone();
                h2_sol_corr->Sumw2();
                h2_sol_correction->Sumw2();
                h2_sol_correction->Divide(h2_sol_acc);
                h2_sol_corr->Multiply(h2_sol_correction);

                h2_integ_sol_corr[mainVarCounter][secVarCounter]->Add(h2_sol_corr);

                //*** check what to do if the histogram is empty ***
                // maybe just add the uncorrected into the corrected 

                //Add uncorrected histos to integrated histograms
                cout<<"adding data liq histo with integral:"<<h2_liq_data->Integral()<<endl;
                cout<<"adding data sol histo with integral:"<<h2_sol_data->Integral()<<endl;

                h2_integ_liq_data[mainVarCounter][secVarCounter]->Add(h2_liq_data);
                h2_integ_sol_data[mainVarCounter][secVarCounter]->Add(h2_sol_data);

            }
        }
    }
    cout<<"------------------------------------------------------------"<<endl;
    cout<<"Ended looping over hadron bins"<<endl;

    //Open uptput file to save root stuff in it
    output->cd();

    //Final histograms for main var histograms NON corrected and corrected
    TH1D* h_liquid_data[N_sec];
    TH1D* h_solid_data[N_sec];
    TH1D* h_liquid_corr[N_sec];
    TH1D* h_solid_corr[N_sec];

    for (int secVarCounter = 0; secVarCounter < N_sec; secVarCounter++) {
        h_liquid_data[secVarCounter] = new TH1D(Form("histo_liq_%i",secVarCounter),"histo liq", N_main, main_bins[0], main_bins[N_main]);
        h_solid_data[secVarCounter] = new TH1D(Form("histo_sol_%i",secVarCounter),"histo sol", N_main, main_bins[0], main_bins[N_main]);
        h_liquid_corr[secVarCounter] = new TH1D(Form("histo_liq_corr_%i",secVarCounter),"histo liq corr", N_main, main_bins[0], main_bins[N_main]);
        h_solid_corr[secVarCounter] = new TH1D(Form("histo_sol_corr_%i",secVarCounter),"histo sol corr", N_main, main_bins[0], main_bins[N_main]);

        //Error propagation
        h_liquid_data[secVarCounter]->Sumw2();
        h_solid_data[secVarCounter]->Sumw2();
        h_liquid_corr[secVarCounter]->Sumw2();
        h_solid_corr[secVarCounter]->Sumw2();
    }

    //Variables to save number of corrected hadrons and statistic errors
    Double_t error_liq_data, error_sol_data, error_sol_corr, error_liq_corr;

    //Set bin values and error using integrated histogram for each main var bin
    for (int mainVarCounter = 0; mainVarCounter < N_main; mainVarCounter++) {
        for (int secVarCounter = 0; secVarCounter < N_sec; secVarCounter++) {
            //Uncorrected

            h_liquid_data[secVarCounter]->SetBinContent(mainVarCounter+1, h2_integ_liq_data[mainVarCounter][secVarCounter]->IntegralAndError(1,N_Nu,1, N_Q2, error_liq_data));
            h_solid_data[secVarCounter] ->SetBinContent(mainVarCounter+1, h2_integ_sol_data[mainVarCounter][secVarCounter]->IntegralAndError(1,N_Nu,1, N_Q2, error_sol_data));
            h_liquid_data[secVarCounter]->SetBinError(mainVarCounter+1, error_liq_data);
            h_solid_data[secVarCounter] ->SetBinError(mainVarCounter+1, error_sol_data);
            h2_integ_liq_data[mainVarCounter][secVarCounter]->Write(Form("NuxQ2_histo_liq_%i_%i", mainVarCounter, secVarCounter));
            h2_integ_sol_data[mainVarCounter][secVarCounter]->Write(Form("NuxQ2_histo_sol_%i_%i", mainVarCounter, secVarCounter));

            //Corrected
            h_liquid_corr[secVarCounter]->SetBinContent(mainVarCounter+1, h2_integ_liq_corr[mainVarCounter][secVarCounter]->IntegralAndError(1,N_Nu,1, N_Q2, error_liq_corr));
            h_solid_corr[secVarCounter] ->SetBinContent(mainVarCounter+1, h2_integ_sol_corr[mainVarCounter][secVarCounter]->IntegralAndError(1,N_Nu,1, N_Q2, error_sol_corr));
            h_liquid_corr[secVarCounter]->SetBinError(mainVarCounter+1, error_liq_corr);
            h_solid_corr[secVarCounter] ->SetBinError(mainVarCounter+1, error_sol_corr);
            h2_integ_liq_corr[mainVarCounter][secVarCounter]->Write(Form("NuxQ2_histo_liq_%i_%i_corr", mainVarCounter, secVarCounter));
            h2_integ_sol_corr[mainVarCounter][secVarCounter]->Write(Form("NuxQ2_histo_sol_%i_%i_corr", mainVarCounter, secVarCounter));
        }
    }

////////////////////////////////////////////////////////////////////////
//////////           ELECTRON ACEPTANCE CORRECTION            //////////
////////////////////////////////////////////////////////////////////////
    cout<<"------------------------------------------------------------"<<endl;
    cout<<"Calculating electron acceptance correction"<<endl;

    //Get 2D electron plots from file
    TH2D* h2_elec_sol_data = (TH2D*)input_data->Get("h2_elec_sol");
    TH2D* h2_elec_liq_data = (TH2D*)input_data->Get("h2_elec_liq");
    TH2D* h2_elec_sol_acc  = (TH2D*)input_acc_sol->Get("h2_elec_sol");
    TH2D* h2_elec_liq_acc  = (TH2D*)input_acc_liq->Get("h2_elec_liq");
    TH2D* h2_elec_sol_thr  = (TH2D*)input_thr_sol->Get("h2_elec_sol");
    TH2D* h2_elec_liq_thr  = (TH2D*)input_thr_liq->Get("h2_elec_liq");

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
    for (int i = 1; i <= N_main; i++) {
        h_elec_sol_corr->SetBinContent(i, n_elec_sol_corr);
        h_elec_liq_corr->SetBinContent(i, n_elec_liq_corr);
        h_elec_sol_corr->SetBinError(i, n_elec_error_sol_corr);
        h_elec_liq_corr->SetBinError(i, n_elec_error_liq_corr);
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

    for (int i = 1; i <= N_main; i++) {
        h_elec_sol_data->SetBinContent(i, n_elec_sol_data);
        h_elec_liq_data->SetBinContent(i, n_elec_liq_data);
        h_elec_sol_data->SetBinError(i, n_elec_error_sol_data);
        h_elec_liq_data->SetBinError(i, n_elec_error_liq_data);
    }

    //Write both data and corrected electrons into output file
    h_elec_sol_data->Write("h1_elec_sol_data");
    h_elec_liq_data->Write("h1_elec_liq_data");
    h_elec_sol_corr->Write("h1_elec_sol_corr");
    h_elec_liq_corr->Write("h1_elec_liq_corr");

////////////////////////////////////////////////////////////////////////
//////////          MULTIPLICITY RATIO CALCULATION            //////////
////////////////////////////////////////////////////////////////////////
    cout<<"------------------------------------------------------------"<<endl;
    cout<<"Calculating multiplicity ratios"<<endl;

    //TH1 histograms for multiplicity ratio
    TH1D* h_mr_data[N_sec];
    TH1D* h_mr_corr[N_sec];
    for (int secVarCounter = 0; secVarCounter < N_sec; secVarCounter++) {
        h_mr_data[secVarCounter] = new TH1D(Form("MR_data_%i", secVarCounter),"MR_data", N_main, main_bins[0], main_bins[N_main]);
        h_mr_corr[secVarCounter] = new TH1D(Form("MR_corr_%i", secVarCounter),"MR_corr", N_main, main_bins[0], main_bins[N_main]);
        h_mr_data[secVarCounter]->Sumw2();
        h_mr_corr[secVarCounter]->Sumw2();

        //Uncorrected MR
        h_liquid_data[secVarCounter]->Divide(h_liquid_data[secVarCounter], h_elec_liq_data);
        h_solid_data[secVarCounter]->Divide(h_solid_data[secVarCounter], h_elec_sol_data);
        h_mr_data[secVarCounter]->Divide(h_solid_data[secVarCounter], h_liquid_data[secVarCounter]);
        h_mr_data[secVarCounter]->SetMarkerStyle(21);
        h_mr_data[secVarCounter]->Draw("COLZ");

        //Corrected MR
        h_liquid_corr[secVarCounter]->Divide(h_liquid_corr[secVarCounter], h_elec_liq_corr);
        h_solid_corr[secVarCounter]->Divide(h_solid_corr[secVarCounter], h_elec_sol_corr);
        h_mr_corr[secVarCounter]->Divide(h_solid_corr[secVarCounter], h_liquid_corr[secVarCounter]);
        h_mr_corr[secVarCounter]->SetMarkerStyle(21);
        h_mr_corr[secVarCounter]->Draw("COLZ");

        //Save plots
        //data
        h_liquid_data[secVarCounter]->Write(Form("ratio_liquid_data_%i", secVarCounter));
        h_solid_data[secVarCounter]->Write(Form("ratio_solid_data_%i", secVarCounter));
        h_mr_data[secVarCounter]->Write(Form("MR_data_%i", secVarCounter));
        //corrected
        h_liquid_corr[secVarCounter]->Write(Form("ratio_liquid_corr_%i", secVarCounter));
        h_solid_corr[secVarCounter]->Write(Form("ratio_solid_corr_%i", secVarCounter));
        h_mr_corr[secVarCounter]->Write(Form("MR_corr_%i", secVarCounter));
    }
	output->Close();
}

/*
TODO
-->Determine what to do with empty bins
-->Use real simulations
*/