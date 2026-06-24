#include "include.h"

gStyle->SetOptStat(0);

//TCut vz_d2, vz_solid, vz_d2_h, vz_solid_h;

TH1F* make_var_histo(TString var, int nbins, float xmin, float xmax, TChain* h_tuple, TString target){
    //Assigning targtet cut for electron counting
    //because we need to check hadrons' corresponding electron vertex, we have to check different
    //variables depending on the tuple type, so the variables in the cut have different names 
    TCut total_cut, target_cut;
    TString h_tuple_name = h_tuple->GetName();
    if (target=="d2"){target_cut="targ_type==1";}
    else if (target=="solid"){target_cut="targ_type==2";}
    //else if (target=="d2" && h_tuple_name=="elec_tuple"){target_cut=vz_d2;}
    //else if (target=="solid" && h_tuple_name!="elec_tuple"){target_cut=vz_solid_h;}
    //else if (target=="solid" && h_tuple_name=="elec_tuple"){target_cut=vz_solid;}

    //if (h_tuple_name=="elec_tuple"){
    //    total_cut= Main_cut&&elec_cut&&target_cut;
    //}
    //else{
    //    total_cut= Main_cut&&Var_cut&&target_cut;
    //}

    //Histogram for the var distibution
    int n_h = h_tuple->Draw(var+">>"+var+"_"+target+Form("(%i, %f, %f)", nbins, xmin, xmax),
                            target_cut, "COLZ");
    TH1F *h_d2_hist = (TH1F *)gDirectory->GetList()->FindObject(var+"_"+target);
    cout<<" Number of hadrons in "<< target <<": "<<n_h<<endl;
    return h_d2_hist;
}

TH1F* make_var_ehisto(TString var, int nbins, float xmin, float xmax, TChain* e_tuple, TString target){
    // check if the variable is an electron variable or not
    //if it is an electron variable, make an histo as same as the hadron
    TH1F* elec_hist;
    if (var=="Q2" || var=="nu"){
        elec_hist = make_var_histo(var, nbins, xmin, xmax, e_tuple, target);
        elec_hist->SetName(var+"_"+target+"_e");
    }
    //if it is an hadron variable, only count the number of electrons at the target
    else{
        //Assigning target cut for electron counting
        TCut target_cut;
        if (target=="d2"){target_cut="targ_type==1";}
        else if (target=="solid"){target_cut="targ_type==2";}

        //Counting of the number of electron in each target by making an hist and counting entries
        int n_e = e_tuple->Draw("vz>>h_e", target_cut, "goff");
        std::cout << "number of elec in " <<target<<" = " << n_e << std::endl;

        //Convert number of electron into flat histogram.
        elec_hist = new TH1F("elec_"+target+"_hist", "", nbins, xmin, xmax);
        for (int i = 1; i <= nbins; i++) {
            elec_hist->SetBinContent(i, n_e);
        }
    }
    return elec_hist;
}

