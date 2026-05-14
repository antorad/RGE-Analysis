#include "include.h"

using namespace std;

void processChain(TChain* input_tuple, TString output_location){
    ROOT::EnableImplicitMT();

    //Output root file
    TFile *output = new TFile(output_location+"/pion_selection.root","RECREATE");

    Float_t pid, TOF, path, beta, p, charge, D_T, theo_time, exp_time, start_time;
    Float_t pimass = 139.57039;
    Float_t c = 29.9702547;

    cout<<"Reading input tuple"<<endl;
    //------Read branches with variables needed for cuts and plots------
    input_tuple->SetBranchAddress("pid",&pid);
    input_tuple->SetBranchAddress("start_time",&start_time);
    input_tuple->SetBranchAddress("TOF",&TOF);
    input_tuple->SetBranchAddress("path",&path);
    input_tuple->SetBranchAddress("beta",&beta);
    input_tuple->SetBranchAddress("p",&p);
    input_tuple->SetBranchAddress("charge",&charge);

    Float_t hadron_vars[9];
    const char* varslist = "pid:TOF:path:beta:p:charge:D_T:theo_time:exp_time";

    TNtuple *positves_tuple = new TNtuple("positives","positives",varslist);
    TNtuple *pion_nofilter_tuple = new TNtuple("pion_nofilter","pion_nofilter",varslist);
    TNtuple *pion_filtered_tuple = new TNtuple("pion_filtered","pion_filtered",varslist);

    //Create p bin pion tuples
    const int N_slices = 32;
    float p_step = 8./N_slices;

    TH1F *h_p_slice[N_slices];
    for (int i = 0; i < N_slices; ++i) {
        TString slice_name = Form("p_dt_slice_%i", i);
        h_p_slice[i] = new TH1F(slice_name, slice_name, 100, -1, 1);
    }

    //Cycle through entries
    Long64_t n_entries = input_tuple->GetEntries();
    for (Long64_t i=0;i<n_entries;i++) {
        input_tuple->GetEntry(i);

        theo_time = (path/(c*beta));
        exp_time  = TOF - start_time;
        D_T       = exp_time - theo_time;

        hadron_vars[0] = pid;
        hadron_vars[1] = TOF;
        hadron_vars[2] = path;
        hadron_vars[3] = beta;
        hadron_vars[4] = p;
        hadron_vars[5] = charge;
        hadron_vars[6] = D_T;
        hadron_vars[7] = theo_time;
        hadron_vars[8] = exp_time;

        if (charge==1){positves_tuple->Fill(hadron_vars);}

        if (pid==211){
            //Fill general all pion tuple
            pion_nofilter_tuple->Fill(hadron_vars);

            //dermine which specific p bin the pion belongs to and fill
            int bin_p = int(p/p_step);
            if (bin_p<N_slices){
                h_p_slice[bin_p]->Fill(D_T);
            }
        }
    }

    output->cd();
    //positves_tuple->Write();
    //pion_nofilter_tuple->Write();

    Float_t mu[N_slices], sigma[N_slices], mu_err[N_slices], sigma_err[N_slices];
    TH1F *mu_Dt = new TH1F("mu_Dt", "mu_Dt", N_slices, 0, 8);
    mu_Dt->SetMarkerStyle(8);
    mu_Dt->SetMarkerColor(2);
    mu_Dt->SetLineColor(2);
    TH1F *sigma_Dt = new TH1F("sigma_Dt", "sigma_Dt", N_slices, 0, 8);

    for (int i=0; i<N_slices; i++){
        h_p_slice[i]->Fit("gaus","","", -0.5, 0.5);
        TF1 * fit = h_p_slice[i]->GetFunction("gaus");
        if (fit){
            mu[i]        = fit->GetParameter(1);
            sigma[i]     = fit->GetParameter(2);
            mu_err[i]    = fit->GetParError(1);
            sigma_err[i] = fit->GetParError(2);

            //Fill mu_Dt and sigma_Dt for each p bin
            mu_Dt->SetBinContent(i, mu[i]);
            mu_Dt->SetBinError(i, mu_err[i]);
            sigma_Dt->SetBinContent(i, sigma[i]);
            sigma_Dt->SetBinError(i, sigma_err[i]);
        }
        else{
            cout <<"emoty bin"<< endl;
            mu_Dt->SetBinContent(i, 0);
            mu_Dt->SetBinError(i, 0);
            sigma_Dt->SetBinContent(i, 99);
            sigma_Dt->SetBinError(i, 99);
        }

        h_p_slice[i]->Write();
    }

// APPROACH 1 (SUM AND THEN FIT)

    TH1F *sigma_up, *sigma_low;
    sigma_up = (TH1F*)mu_Dt->Clone("sigma_up");
    sigma_up->Add(sigma_Dt, 3);

    sigma_low = (TH1F*)mu_Dt->Clone("sigma_low");
    sigma_low->Add(sigma_Dt, -3);

    //Fit mu and sigma plots
    TF1* fit_mu = new TF1("fit_mu", "[0]*x*x+[1]*x+[2]", 0, 8);
    mu_Dt->Fit("fit_mu","","",0,8);
    mu_Dt->Write();

    TF1* fit_sigma_up = new TF1("fit_sigma_up", "[0]*x*x+[1]*x+[2]", 0, 8);
    sigma_up->Fit("fit_sigma_up","","",0,8);
    sigma_up->Write();

    TF1* fit_sigma_low = new TF1("fit_sigma_low", "[0]*x*x+[1]*x+[2]", 0, 8);
    sigma_low->Fit("fit_sigma_low","","",0,8);
    sigma_low->Write();

// APPROACH 2 (FIT AND THEN SUM)
    
    TF1* fit_sigma = new TF1("fit_sigma", "[s0]*x*x+[s1]*x+[s2]", 0, 8);
    sigma_Dt->Fit("fit_sigma","","",0,8);
    sigma_Dt->Write();

    TF1* upper_lim = new TF1("upper_lim", "fit_mu+3*fit_sigma",0,8);
    TF1* lower_lim = new TF1("lower_lim", "fit_mu-3*fit_sigma",0,8);
    upper_lim->Write();
    lower_lim->Write();


//PLOTS

    //2D plots, positives and REC pions
    TCut pi_cut = "pid==211";
    TCut pos_cut = "charge==1";
    positves_tuple->Draw("D_T:p>>h2_dt_p_pos(100,0,10,100,-1.5,1.5)",pos_cut,"COLZ");
    TH2F *h2_dt_p_pos = (TH2F*)gDirectory->GetList()->FindObject("h2_dt_p_pos");

    pion_nofilter_tuple->Draw("D_T:p>>h2_dt_p_pi_nocut(100,0,10,100,-1.5,1.5)",pi_cut,"COLZ");
    TH2F *h2_dt_p_pi_nocut = (TH2F*)gDirectory->GetList()->FindObject("h2_dt_p_pi_nocut");

    h2_dt_p_pos->Write("h2_dt_p_pos");
    h2_dt_p_pi_nocut->Write("h2_dt_p_pi_nocut");

    TCanvas* c_fitsum = new TCanvas("fitsum", "fitsum", 600, 400);
    c_fitsum->cd();
    h2_dt_p_pi_nocut->Draw("COLZ");
    fit_mu->Draw("same");
    upper_lim->Draw("same");
    lower_lim->Draw("same");
    c_fitsum->Write();
    
    TCanvas* c_sumfit = new TCanvas("sumfit", "sumfit", 600, 400);
    c_sumfit->cd();
    h2_dt_p_pi_nocut->Draw("COLZ");
    mu_Dt->Draw("same");
    sigma_up->Draw("same");
    sigma_low->Draw("same");
    c_sumfit->Write();

    output->Close();
}

