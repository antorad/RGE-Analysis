#include "TROOT.h"
#include "TFile.h"
#include "TChain.h"
#include "TNtuple.h"
#include "TMath.h"
#include "TList.h"
#include "TString.h"
#include "TCut.h"
#include <iostream>
#include <string>
#include <cmath>
#include <fstream>

using namespace std;

void get_vertex_cut(TString Target="C", TString type="data"){
	//number of sugmas around mean for cut
	Float_t Nsigmas = 2.5;

    //get electron TH1 input created from simple_plots
    TFile *input = new TFile("output_"+fwd_rec+"/"+type+"/"+Target+"/out_clas12.root","READ");
    TH1F* v_z_histo = (TH1F*)input->Get("e_v_z");

	//liquid target
    TF1* f_d2 = new TF1("f_d2", "gaus", -9, -4);
    f_d2->SetParameters(v_z_histo->GetMaximum(), -6.5, 1 );
    v_z_histo->Fit("f_d2", "LRN");
    //get paramters
    Float_t mean_liq = f_d2->GetParameter(1);
    Float_t sigma_liq = f_d2->GetParameter(2);
    Float_t vz_min_liq = mean_liq-Nsigmas*sigma_liq;
    Float_t vz_max_liq = mean_liq+Nsigmas*sigma_liq;

    TH1F* liq_fill = (TH1F*)v_z_histo->Clone("liq_fill");
    for (int i = 1; i <= liq_fill->GetNbinsX(); ++i) {
	    double bin_center = liq_fill->GetBinCenter(i);
	    if (bin_center < vz_min_liq || bin_center > vz_max_liq)
	        liq_fill->SetBinContent(i, 0);
	}
	liq_fill->SetFillColor(kBlue);

	//solid target
    TF1* f_sol = new TF1("f_sol", "gaus", -2.5, 0);
    f_sol->SetParameters(v_z_histo->GetMaximum(), -1.5, 0.7 );
    v_z_histo->Fit("f_sol", "LRN");
    //get parameters
    Float_t mean_sol = f_sol->GetParameter(1);
    Float_t sigma_sol= f_sol->GetParameter(2);
    Float_t vz_min_sol = mean_sol-Nsigmas*sigma_sol;
    Float_t vz_max_sol = mean_sol+Nsigmas*sigma_sol;

    TH1F* sol_fill = (TH1F*)v_z_histo->Clone("sol_fill");
    for (int i = 1; i <= sol_fill->GetNbinsX(); ++i) {
	    double bin_center = sol_fill->GetBinCenter(i);
	    if (bin_center < vz_min_sol || bin_center > vz_max_sol)
	        sol_fill->SetBinContent(i, 0);
	}
	sol_fill->SetFillColor(kRed);

	//final plot
	TCanvas* canvas = new TCanvas("canvas", "canvas", 800, 600);
	canvas->cd();
	v_z_histo->Draw();
	liq_fill->Draw("same");
	sol_fill->Draw("same");

	//export fit values
	std::ofstream out("vertex_cuts.txt", ios::app);
	if ((type=="data")||(type=="simul"&&Target=="D2")){
		out << "// Liquid " << type << '\n';
	    out << "float D2_mean_"<< type << " = "   << mean_liq	 <<	";" << '\n';
	    out << "float D2_sigma_" << type << " = "  << sigma_liq	 <<	";" << '\n';
	    out << "float D2_vz_min_" << type << " = " << vz_min_liq << ";" << '\n';
	    out << "float D2_vz_max_" << type <<" = " << vz_max_liq << ";" << '\n' << '\n';
	}
	if ((type=="data")||(type=="simul"&&Target!="D2")){
	    out << "// Solid " << type << '\n';
	    out << "float C_mean_"<< type <<" = "   << mean_sol   << ";" << '\n';
	    out << "float C_sigma_"<< type <<" = "  << sigma_sol  << ";" << '\n';
	    out << "float C_vz_min_"<< type <<" = " << vz_min_sol << ";" << '\n';
	    out << "float C_vz_max_"<< type <<" = " << vz_max_sol << ";" << '\n' << '\n';
	}
}