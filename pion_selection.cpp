#include "include.h"

using namespace std;

void processChain(TChain* input_tuple, TString output_location){
    gStyle->SetOptStat(0);
    ROOT::EnableImplicitMT();

    //Output root file
    TFile *output = new TFile(output_location+"/pion_selection.root","RECREATE");

    Float_t pid, TOF, path, beta_pi, p, charge, D_T, theo_time, exp_time, start_time, status;
    Float_t pimass = 0.139570;
    Float_t c = 29.9702547;

    cout<<"Reading input tuple"<<endl;
    //------Read branches with variables needed for cuts and plots------
    input_tuple->SetBranchAddress("pid",&pid);
    input_tuple->SetBranchAddress("start_time",&start_time);
    input_tuple->SetBranchAddress("time_tof",&TOF);
    input_tuple->SetBranchAddress("path_tof",&path);
    input_tuple->SetBranchAddress("p",&p);
    input_tuple->SetBranchAddress("charge",&charge);
    input_tuple->SetBranchAddress("status",&status);

    //Create TH2 histos for positive and pions
    TH2F* h2_dt_p_pos = new TH2F("h2_dt_p_pos","h2_dt_p_pos",100,0,10,100,-1.5,1.5);
    TH2F* h2_dt_p_pi_nocut = new TH2F("h2_dt_p_pi_nocut","h2_dt_p_pi_nocut",100,0,10,100,-1.5,1.5);

    //Create p bin pion tuples fro fitting later
    const int N_slices = 45;
    float p_step = 9./N_slices;

    TH1F *h_p_slice[N_slices];
    for (int i = 0; i < N_slices; ++i) {
        TString slice_name = Form("p_dt_slice_%i", i);
        h_p_slice[i] = new TH1F(slice_name, slice_name, 500, -1, 1);
    }

    //Cycle through entries
    Long64_t n_entries = input_tuple->GetEntries();
    for (Long64_t i=0;i<n_entries;i++) {
        input_tuple->GetEntry(i);

        beta_pi=p/sqrt(p*p+pimass*pimass);
        theo_time = (path/(c*beta_pi));
        exp_time  = TOF - start_time;
        D_T       = exp_time - theo_time;

        if (charge==1&&status<4000){
            //positives_tuple->Fill(hadron_vars);
            h2_dt_p_pos->Fill(p,D_T);
        }

        if (pid==211&&status<4000){
            //Fill general all pion tuple
            //pion_nofilter_tuple->Fill(hadron_vars);
            h2_dt_p_pi_nocut->Fill(p,D_T);

            //dermine which specific p bin the pion belongs to and fill
            int bin_p = int(p/p_step);
            if (bin_p<N_slices){
                h_p_slice[bin_p]->Fill(D_T);
            }
        }
    }

    output->cd();

    Float_t mu[N_slices], sigma[N_slices], mu_err[N_slices], sigma_err[N_slices];
    TH1F *mu_Dt = new TH1F("mu_Dt", "mu_Dt", N_slices, 0, 9);
    mu_Dt->SetMarkerStyle(8);
    mu_Dt->SetMarkerColor(2);
    mu_Dt->SetLineColor(2);
    TH1F *sigma_Dt = new TH1F("sigma_Dt", "sigma_Dt", N_slices, 0, 9);
    sigma_Dt->SetMarkerStyle(8);
    sigma_Dt->SetMarkerColor(2);
    sigma_Dt->SetLineColor(2);

    for (int i=0; i<N_slices; i++){
        h_p_slice[i]->Fit("gaus","","", -0.25, 0.20);
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
            cout <<"empty bin"<< endl;
            mu_Dt->SetBinContent(i, 0);
            mu_Dt->SetBinError(i, 0);
            sigma_Dt->SetBinContent(i, 99);
            sigma_Dt->SetBinError(i, 99);
        }

        h_p_slice[i]->Write();
    }