//Main function that recieves a txt with a list of run number asn the name of the output file
void pion_selection(const char* inputFileName, TString Target, TString type="data"){
    ROOT::EnableImplicitMT();

    // Open the input text file
    std::ifstream inputFile(inputFileName);

    // Create a TChain to combine input TNuples
    TChain* input_tuple = new TChain("data");
    TChain* input_tuple_mc = new TChain("MC");

    // Read each line from the text file and add the corresponding ROOT file to the TChain
    char buffer [10];
    std::string line;
    while (std::getline(inputFile, line)) {
        int run_N = stoi(line);
        sprintf(buffer,"%0*d", 6, run_N);
        TString run_N_str=TString(buffer);
        if (type=="data"){
            cout<<"Adding run: "<<line<<endl;
            input_tuple->Add(type+"/ntuples_dc_"+run_N_str+".root");
        }
        if (type=="simul"){
            cout<<"Addind simul job: "<<line<<endl;
            input_tuple->Add(type+"/"+Target+"/ntuples_dc_"+run_N_str+".root");
            input_tuple_mc->Add(type+"/"+Target+"/ntuples_dc_"+run_N_str+".root");
        }
    }

    // Close the input file
    inputFile.close();

    //process the Tchain to make plots and output tuples
    TString output_location = "output/"+type+"/"+Target+"/";
    gSystem->Exec("mkdir -vp "+output_location);
    cout<<"Output location: "<<output_location<<endl;

    processChain(input_tuple, output_location);
    cout<<"Data tuple done"<<endl;

    if (type=="simul") {
        output_location=output_location+"thrown/";
        processChain(input_tuple_mc, output_location);
        cout<<"MC tuple done"<<endl;
    }

    //delete all objects
    delete input_tuple;
    delete input_tuple_mc;
}

//Main function that recieves a run number as the input
void pion_selection(int run_N=000000, TString Target="unkw", TString type="data"){
    ROOT::EnableImplicitMT();

    // Create a TChain to load input TNuples
    TChain* input_tuple = new TChain("data");
    TChain* input_tuple_mc = new TChain("MC");

    //Transform input run number to Tstring with correct number of digits
    TString run_N_str = TString::Format("%06d", run_N);

    //Output directory
    TString output_location;
    output_location = "output/"+type+"/"+Target+"/"+run_N_str+"/";
    gSystem->Exec("mkdir -vp "+output_location);
    cout<<"Output location: "<<output_location<<endl;

    if (type=="data"){
        input_tuple->Add(type+"/ntuples_dc_"+run_N_str+".root");
    }
    if (type=="simul"){
        input_tuple->Add(type+"/"+Target+"/ntuples_dc_"+run_N_str+".root");
        input_tuple_mc->Add(type+"/"+Target+"/ntuples_dc_"+run_N_str+".root");
    }

    //process the Tchain to make plots and output tuples
    processChain(input_tuple, output_location);
    cout<<"Data tuple done"<<endl;

    if (type=="simul") {
        output_location=output_location+"thrown/";
        processChain(input_tuple_mc, output_location);
        cout<<"MC tuple done"<<endl;
    }

    //delete all objects
    delete input_tuple;
    delete input_tuple_mc;
}