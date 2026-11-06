#include "include.h"

gStyle->SetOptStat(0);

void m_ratio(TString var, int nbins, float xmin, float xmax, TString hadron,
                    TFile* input_sol, TFile* input_d2, TString output_location, TFile* output){
    //Print message of wich vatiable is being calculated
    cout<<"Calculating Multiplicity Ratio of "<<var<<" variable"<<endl;

    //create canvas for the plots
    TCanvas *canvas= new TCanvas("canvas","canvas",1000,600);
    canvas->cd();

    //var histogram for hadron
    TH1F *h_d2_hist = (TH1F*)input_d2->Get(hadron+"_"+var+"_d2");
    TH1F *h_solid_hist = (TH1F*)input_sol->Get(hadron+"_"+var+"_solid");

    //var histogram for electrons
    TH1F* elec_d2_hist;
    TH1F* elec_solid_hist;

    if (var=="nu" || var=="Q2"){
        elec_d2_hist = (TH1F*)input_d2->Get("e_"+var+"_d2");
        elec_solid_hist = (TH1F*)input_sol->Get("e_"+var+"_solid");
    }

    else {
        int n_e_d2, n_e_solid;
        TH1F* elec_q2_d2_hist = (TH1F*)input_d2->Get("e_Q2_d2");
        TH1F* elec_q2_solid_hist = (TH1F*)input_sol->Get("e_Q2_solid");
        n_e_d2 = elec_q2_d2_hist->GetEntries();
        n_e_solid = elec_q2_solid_hist->GetEntries();

        //Convert number of electron into flat histogram.
        elec_d2_hist = new TH1F("e_"+var+"_d2", "e_"+var+"_d2", nbins, xmin, xmax);
        elec_solid_hist = new TH1F("e_"+var+"_solid", "e_"+var+"_solid", nbins, xmin, xmax);
        for (int i = 1; i <= nbins; i++) {
            elec_d2_hist->SetBinContent(i, n_e_d2);
            elec_solid_hist->SetBinContent(i, n_e_solid);
        }
        cout<<elec_d2_hist->GetEntries()<<endl;
    }

    //Error propagation
    h_d2_hist->Sumw2();
    h_solid_hist->Sumw2();
    elec_d2_hist->Sumw2();
    elec_solid_hist->Sumw2();

    //Multiplicity and multiplicity ratio histograms
    TH1F *m_d2 = new TH1F("Multiplicity D2 " + var, "Multiplicity D2" + var, nbins, xmin, xmax);
    TH1F *m_solid = new TH1F("Multiplicity Solid" + var, "Multiplicity solid" + var, nbins, xmin, xmax);
    TH1F *mr = new TH1F("Multiplicity ratio " + var, "Multiplicity ratio" + var, nbins, xmin, xmax);

    //Error propagation
    m_d2->Sumw2();
    m_solid->Sumw2();
    mr->Sumw2();

    //Calculation of multiplicity and multiplicity ratio
    m_d2->Divide(h_d2_hist, elec_d2_hist);
    m_solid->Divide(h_solid_hist, elec_solid_hist);
    mr->Divide(m_solid, m_d2);
    mr->GetXaxis()->SetTitle(var);
    mr->GetYaxis()->SetTitle("#frac{N_{A}#pi^{+}}{N_{D2}#pi^{+}}#frac{N_{D2}e^{-}}{N_{A}e^{-}}");
    mr->SetTitle("Multiplicity Ratio");
    mr->SetMarkerStyle(21);
    mr->Draw("COLZ");
    canvas->SaveAs(output_location+"mr_"+var+"_"+hadron+".pdf");

    output->cd();
    h_d2_hist->Write(hadron+"_"+var+"_d2", TObject::kOverwrite);
    h_solid_hist->Write(hadron+"_"+var+"_solid", TObject::kOverwrite);
    elec_d2_hist->Write(hadron+"_"+var+"_elec_d2", TObject::kOverwrite);
    elec_solid_hist->Write(hadron+"_"+var+"_elec_solid", TObject::kOverwrite);
    m_d2->Write(hadron+"_"+var+"_multiplicity_d2", TObject::kOverwrite);
    m_solid->Write(hadron+"_"+var+"_multiplicity_solid", TObject::kOverwrite);
    mr->Write(hadron+"_"+var+"_mratio", TObject::kOverwrite);

    //delete objects
    delete mr;
    delete m_solid;
    delete m_d2;
    delete elec_solid_hist;
    delete elec_d2_hist;
    delete h_solid_hist;
    delete h_d2_hist;
    delete canvas;

    std::cout << var <<" MR finished" << std::endl; 
}

void simple_mr(TString Target="C", int Hadron_pid=211, TString type="data"){
    ROOT::EnableImplicitMT();

    TString subdir;
    TString thrown_dir = "";
    if (type=="data"){subdir="data";}
    if (type=="acc" || type =="thrown"){subdir="simul";}
    if (type=="thrown"){thrown_dir="/thrown";}

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

    //output root file for histograms
    TString output_location = "output_"+fwd_rec+"/"+torus_pol+"/"+type+"/"+detector+"/"+Target+"/";
    TFile *output = new TFile(output_location+"mr_clas12.root","UPDATE");

    //Run the calculation for each variable
    //get input with histos created from simple_plots
    TFile *input_d2;
    TFile *input_sol = new TFile("output_"+fwd_rec+"/"+torus_pol+"/"+type+"/"+detector+"/"+Target+"/out_clas12.root","READ");
    if(type=="data"){input_d2 = new TFile("output_"+fwd_rec+"/"+torus_pol+"/"+type+"/"+detector+"/"+Target+"/out_clas12.root","READ");}
    else if(type=="acc"||type=="thrown"){input_d2 = new TFile("output_"+fwd_rec+"/"+torus_pol+"/"+type+"/"+detector+"/D2/out_clas12.root","READ");}

    m_ratio("z_h" , 10, Zh_bins[0], Zh_bins[N_Zh], hadron, input_sol, input_d2, output_location, output);
    m_ratio("nu"  , 10, Nu_bins[0], Nu_bins[N_Nu], hadron, input_sol, input_d2, output_location, output);
    m_ratio("p_T2", 10, Pt2_1d_bins[0],Pt2_1d_bins[N_Pt2], hadron, input_sol, input_d2, output_location,output);
    m_ratio("Q2"  , 10, Q2_bins[0], Q2_bins[N_Q2], hadron, input_sol, input_d2, output_location,output);
    m_ratio("phi_PQ",10,Phi_bins[0], Phi_bins[N_Phi], hadron, input_sol, input_d2, output_location,output);

    output->Close();
}