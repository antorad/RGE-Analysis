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

    //DATA
    //Get histgrams from output root file
    TFile *output = new TFile("output/"+Target+"/data_binned.root","UPDATE");
    //Get electron TNtuple input created from simple_plots
    TFile *input = new TFile("output/"+Target+"/out_clas12.root","READ");
    TNtuple* elec_tuple = (TNtuple*)input->Get("elec_tuple");

    //ACC (Data from other target for now)
    TFile *output_acc = new TFile("output/C/data_binned.root","UPDATE");
    //Get electron TNtuple input created from simple_plots
    TFile *input_acc = new TFile("output/C/out_clas12.root","READ");
    TNtuple* elec_tuple_acc = (TNtuple*)input->Get("elec_tuple");

    //THROWN (Data from another target for now)
    TFile *output_thr = new TFile("output/C/data_binned.root","UPDATE");
    //Get electron TNtuple input created from simple_plots
    TFile *input_thr = new TFile("output/C/out_clas12.root","READ");
    TNtuple* elec_tuple_thr = (TNtuple*)input->Get("elec_tuple");

    //Create var cuts
    TCut Q2_Cut, Nu_Cut, Zh_Cut, Pt2_Cut, total_cut;

    //Creating histograms to integrate entries
    //Uncorrected
    TH1F* Integ_liq[10];
    TH1F* Integ_sol[10];
    //Corrected
    TH1F* Integ_liq_corr[10];
    TH1F* Integ_sol_corr[10];
    //TH1F used
    TH1F *h_liq, *h_sol, *h_liq_acc, *h_sol_acc, *h_liq_thr, *h_sol_thr, *h_liq_corr, *h_sol_corr, *liq_correction, *sol_correction;

    //TH2F for electrons
    //Draw the 2D electron plots
    elec_tuple->Draw(Form("Q2:nu>>h_elec_sol(%i,%f,%f,%i,%f,%f)",N_Q2,Q2_bins[0],Q2_bins[N_Q2],N_Nu,Nu_bins[0],Nu_bins[N_Nu]), Main_cut&&vz_solid, "goff");
    elec_tuple->Draw(Form("Q2:nu>>h_elec_liq(%i,%f,%f,%i,%f,%f)",N_Q2,Q2_bins[0],Q2_bins[N_Q2],N_Nu,Nu_bins[0],Nu_bins[N_Nu]), Main_cut&&vz_d2, "goff");
    elec_tuple_acc->Draw(Form("Q2:nu>>h_elec_sol_acc(%i,%f,%f,%i,%f,%f)",N_Q2,Q2_bins[0],Q2_bins[N_Q2],N_Nu,Nu_bins[0],Nu_bins[N_Nu]), Main_cut&&vz_solid, "goff");
    elec_tuple_acc->Draw(Form("Q2:nu>>h_elec_liq_acc(%i,%f,%f,%i,%f,%f)",N_Q2,Q2_bins[0],Q2_bins[N_Q2],N_Nu,Nu_bins[0],Nu_bins[N_Nu]), Main_cut&&vz_d2, "goff");
    elec_tuple_thr->Draw(Form("Q2:nu>>h_elec_sol_thr(%i,%f,%f,%i,%f,%f)",N_Q2,Q2_bins[0],Q2_bins[N_Q2],N_Nu,Nu_bins[0],Nu_bins[N_Nu]), Main_cut&&vz_solid, "goff");
    elec_tuple_thr->Draw(Form("Q2:nu>>h_elec_liq_thr(%i,%f,%f,%i,%f,%f)",N_Q2,Q2_bins[0],Q2_bins[N_Q2],N_Nu,Nu_bins[0],Nu_bins[N_Nu]), Main_cut&&vz_d2, "goff");
    //Save the plots to access them later
    TH2F *h_elec_sol = (TH2F*)gDirectory->Get("h_elec_sol");
    TH2F *h_elec_liq = (TH2F*)gDirectory->Get("h_elec_liq");
    TH2F *h_elec_sol_acc = (TH2F*)gDirectory->Get("h_elec_sol_acc");
    TH2F *h_elec_liq_acc = (TH2F*)gDirectory->Get("h_elec_liq_acc");
    TH2F *h_elec_sol_thr = (TH2F*)gDirectory->Get("h_elec_sol_thr");
    TH2F *h_elec_liq_thr = (TH2F*)gDirectory->Get("h_elec_liq_thr");

    //Initialize variable to store number of electrons per bin.
    int N_elec_sol, N_elec_liq, N_elec_sol_acc, N_elec_liq_acc, N_elec_sol_thr, N_elec_liq_thr;

    //Loop every bin for every variable (main var first)
    for (int mainVarCounter = 0; mainVarCounter < N_main; mainVarCounter++) {
        //Create integration for each main bin
        Integ_liq[mainVarCounter] = new TH1F(Form("Integrated_histo_liq_%i", mainVarCounter), "Integrated histo liq", 12, -3.1416, 3.1416);
        Integ_sol[mainVarCounter] = new TH1F(Form("Integrated_histo_sol_%i", mainVarCounter), "Integrated histo sol", 12, -3.1416, 3.1416);
        Integ_liq_corr[mainVarCounter] = new TH1F(Form("Integrated_histo_liq_corr_%i", mainVarCounter), "Integrated histo liq", 12, -3.1416, 3.1416);
        Integ_sol_corr[mainVarCounter] = new TH1F(Form("Integrated_histo_sol_corr_%i", mainVarCounter), "Integrated histo sol", 12, -3.1416, 3.1416);
        //Loop over remaining variables
        for (int Q2Counter = 0; Q2Counter < N_Q2; Q2Counter++) {
            for (int NuCounter = 0; NuCounter < N_Nu; NuCounter++) {
                N_elec_sol = h_elec_sol->GetBinContent(Q2Counter, NuCounter);
                N_elec_liq = h_elec_liq->GetBinContent(Q2Counter, NuCounter);
                N_elec_sol_acc = h_elec_sol->GetBinContent(Q2Counter, NuCounter);
                N_elec_liq_acc = h_elec_sol->GetBinContent(Q2Counter, NuCounter);
                N_elec_sol_thr = h_elec_sol->GetBinContent(Q2Counter, NuCounter);
                N_elec_liq_thr = h_elec_sol->GetBinContent(Q2Counter, NuCounter);

                cout<<"number of electrons: "<<N_elec_sol<<" in bin "<<Q2Counter<<"_"<<NuCounter<<endl;

                for (int Pt2Counter = 0; Pt2Counter < N_Pt2; Pt2Counter++) {
                    //Obtain histos from file
                    cout<<"Getting histos: "<< Form("%i_%i_%i_%i", Q2Counter, NuCounter, mainVarCounter, Pt2Counter)<<endl;
                    //Data histos
                    h_liq = (TH1F*)output->Get(Form("Data_liq_%i_%i_%i_%i", Q2Counter, NuCounter, mainVarCounter, Pt2Counter));
                    h_sol = (TH1F*)output->Get(Form("Data_sol_%i_%i_%i_%i", Q2Counter, NuCounter, mainVarCounter, Pt2Counter));
                    //Accepted histos
                    h_liq_acc = (TH1F*)output_acc->Get(Form("Data_liq_%i_%i_%i_%i", Q2Counter, NuCounter, mainVarCounter, Pt2Counter));
                    h_sol_acc = (TH1F*)output_acc->Get(Form("Data_sol_%i_%i_%i_%i", Q2Counter, NuCounter, mainVarCounter, Pt2Counter));
                    //Thrown histos
                    h_liq_thr = (TH1F*)output_thr->Get(Form("Data_liq_%i_%i_%i_%i", Q2Counter, NuCounter, mainVarCounter, Pt2Counter));
                    h_sol_thr = (TH1F*)output_thr->Get(Form("Data_sol_%i_%i_%i_%i", Q2Counter, NuCounter, mainVarCounter, Pt2Counter));
                    //acceptance correction factors calculation
                    if (h_liq!=NULL && h_liq_acc!=NULL && h_liq_thr!=NULL){
                        h_liq_corr = (TH1F*)h_liq->Clone();
                        liq_correction = (TH1F*)h_liq_thr->Clone();
                        liq_correction->Divide(h_liq_acc);
                        h_liq_corr->Multiply(liq_correction);
                        cout<<h_liq_corr->Integral()<<endl;

                       //save corrected histogram if it is not empty
                        if (h_liq_corr->Integral()!=0){
                            cout<<"inside the if  with integral:"<<h_liq_corr->Integral()<<endl; 
                            Integ_liq_corr[mainVarCounter]->Add(h_liq_corr);
                            cout<<"after the adding "<<endl; 
                            //Integ_sol_corr[mainVarCounter]->Add(h_sol_corr); 
                        }
                        //if it is empty save the original in the corrected one anyways
                        else{
                            Integ_liq_corr[mainVarCounter]->Add(h_liq);
                        }
                    }
                    if (h_sol!=NULL && h_sol_acc!=NULL && h_sol_thr!=NULL){
                        h_sol_corr = (TH1F*)h_sol->Clone();
                        sol_correction = (TH1F*)h_sol_thr->Clone();
                        sol_correction->Divide(h_sol_acc);
                        h_sol_corr->Multiply(sol_correction);
                        cout<<h_sol_corr->Integral()<<endl;

                       //save corrected histogram if it is not empty
                        if (h_sol_corr->Integral()!=0){
                            cout<<"inside the if with integral:"<<h_sol_corr->Integral()<<endl; 
                            Integ_sol_corr[mainVarCounter]->Add(h_sol_corr);
                            cout<<"after the adding "<<endl; 
                            //Integ_sol_corr[mainVarCounter]->Add(h_sol_corr); 
                        }
                        //if it is empty save the original in the corrected one anyways
                        else{
                            Integ_sol_corr[mainVarCounter]->Add(h_sol);
                        }
                    }

                    //Add histos to integrated histograms
                    Integ_liq[mainVarCounter]->Add(h_liq);
                    Integ_sol[mainVarCounter]->Add(h_sol);
                }
            }
        }
    }
    output->cd();

    //Main var histograms non corrected
    TH1F* histo_liquid = new TH1F(" histo liq"," histo liq", N_main, main_bins[0], main_bins[N_main]);
    TH1F* histo_solid = new TH1F(" histo sol"," histo sol", N_main, main_bins[0], main_bins[N_main]);
    TH1F* mr = new TH1F("MR","MR", N_main, main_bins[0], main_bins[N_main]);
    //corrected
    TH1F* histo_liquid_corr = new TH1F(" histo liq corr"," histo liq corr", N_main, main_bins[0], main_bins[N_main]);
    TH1F* histo_solid_corr = new TH1F(" histo sol corr"," histo sol corr", N_main, main_bins[0], main_bins[N_main]);
    TH1F* mr_corr = new TH1F("MR_corr","MR_corr", N_main, main_bins[0], main_bins[N_main]);

    //Error propagation
    histo_liquid->Sumw2();
    histo_solid->Sumw2();
    mr->Sumw2();
    histo_liquid_corr->Sumw2();
    histo_solid_corr->Sumw2();
    mr_corr->Sumw2();
    Double_t error_liq, error_sol, error_sol_corr, error_liq_corr;

    //Set bin values and error using integrated histogram for each main var bin
    for (int mainVarCounter = 0; mainVarCounter < N_main; mainVarCounter++) {
        histo_liquid->SetBinContent(mainVarCounter+1, Integ_liq[mainVarCounter]->IntegralAndError(1,10, error_liq));
        histo_solid->SetBinContent(mainVarCounter+1, Integ_sol[mainVarCounter]->IntegralAndError(1,10, error_sol));
        histo_liquid->SetBinError(mainVarCounter+1, error_liq);
        histo_solid->SetBinError(mainVarCounter+1, error_sol);
        Integ_liq[mainVarCounter]->Write(Form("phiPQ_histo_liq_%i", mainVarCounter), TObject::kOverwrite);
        Integ_sol[mainVarCounter]->Write(Form("phiPQ_histo_sol_%i", mainVarCounter), TObject::kOverwrite);

        histo_liquid_corr->SetBinContent(mainVarCounter+1, Integ_liq_corr[mainVarCounter]->IntegralAndError(1,10, error_liq_corr));
        histo_solid_corr->SetBinContent(mainVarCounter+1, Integ_sol_corr[mainVarCounter]->IntegralAndError(1,10, error_sol_corr));
        histo_liquid_corr->SetBinError(mainVarCounter+1, error_liq_corr);
        histo_solid_corr->SetBinError(mainVarCounter+1, error_sol_corr);
        Integ_liq_corr[mainVarCounter]->Write(Form("phiPQ_histo_liq_%i_corr", mainVarCounter), TObject::kOverwrite);
        Integ_sol_corr[mainVarCounter]->Write(Form("phiPQ_histo_sol_%i_corr", mainVarCounter), TObject::kOverwrite);
    }

    //Counting of the number of electron in each target by making an hist and counting entries
    //data
    int n_e_liq = elec_tuple->Draw("v_z>>h_e_liq", Main_cut&&vz_d2, "goff");
    int n_e_sol = elec_tuple->Draw("v_z>>h_e_sol", Main_cut&&vz_solid, "goff");
    //acc
    int n_e_liq_acc = elec_tuple_acc->Draw("v_z>>h_e_liq", Main_cut&&vz_d2, "goff");
    int n_e_sol_acc = elec_tuple_acc->Draw("v_z>>h_e_sol", Main_cut&&vz_solid, "goff");
    //thr
    int n_e_liq_thr = elec_tuple_thr->Draw("v_z>>h_e_liq", Main_cut&&vz_d2, "goff");
    int n_e_sol_thr = elec_tuple_thr->Draw("v_z>>h_e_sol", Main_cut&&vz_solid, "goff");

    //Convert number of electron into flat histogram.
    //data
    TH1F* elec_hist_liq = new TH1F("elec_hist_liq", "", 10, 0, 1);
    TH1F* elec_hist_sol = new TH1F("elec_hist_sol", "", 10, 0, 1);
    //acc
    TH1F* elec_hist_liq_acc = new TH1F("elec_hist_liq_acc", "", 10, 0, 1);
    TH1F* elec_hist_sol_acc = new TH1F("elec_hist_sol_acc", "", 10, 0, 1);
    //thr
    TH1F* elec_hist_liq_thr = new TH1F("elec_hist_liq_thr", "", 10, 0, 1);
    TH1F* elec_hist_sol_thr = new TH1F("elec_hist_sol_thr", "", 10, 0, 1);
    //corr
    TH1F* elec_hist_liq_corr = new TH1F("elec_hist_liq_corr", "", 10, 0, 1);
    TH1F* elec_hist_sol_corr = new TH1F("elec_hist_sol_corr", "", 10, 0, 1);
    for (int i = 1; i <= 10; i++) {
        elec_hist_liq->SetBinContent(i, n_e_liq);
        elec_hist_sol->SetBinContent(i, n_e_sol);
        elec_hist_liq_acc->SetBinContent(i, n_e_liq_acc);
        elec_hist_sol_acc->SetBinContent(i, n_e_sol_acc);
        elec_hist_liq_thr->SetBinContent(i, n_e_liq_thr);
        elec_hist_sol_thr->SetBinContent(i, n_e_sol_thr);
    }

    //Calculate MR
    histo_liquid->Divide(histo_liquid, elec_hist_liq);
    histo_solid->Divide(histo_solid, elec_hist_sol);
    mr->Divide(histo_solid, histo_liquid);
    mr->SetMarkerStyle(21);
    mr->Draw("COLZ");
    //corrected
    elec_hist_liq_corr->Multiply(elec_hist_liq, elec_hist_liq_thr);
    elec_hist_liq_corr->Divide(elec_hist_liq_acc);
    histo_liquid_corr->Divide(histo_liquid_corr, elec_hist_liq_corr);
    elec_hist_sol_corr->Multiply(elec_hist_sol, elec_hist_sol_thr);
    elec_hist_sol_corr->Divide(elec_hist_sol_acc);
    histo_solid_corr->Divide(histo_solid_corr, elec_hist_sol_corr);
    mr_corr->Divide(histo_solid_corr, histo_liquid_corr);
    mr_corr->SetMarkerStyle(21);
    mr_corr->Draw("COLZ");

    //Save plots
    //data
    histo_liquid->Write("ratio_liquid", TObject::kOverwrite);
    histo_solid->Write("ratio_solid", TObject::kOverwrite);
    mr->Write("MR", TObject::kOverwrite);
    //corrected
    histo_liquid_corr->Write("ratio_liquid_corr", TObject::kOverwrite);
    histo_solid_corr->Write("ratio_solid_corr", TObject::kOverwrite);
    mr_corr->Write("MR_corr", TObject::kOverwrite);

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