//FITTING MU
    cout<<"fitting mu"<<endl;
    //Fit mu and sigma plots
    TF1* fit_mu = new TF1("fit_mu", "[m0]*x*x+[m1]*x+[m2]", 0, 10);
    mu_Dt->Fit("fit_mu","","",0.4, 9);
    mu_Dt->Write();

//FITTING SIGMA
// APPROACH 1 (SUM AND THEN FIT)
    cout<<"app 1"<<endl;
    TH1F *sigma_up, *sigma_low;
    sigma_up = (TH1F*)mu_Dt->Clone("sigma_up");
    sigma_up->Add(sigma_Dt, 3);

    sigma_low = (TH1F*)mu_Dt->Clone("sigma_low");
    sigma_low->Add(sigma_Dt, -3);

    cout<<"fitting sigma"<<endl;

    TF1* fit_pol_sigma_up = new TF1("fit_pol_sigma_up", "[0_up]*x*x+[1_up]*x+[2_up]", 0, 10);
    TF1* fit_rgd_sigma_up = new TF1("fit_rgd_sigma_up", "sqrt(pow([s_up],2)+pow(([k_up]/x),2))+[c_up]*x", 0, 10);
    TF1* fit_res_sigma_up = new TF1("fit_res_sigma_up", "sqrt(pow([a_up],2)+pow([b_up]/pow(x,[e_up]),2))", 0, 10);
    fit_res_sigma_up->SetParameters(0.05,0,0.5);
    sigma_up->Fit("fit_pol_sigma_up","0","",0.4, 9);
    sigma_up->Fit("fit_rgd_sigma_up","0+","",0.4, 9);
    sigma_up->Fit("fit_res_sigma_up","0+","",0.4, 9);
    sigma_up->Write();

    TF1* fit_pol_sigma_low = new TF1("fit_pol_sigma_low", "[0_dw]*x*x+[1_dw]*x+[2_dw]", 0, 10);
    TF1* fit_rgd_sigma_low = new TF1("fit_rgd_sigma_low", "-sqrt(pow([s_dw],2)+pow(([k_dw]/x),2))+[c_dw]*x", 0, 10);
    TF1* fit_res_sigma_low = new TF1("fit_res_sigma_low", "-sqrt(pow([a_dw],2)+pow([b_dw]/pow(x,[e_dw]),2))", 0, 10);
    fit_res_sigma_low->SetParameters(0.05,0,0.5);
    sigma_low->Fit("fit_pol_sigma_low","0","",0.4, 9);
    sigma_low->Fit("fit_rgd_sigma_low","0+","",0.4, 9);
    sigma_low->Fit("fit_res_sigma_low","0+","",0.4, 9);
    sigma_low->Write();

// APPROACH 2 (FIT AND THEN SUM)
    cout<<"app2"<<endl;
    TF1* fit_p_sigma = new TF1("fit_p_sigma", "[s0]*x*x+[s1]*x+[s2]", 0, 10);
    TF1* fit_rgd_sigma = new TF1("fit_rgd_sigma", "sqrt(pow([s],2)+pow(([k]/x),2))+[c]*x", 0, 10);
    TF1* fit_res_sigma = new TF1("fit_res_sigma", "sqrt(pow([a],2)+pow([b]/pow(x,[e]),2))", 0, 10);
    fit_res_sigma->SetParameters(0.05,1,1);
    sigma_Dt->Fit("fit_p_sigma","0","",0.4, 9);
    sigma_Dt->Fit("fit_rgd_sigma","0+","",0.4, 9);
    sigma_Dt->Fit("fit_res_sigma","0+","",0.4, 9);
    sigma_Dt->Write();

    TF1* upper_lim_pol = new TF1("upper_lim_p", "fit_mu+3*fit_p_sigma",0, 10);
    TF1* lower_lim_pol = new TF1("lower_lim_p", "fit_mu-3*fit_p_sigma",0, 10);
    TF1* upper_lim_rgd = new TF1("upper_lim_rgd", "fit_mu+3*fit_rgd_sigma",0, 10);
    TF1* lower_lim_rgd = new TF1("lower_lim_rgd", "fit_mu-3*fit_rgd_sigma",0, 10);
    TF1* upper_lim_res = new TF1("upper_lim_res", "fit_mu+3*fit_res_sigma",0, 10);
    TF1* lower_lim_res = new TF1("lower_lim_res", "fit_mu-3*fit_res_sigma",0, 10);

    upper_lim_pol->Write();
    lower_lim_pol->Write();
    upper_lim_rgd->Write();
    lower_lim_rgd->Write();
    upper_lim_res->Write();
    lower_lim_res->Write();

