#include "include.h"

void vertex_banks(int run_N=000000, TString Target="C", TString type="data"){
    //Transform input run number to Tstring with correct number of digits
    TString run_N_str = TString::Format("%06d", run_N);

    gStyle->SetOptStat(0);

    //input directory
    TString input_location;
    if      (run_location=="own_pc"){input_location="data";}
    else if (run_location=="farm"){input_location="/volatile/clas12/antorad/rge/MR_analysis/"+type+"/pass1/"+fwd_rec+"/"+Target+"/"+run_N_str;}

    //get input tuple
    TFile *input_dc = new TFile(input_location+"/ntuples_dc_"+run_N_str+".root","READ");
    TFile *input_dc_fmt = new TFile(input_location+"/ntuples_dc_fmt_"+run_N_str+".root","READ");
    TNtuple* e_tuple_dc = (TNtuple*)input_dc->Get("DT");
    TNtuple* e_tuple_dc_fmt = (TNtuple*)input_dc_fmt->Get("DT");

    //Plots
    TCanvas *canvas = new TCanvas("canvas","canvas",1000,600);
    canvas->cd();

    e_tuple_dc->Draw("vz>>histo_dc(500,-12,5)","pid==11&&status<0","COLZ");
    TH1F *histo_dc = (TH1F*)gDirectory->GetList()->FindObject("histo_dc");

    e_tuple_dc_fmt->Draw("vz>>histo_dc_fmt(500,-12,5)","pid==11&&status<0","COLZ");
    TH1F *histo_dc_fmt = (TH1F*)gDirectory->GetList()->FindObject("histo_dc_fmt");

    e_tuple_dc_fmt->Draw("vz>>histo_fmt(500,-12,5)","pid==11&&status<0&&FMT_layers>1","COLZ");
    TH1F *histo_fmt = (TH1F*)gDirectory->GetList()->FindObject("histo_fmt");

    histo_dc_fmt->GetXaxis()->SetTitle("Vz [cm]");
    histo_dc_fmt->GetYaxis()->SetTitle("counts");
    histo_dc_fmt->SetTitle("Z-vertex distrubtion for different reconstruction banks");
    histo_dc_fmt->SetLineColor(kRed);
    histo_dc_fmt->Draw("COLZ");
    histo_dc->SetLineColor(kBlue);
    histo_dc->Draw("same");
    histo_fmt->SetLineColor(kBlack);
    histo_fmt->Draw("same");
    canvas->SaveAs("vertex_banks.png");
}