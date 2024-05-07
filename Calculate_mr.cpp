#include "TChain.h"
#include "TFile.h"
#include "TList.h"
#include "TMath.h"
#include "TNtuple.h"
#include "TROOT.h"
#include "TString.h"
#include <algorithm>
#include <cmath>
#include <iostream>

// Max number of events in the solid target.
// The idea is to have the same number of events in all the solids targets

int const MAX_EVENTS = 166000;

// Transform a histogram(TH1) into a TGraphErrors (easy to plot in matplotlib)
TGraphErrors *TH1TOTGraph(TH1 *h1) {

    TGraphErrors *g1 = new TGraphErrors();
    if (!h1) {
        std::cout << "TH1TOTGraph: histogram not found !" << std::endl;
        return g1;
    }
    Double_t x, y, ex, ey;
    for (Int_t i = 1; i <= h1->GetNbinsX(); i++) {
        y = h1->GetBinContent(i);
        ey = h1->GetBinError(i);
        x = h1->GetBinCenter(i);
        // ex=h1->GetBinWidth(i);
        ex = 0.5 * h1->GetBinWidth(i);
        g1->SetPoint(i - 1, x, y);
        g1->SetPointError(i - 1, ex, ey);
    }
    return g1;
}

void Calculate_mr(TString run_name_input, int pid_input) {

    /// input files has the name ntuple_dc_ runname, it can be the run number or
    /// target
    TString run_name = run_name_input;
    int hadron_pid = pid_input;

    std::cout << "Current run/target: " << run_name << std::endl;
    std::cout << "Looking for : pid = " << hadron_pid << std::endl;

    TFile *input_file =
        new TFile("./data/ntuples_dc_" + run_name + ".root", "READ");
    TNtuple *input_tuple = (TNtuple *)input_file->Get("data");
    gROOT->cd();

    //
    // ******* PARTICLE AND VERTEX IDENTIFICATION *******
    //

    // Useful variables
    float pid, Q2, nu, v_z, z_h, p, E_total, E_ECIN, E_ECOU, event_num,
        v_z_elec, phi, W2, yb;

    //------Read branches with variables used and needed for cuts_pions------
    input_tuple->SetBranchAddress("pid", &pid);
    input_tuple->SetBranchAddress("Q2", &Q2);
    input_tuple->SetBranchAddress("nu", &nu);
    input_tuple->SetBranchAddress("v_z", &v_z);
    input_tuple->SetBranchAddress("z_h", &z_h);
    input_tuple->SetBranchAddress("p", &p);
    input_tuple->SetBranchAddress("E_total", &E_total);
    input_tuple->SetBranchAddress("E_ECIN", &E_ECIN);
    input_tuple->SetBranchAddress("E_ECOU", &E_ECOU);
    input_tuple->SetBranchAddress("event_num", &event_num);
    input_tuple->SetBranchAddress("W2", &W2);
    input_tuple->SetBranchAddress("y_bjorken", &yb);

    //------output ntuple------
    Float_t pion_vars[10];
    Float_t elec_vars[8];
    const char *pion_varslist =
        "pid:Q2:nu:v_z:p:E_total:E_ECIN:E_ECOU:z_h:v_z_elec";
    const char *elec_varslist = "pid:Q2:nu:v_z:p:E_total:E_ECIN:E_ECOU";
    TNtuple *pion_tuple = new TNtuple("pion_ntuple", "pions", pion_varslist);
    TNtuple *elec_tuple = new TNtuple("elec_tuple", "electrons", elec_varslist);

    v_z_elec = 0;
    int event_counter = 0;

    // Premiliminary cuts to separate events from liquid and solid target
    float vz_d2_min = -8.01;
    float vz_d2_max = -3.62;
    float vz_solid_min = -1.84;
    float vz_solid_max = 0.09;

    int n_entries = input_tuple->GetEntries(); // number of events in the tuple
    for (int i = 0; i < n_entries; i++) {
        input_tuple->GetEntry(i);

        // We want the same number of events in the solid target to study the
        // error behavior
        if (event_counter >= MAX_EVENTS)
            break;

        // Fill the tuples after the cuts
        if (Q2 > 1 && W2 > 4 && yb < 0.85) { // DIS cuts

            if (pid == 11) { // Check if it is and electron
                elec_vars[0] = pid;
                elec_vars[1] = Q2;
                elec_vars[2] = nu;
                elec_vars[3] = v_z;
                elec_vars[4] = p;
                elec_vars[5] = E_total;
                elec_vars[6] = E_ECIN;
                elec_vars[7] = E_ECOU;
                elec_tuple->Fill(elec_vars);
                v_z_elec = v_z;
                if (v_z > vz_solid_min && v_z < vz_solid_max) {
                    event_counter++;
                }
                // In case we want same number of events in D2 + solid
                // if((v_z > vz_d2_min && v_z < vz_d2_max) || (v_z >
                // vz_solid_min && v_z < vz_solid_max)) event_counter++;

            } else if (pid == hadron_pid) { // Check if it is a pion
                pion_vars[0] = pid;
                pion_vars[1] = Q2;
                pion_vars[2] = nu;
                pion_vars[3] = v_z;
                pion_vars[4] = p;
                pion_vars[5] = E_total;
                pion_vars[6] = E_ECIN;
                pion_vars[7] = E_ECOU;
                pion_vars[8] = z_h;
                pion_vars[9] = v_z_elec;
                pion_tuple->Fill(pion_vars);
            }
        }
    }

    std::cout << "Number of electrons in solid target: " << event_counter
              << std::endl;

    //------output writing------
    TFile *output =
        new TFile("./output/" + run_name + "/out_clas12_" +
                      std::to_string(hadron_pid) + "_" + run_name + ".root",
                  "RECREATE");
    output->cd();
    pion_tuple->Write();
    elec_tuple->Write();
    // output->Close();
    // gROOT->cd();
    cout << "Particle and vertex identification done!" << endl;

    //
    // ******* MUTIPLICITY RATIO CALCULATION *******
    //

    // as a funcion of zh

    TString var = "z_h";
    int n_bins = 10; // number of bins in the MR plot
    float zh_min = 0.;
    float zh_max = 1.;

    // vertex cuts
    TCut cuts_elec_solid =
        Form("p>0 && v_z>%f && v_z<%f", vz_solid_min, vz_solid_max);
    TCut cuts_pions_solid =
        Form("p>0 && v_z_elec>%f && v_z_elec<%f", vz_solid_min, vz_solid_max);
    TCut cuts_elec_d2 = Form("p>0 && v_z>%f && v_z<%f", vz_d2_min, vz_d2_max);
    TCut cuts_pions_d2 =
        Form("p>0 && v_z_elec>%f && v_z_elec<%f", vz_d2_min, vz_d2_max);

    // Zh is not a electron variable so we use the number of events
    int n_e_d2 = elec_tuple->Draw("v_z>>hist_e_d2", cuts_elec_d2, "goff");
    int n_e_solid =
        elec_tuple->Draw("v_z>>hist_e_solid", cuts_elec_solid, "goff");

    std::cout << "elec d2 = " << n_e_d2 << std::endl;
    std::cout << "elec solid = " << n_e_solid << std::endl;

    // Histogram with the Zh binning. All bins contain the number of electrons
    TH1F *elec_d2_zh_hist =
        new TH1F("elec_d2_zh_hist", "", n_bins, zh_min, zh_max);
    TH1F *elec_solid_zh_hist =
        new TH1F("elec_solid_zh_hist", "", n_bins, zh_min, zh_max);

    for (int i = 1; i <= n_bins; i++) {
        elec_d2_zh_hist->SetBinContent(i, n_e_d2);
        elec_solid_zh_hist->SetBinContent(i, n_e_solid);
    }

    // Zh histogram for pions
    pion_tuple->Draw(var + ">>" + var +
                         Form("_d2(%i, %f, %f)", n_bins, zh_min, zh_max),
                     cuts_pions_d2, "COLZ");
    TH1F *pion_zh_d2_hist =
        (TH1F *)gDirectory->GetList()->FindObject(var + "_d2");
    pion_tuple->Draw(var + ">>" + var +
                         Form("_solid(%i, %f, %f)", n_bins, zh_min, zh_max),
                     cuts_pions_solid, "COLZ");
    TH1F *pion_zh_solid_hist =
        (TH1F *)gDirectory->GetList()->FindObject(var + "_solid");

    pion_zh_d2_hist->Sumw2();
    pion_zh_solid_hist->Sumw2();
    elec_d2_zh_hist->Sumw2();
    elec_solid_zh_hist->Sumw2();

    // Multiplicity and multiplicity ratio histogram
    TH1F *m_d2_zh = new TH1F("Multiplicity D2" + var, "Multiplicity D2" + var,
                             n_bins, zh_min, zh_max);
    TH1F *m_solid_zh =
        new TH1F("Multiplicity Solid" + var, "Multiplicity solid" + var, n_bins,
                 zh_min, zh_max);
    TH1F *mr_zh = new TH1F("Multiplicity ratio" + var,
                           "Multiplicity ratio" + var, n_bins, zh_min, zh_max);

    m_d2_zh->Sumw2();
    m_solid_zh->Sumw2();
    mr_zh->Sumw2();

    // Calculate multiplicity and multiplicity ratio
    m_d2_zh->Divide(pion_zh_d2_hist, elec_d2_zh_hist);
    m_solid_zh->Divide(pion_zh_solid_hist, elec_solid_zh_hist);
    mr_zh->Divide(m_solid_zh, m_d2_zh);

    std::cout << "Zh finished" << std::endl;

    // As a funcion of a electron variable

    var = "nu";
    float var_min = 0.;
    float var_max = 11.;

    pion_tuple->Draw(var + ">>" + var +
                         Form("_d2(%i, %f, %f)", n_bins, var_min, var_max),
                     cuts_pions_d2, "COLZ");
    TH1F *pion_var_d2_hist =
        (TH1F *)gDirectory->GetList()->FindObject(var + "_d2");
    pion_tuple->Draw(var + ">>" + var +
                         Form("_solid(%i, %f, %f)", n_bins, var_min, var_max),
                     cuts_pions_solid, "COLZ");
    TH1F *pion_var_solid_hist =
        (TH1F *)gDirectory->GetList()->FindObject(var + "_solid");
    elec_tuple->Draw(var + ">>" + var +
                         Form("_d2_e(%i, %f, %f)", n_bins, var_min, var_max),
                     cuts_elec_d2, "COLZ");
    TH1F *elec_var_d2_hist =
        (TH1F *)gDirectory->GetList()->FindObject(var + "_d2_e");
    elec_tuple->Draw(var + ">>" + var +
                         Form("_solid_e(%i, %f, %f)", n_bins, var_min, var_max),
                     cuts_elec_solid, "COLZ");
    TH1F *elec_var_solid_hist =
        (TH1F *)gDirectory->GetList()->FindObject(var + "_solid_e");

    // Multiplicity and multiplicity ratio histogram
    TH1F *m_d2_var = new TH1F("Multiplicity D2" + var, "Multiplicity D2" + var,
                              n_bins, var_min, var_max);
    TH1F *m_solid_var =
        new TH1F("Multiplicity Solid" + var, "Multiplicity solid" + var, n_bins,
                 var_min, var_max);
    TH1F *mr_var =
        new TH1F("Multiplicity ratio" + var, "Multiplicity ratio" + var, n_bins,
                 var_min, var_max);

    m_d2_var->Sumw2();
    m_solid_var->Sumw2();
    mr_var->Sumw2();

    // Calculate multiplicity and multiplicity ratio
    m_d2_var->Divide(pion_var_d2_hist, elec_var_d2_hist);
    m_solid_var->Divide(pion_var_solid_hist, elec_var_solid_hist);
    mr_var->Divide(m_solid_var, m_d2_var);

    std::cout << var << " finished" << std::endl;

    TGraphErrors *graph_zh = (TGraphErrors *)TH1TOTGraph(mr_zh);
    TGraphErrors *graph_var = (TGraphErrors *)TH1TOTGraph(mr_var);

    // Save MR Tgraph
    output->cd();
    graph_zh->Write("multiplicity_ratio_z_h");
    graph_var->Write("multiplicity_ratio_nu");
    elec_d2_zh_hist->Write("elec_zh_d2");
    elec_solid_zh_hist->Write("elec_zh_solid");

    std::cout << "Saved!" << std::endl;

    output->Close();
    input_file->Close();
}