void m_ratio(TString var, int nbins, float xmin, float xmax, TString hadron,
                    TFile* input, TString output_location, TFile* output){
    //Print message of wich vatiable is being calculated
    cout<<"Calculating Multiplicity Ratio of "<<var<<" variable"<<endl;

    //create canvas for the plots
    TCanvas *canvas= new TCanvas("canvas","canvas",1000,600);
    canvas->cd();

    //var histogram for hadron
    TH1F *h_d2_hist = (TH1F*)input->Get(hadron+"_"+var+"_d2");
    TH1F *h_solid_hist = (TH1F*)input->Get(hadron+"_"+var+"_solid");

    //var histogram for electrons
    TH1F* elec_d2_hist;
    TH1F* elec_solid_hist;

    if (var=="nu" || var=="Q2"){
        elec_d2_hist = (TH1F*)input->Get("e_"+var+"_d2");
        elec_solid_hist = (TH1F*)input->Get("e_"+var+"_solid");
    }

    else {
        int n_e_d2, n_e_solid;
        TH1F* elec_q2_d2_hist = (TH1F*)input->Get("e_Q2_d2");
        TH1F* elec_q2_solid_hist = (TH1F*)input->Get("e_Q2_solid");
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

void m_ratio_simul(TString var, int nbins, float xmin, float xmax, TString hadron,
                    TChain* h_tuple_sol, TChain* e_tuple_sol, TChain* h_tuple_liq, TChain* e_tuple_liq,
                    TString output_location, TFile* output){
    //Print message of wich vatiable is being calculated
    cout<<"Calculating Multiplicity Ratio of "<<var<<" variable"<<endl;

    //create canvas for the plots
    TCanvas *canvas= new TCanvas("canvas","canvas",1000,600);
    canvas->cd();

    //var histogram for electrons
    TH1F* elec_d2_hist = make_var_ehisto(var, nbins, xmin, xmax, e_tuple_liq, "d2");
    TH1F* elec_solid_hist = make_var_ehisto(var, nbins, xmin, xmax, e_tuple_sol, "solid");

    //var histogram for hadron
    TH1F *h_d2_hist = make_var_histo(var, nbins, xmin, xmax, h_tuple_liq, "d2");
    TH1F *h_solid_hist = make_var_histo(var, nbins, xmin, xmax, h_tuple_sol, "solid");

    //Error propagation
    h_d2_hist->Sumw2();
    h_solid_hist->Sumw2();
    elec_d2_hist->Sumw2();
    elec_solid_hist->Sumw2();

    //Multiplicity and multiplicity ratio histograms
    TH1F *m_d2 = new TH1F("Multiplicity D2 " + var, "Multiplicity D2" + var, nbins, xmin, xmax);
    TH1F *m_solid = new TH1F("Multiplicity Solid" + var, "Multiplicity solid" + var,
                                nbins, xmin, xmax);
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
    canvas->SaveAs(output_location+"mr_"+var+"_"+hadron+"_simul.pdf");

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
    TString output_location = "output/"+subdir+"/"+Target+thrown_dir+"/";
    TFile *output = new TFile(output_location+"mr_clas12.root","UPDATE");

    //Run the calculation for each variable
    if (type == "data"){
        //get input with histos created from simple_plots
        TFile *input = new TFile("output/"+subdir+"/"+Target+"/out_clas12.root","READ");

        m_ratio("z_h" , 10, Zh_bins[0], Zh_bins[N_Zh], hadron, input, output_location, output);
        m_ratio("nu"  , 10, Nu_bins[0], Nu_bins[N_Nu], hadron, input, output_location, output);
        m_ratio("p_T2", 10, Pt2_bins[0],Pt2_bins[N_Pt2], hadron, input, output_location,output);
        m_ratio("Q2"  , 10, Q2_bins[0], Q2_bins[N_Q2], hadron, input, output_location,output);
    }

    if (type=="acc" || type=="thrown"){
        //TFile *input_sol = new TFile("output/"+subdir+"/"+Target+thrown_dir+"/out_clas12.root","READ");
        //TFile *input_liq = new TFile("output/"+subdir+"/D2"+thrown_dir+"/out_clas12.root","READ");

        //TNtuple* hadron_tuple_sol = (TNtuple*)input_sol->Get(hadron+"_ntuple");
        //TNtuple* hadron_tuple_liq = (TNtuple*)input_liq->Get(hadron+"_ntuple");
        TChain* hadron_tuple_sol = new TChain(hadron+"_ntuple");
        TChain* hadron_tuple_liq = new TChain(hadron+"_ntuple");
        hadron_tuple_sol->Add("output/"+subdir+"/"+Target+thrown_dir+"/out_clas12.root");
        hadron_tuple_liq->Add("output/"+subdir+"/D2"+thrown_dir+"/out_clas12.root");

        //TNtuple* elec_tuple_sol = (TNtuple*)input_sol->Get("elec_tuple");
        //TNtuple* elec_tuple_liq = (TNtuple*)input_liq->Get("elec_tuple");

        TChain* elec_tuple_sol = new TChain("elec_tuple");
        TChain* elec_tuple_liq = new TChain("elec_tuple");
        elec_tuple_sol->Add("output/"+subdir+"/"+Target+thrown_dir+"/out_clas12.root");
        elec_tuple_liq->Add("output/"+subdir+"/D2"+thrown_dir+"/out_clas12.root");

        m_ratio_simul("z_h", 10, 0., 1., hadron, hadron_tuple_sol, elec_tuple_sol, hadron_tuple_liq, elec_tuple_liq, output_location, output);
        m_ratio_simul("nu", 10, 0., 11., hadron, hadron_tuple_sol, elec_tuple_sol, hadron_tuple_liq, elec_tuple_liq, output_location, output);
        m_ratio_simul("p_T2", 10, 0., 5., hadron, hadron_tuple_sol, elec_tuple_sol, hadron_tuple_liq, elec_tuple_liq, output_location,output);
    }

    output->Close();
}