//WRITE PARAMETERS INTO TXT FILE
    ofstream myFile(output_location+"/pion_fit_pars.txt");
    myFile<<"MEAN Polynomial fit: [a]*p*p+[b]*p+[c]"<<endl;
    myFile<<"a: "<<fit_mu->GetParameter(0)<<" , error: "<<fit_mu->GetParError(0)<<endl;
    myFile<<"b: "<<fit_mu->GetParameter(1)<<" , error: "<<fit_mu->GetParError(1)<<endl;
    myFile<<"c: "<<fit_mu->GetParameter(2)<<" , error: "<<fit_mu->GetParError(2)<<endl;
    myFile<<"chi2: "<<fit_mu->GetChisquare()<<" , NDF: "<<fit_mu->GetNDF()<<", chi2/NDF: "<<fit_mu->GetChisquare()/fit_mu->GetNDF()<<endl;
    myFile<<"-------------------------------------------------"<<endl;
    myFile<<"Sum mu and 3*sigma histograms and then fit"<<endl;
    myFile<<"-------------------------------------------------"<<endl;
    myFile<<"SIGMA Polynomial fit: [a]*p*p+[b]*p+[c]"<<endl;
    myFile<<"-------------------------------------------------"<<endl;
    myFile<<"upper a: "<<fit_pol_sigma_up->GetParameter(0)<<" , error: "<<fit_pol_sigma_up->GetParError(0)<<endl;
    myFile<<"upper b: "<<fit_pol_sigma_up->GetParameter(1)<<" , error: "<<fit_pol_sigma_up->GetParError(1)<<endl;
    myFile<<"upper c: "<<fit_pol_sigma_up->GetParameter(2)<<" , error: "<<fit_pol_sigma_up->GetParError(2)<<endl;
    myFile<<"chi2: "<<fit_pol_sigma_up->GetChisquare()<<" , NDF: "<<fit_pol_sigma_up->GetNDF()<<", chi2/NDF: "<<fit_pol_sigma_up->GetChisquare()/fit_pol_sigma_up->GetNDF()<<endl;
    myFile<<"-------------------------------------------------"<<endl;
    myFile<<"lower a: "<<fit_pol_sigma_low->GetParameter(0)<<" , error: "<<fit_pol_sigma_low->GetParError(0)<<endl;
    myFile<<"lower b: "<<fit_pol_sigma_low->GetParameter(1)<<" , error: "<<fit_pol_sigma_low->GetParError(1)<<endl;
    myFile<<"lower c: "<<fit_pol_sigma_low->GetParameter(2)<<" , error: "<<fit_pol_sigma_low->GetParError(2)<<endl;
    myFile<<"chi2: "<<fit_pol_sigma_low->GetChisquare()<<" , NDF: "<<fit_pol_sigma_low->GetNDF()<<", chi2/NDF: "<<fit_pol_sigma_low->GetChisquare()/fit_pol_sigma_low->GetNDF()<<endl;
    myFile<<"-------------------------------------------------"<<endl;
    myFile<<"SIGMA RGD fit: (-)sqrt(pow([s],2)+pow(([k]/p),2))+[c]*p"<<endl;
    myFile<<"-------------------------------------------------"<<endl;
    myFile<<"upper s: "<<fit_rgd_sigma_up->GetParameter(0)<<" , error: "<<fit_rgd_sigma_up->GetParError(0)<<endl;
    myFile<<"upper k: "<<fit_rgd_sigma_up->GetParameter(1)<<" , error: "<<fit_rgd_sigma_up->GetParError(1)<<endl;
    myFile<<"upper c: "<<fit_rgd_sigma_up->GetParameter(2)<<" , error: "<<fit_rgd_sigma_up->GetParError(2)<<endl;
    myFile<<"chi2: "<<fit_rgd_sigma_up->GetChisquare()<<" , NDF: "<<fit_rgd_sigma_up->GetNDF()<<", chi2/NDF: "<<fit_rgd_sigma_up->GetChisquare()/fit_rgd_sigma_up->GetNDF()<<endl;
    myFile<<"-------------------------------------------------"<<endl;
    myFile<<"lower s: "<<fit_rgd_sigma_low->GetParameter(0)<<" , error: "<<fit_rgd_sigma_low->GetParError(0)<<endl;
    myFile<<"lower k: "<<fit_rgd_sigma_low->GetParameter(1)<<" , error: "<<fit_rgd_sigma_low->GetParError(1)<<endl;
    myFile<<"lower c: "<<fit_rgd_sigma_low->GetParameter(2)<<" , error: "<<fit_rgd_sigma_low->GetParError(2)<<endl;
    myFile<<"chi2: "<<fit_rgd_sigma_low->GetChisquare()<<" , NDF: "<<fit_rgd_sigma_low->GetNDF()<<", chi2/NDF: "<<fit_rgd_sigma_low->GetChisquare()/fit_rgd_sigma_low->GetNDF()<<endl;
    myFile<<"-------------------------------------------------"<<endl;
    myFile<<"SIGMA Udi fit: (-)sqrt(pow([a],2)+pow([b]/pow(p,[e]),2))"<<endl;
    myFile<<"-------------------------------------------------"<<endl;
    myFile<<"upper a: "<<fit_res_sigma_up->GetParameter(0)<<" , error: "<<fit_res_sigma_up->GetParError(0)<<endl;
    myFile<<"upper b: "<<fit_res_sigma_up->GetParameter(1)<<" , error: "<<fit_res_sigma_up->GetParError(1)<<endl;
    myFile<<"upper e: "<<fit_res_sigma_up->GetParameter(2)<<" , error: "<<fit_res_sigma_up->GetParError(2)<<endl;
    myFile<<"chi2: "<<fit_res_sigma_up->GetChisquare()<<" , NDF: "<<fit_res_sigma_up->GetNDF()<<", chi2/NDF: "<<fit_res_sigma_up->GetChisquare()/fit_res_sigma_up->GetNDF()<<endl;
    myFile<<"-------------------------------------------------"<<endl;
    myFile<<"lower a: "<<fit_res_sigma_low->GetParameter(0)<<" , error: "<<fit_res_sigma_low->GetParError(0)<<endl;
    myFile<<"lower b: "<<fit_res_sigma_low->GetParameter(1)<<" , error: "<<fit_res_sigma_low->GetParError(1)<<endl;
    myFile<<"lower e: "<<fit_res_sigma_low->GetParameter(2)<<" , error: "<<fit_res_sigma_low->GetParError(2)<<endl;
    myFile<<"chi2: "<<fit_res_sigma_low->GetChisquare()<<" , NDF: "<<fit_res_sigma_low->GetNDF()<<", chi2/NDF: "<<fit_res_sigma_low->GetChisquare()/fit_res_sigma_low->GetNDF()<<endl;
    myFile<<"-------------------------------------------------"<<endl;
    myFile<<"Fit mu and sigma histograms and then sum functions"<<endl;
    myFile<<"-------------------------------------------------"<<endl;
    myFile<<"SIGMA Polynomial fit: [a]*p*p+[b]*p+[c]"<<endl;
    myFile<<"-------------------------------------------------"<<endl;
    myFile<<"a: "<<fit_p_sigma->GetParameter(0)<<" , error: "<<fit_p_sigma->GetParError(0)<<endl;
    myFile<<"b: "<<fit_p_sigma->GetParameter(1)<<" , error: "<<fit_p_sigma->GetParError(1)<<endl;
    myFile<<"c: "<<fit_p_sigma->GetParameter(2)<<" , error: "<<fit_p_sigma->GetParError(2)<<endl;
    myFile<<"chi2: "<<fit_p_sigma->GetChisquare()<<" , NDF: "<<fit_p_sigma->GetNDF()<<", chi2/NDF: "<<fit_p_sigma->GetChisquare()/fit_p_sigma->GetNDF()<<endl;
    myFile<<"-------------------------------------------------"<<endl;
    myFile<<"SIGMA RGD fit: sqrt(pow([s],2)+pow(([k]/p),2))+[c]*p"<<endl;
    myFile<<"-------------------------------------------------"<<endl;
    myFile<<"s: "<<fit_rgd_sigma->GetParameter(0)<<" , error: "<<fit_rgd_sigma->GetParError(0)<<endl;
    myFile<<"k: "<<fit_rgd_sigma->GetParameter(1)<<" , error: "<<fit_rgd_sigma->GetParError(1)<<endl;
    myFile<<"c: "<<fit_rgd_sigma->GetParameter(2)<<" , error: "<<fit_rgd_sigma->GetParError(2)<<endl;
    myFile<<"chi2: "<<fit_rgd_sigma->GetChisquare()<<" , NDF: "<<fit_rgd_sigma->GetNDF()<<", chi2/NDF: "<<fit_rgd_sigma->GetChisquare()/fit_rgd_sigma->GetNDF()<<endl;
    myFile<<"-------------------------------------------------"<<endl;
    myFile<<"SIGMA Udi fit: sqrt(pow([a],2)+pow([b]/pow(p,[e]),2))"<<endl;
    myFile<<"-------------------------------------------------"<<endl;
    myFile<<"a: "<<fit_res_sigma->GetParameter(0)<<" , error: "<<fit_res_sigma->GetParError(0)<<endl;
    myFile<<"b: "<<fit_res_sigma->GetParameter(1)<<" , error: "<<fit_res_sigma->GetParError(1)<<endl;
    myFile<<"e: "<<fit_res_sigma->GetParameter(2)<<" , error: "<<fit_res_sigma->GetParError(2)<<endl;
    myFile<<"chi2: "<<fit_res_sigma->GetChisquare()<<" , NDF: "<<fit_res_sigma->GetNDF()<<", chi2/NDF: "<<fit_res_sigma->GetChisquare()/fit_res_sigma->GetNDF()<<endl;

//PLOTS
    TCanvas* c_pos = new TCanvas("c_pos", "c_pos", 600, 400);
    c_pos->cd();
    h2_dt_p_pos->Draw("COLZ");
    h2_dt_p_pos->GetXaxis()->SetTitle("p (GeV)");
    h2_dt_p_pos->GetYaxis()->SetTitle("#DeltaT_{#pi} (ns)");
    h2_dt_p_pos->SetTitle("#DeltaT_{#pi} vs p for postive particles in forward detector");
    h2_dt_p_pos->SetStats("kFalse");
    c_pos->Write();

    TCanvas* c_pion = new TCanvas("c_pion", "c_pion", 600, 400);
    c_pion->cd();
    h2_dt_p_pi_nocut->Draw("COLZ");
    h2_dt_p_pi_nocut->GetXaxis()->SetTitle("p (GeV)");
    h2_dt_p_pi_nocut->GetYaxis()->SetTitle("#DeltaT_{#pi} (ns)");
    h2_dt_p_pi_nocut->SetTitle("#DeltaT_{#pi} vs p for #pi^{+} in forward detector");
    h2_dt_p_pi_nocut->SetStats("kFalse");
    c_pion->Write();

    TCanvas* c_fit_mu = new TCanvas("c_fit_mu", "c_fit_mu", 600, 400);
    c_fit_mu->cd();
    mu_Dt->SetMinimum(-0.2);
    mu_Dt->SetMaximum(0.2);
    mu_Dt->Draw();
    mu_Dt->GetXaxis()->SetTitle("p (GeV)");
    mu_Dt->GetYaxis()->SetTitle("#mu parameter from fit");
    mu_Dt->SetTitle("#mu values from fits for #pi^{+} in forward detector");
    c_fit_mu->Write();

    TCanvas* c_fit_sigma = new TCanvas("c_fit_sigma", "c_fit_sigma", 600, 400);
    c_fit_sigma->cd();
    sigma_Dt->GetFunction("fit_p_sigma")->SetBit(TF1::kNotDraw);
    sigma_Dt->SetMinimum(0);
    sigma_Dt->SetMaximum(0.4);
    sigma_Dt->Draw();
    fit_rgd_sigma->SetLineColor(8);
    fit_rgd_sigma->Draw("same");
    fit_res_sigma->Draw("same");
    sigma_Dt->GetXaxis()->SetTitle("p (GeV)");
    sigma_Dt->GetYaxis()->SetTitle("#sigma parameter from fit");
    sigma_Dt->SetTitle("#sigma values from fits for #pi^{+} in forward detector");
    c_fit_sigma->Write();

    TCanvas* c_fitsum = new TCanvas("fitsum", "fitsum", 600, 400);
    c_fitsum->cd();
    h2_dt_p_pi_nocut->Draw("COLZ");
    fit_mu->Draw("same");
    upper_lim_rgd->SetMarkerColor(8);
    upper_lim_rgd->SetLineColor(8);
    upper_lim_rgd->SetLineWidth(5);
    upper_lim_rgd->Draw("same");
    lower_lim_rgd->SetMarkerColor(8);
    lower_lim_rgd->SetLineColor(8);
    lower_lim_rgd->SetLineWidth(5);
    lower_lim_rgd->Draw("same");
    upper_lim_res->SetLineWidth(5);
    upper_lim_res->Draw("same");
    lower_lim_res->SetLineWidth(5);
    lower_lim_res->Draw("same");
    c_fitsum->Write();
    
    TCanvas* c_sumfit = new TCanvas("sumfit", "sumfit", 600, 400);
    c_sumfit->cd();
    h2_dt_p_pi_nocut->Draw("COLZ");
    mu_Dt->Draw("same");
    sigma_up->Draw("same");
    sigma_low->Draw("same");
    fit_rgd_sigma_low->Draw("same");
    fit_rgd_sigma_low->SetMarkerColor(8);
    fit_rgd_sigma_low->SetLineColor(8);
    fit_rgd_sigma_low->SetLineWidth(5);
    fit_rgd_sigma_low->Draw("same");
    fit_rgd_sigma_up->SetMarkerColor(8);
    fit_rgd_sigma_up->SetLineColor(8);
    fit_rgd_sigma_up->SetLineWidth(5);
    fit_rgd_sigma_up->Draw("same");
    fit_res_sigma_low->SetLineWidth(5);
    fit_res_sigma_low->Draw("same");
    fit_res_sigma_up->SetLineWidth(5);
    fit_res_sigma_up->Draw("same");
    c_sumfit->Write();

    output->Close();
}

//Main function that recieves a txt with a list of run number asn the name of the output file
void pion_selection(const char* inputFileName, TString Target, TString type="data"){
    ROOT::EnableImplicitMT();

    // Open the input text file
    std::ifstream inputFile(inputFileName);

    // Create a TChain to combine input TNuples
    TChain* input_tuple = new TChain("DT");
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
    TChain* input_tuple = new TChain("DT");
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