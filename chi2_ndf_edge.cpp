#include "include.h"

void plot_profiles(TFile* input_file, char const*particle, TString output_location){

    float DC_R1_fc, DC_R2_fc, DC_R3_fc;

    if (TString(particle)=="e"){
        DC_R1_fc = DC_R1_fc_e;
        DC_R2_fc = DC_R2_fc_e;
        DC_R3_fc = DC_R3_fc_e;
    }
    else if (TString(particle)=="pi"){
        DC_R1_fc = DC_R1_fc_pi;
        DC_R2_fc = DC_R2_fc_pi;
        DC_R3_fc = DC_R3_fc_pi;
    }
    else if (TString(particle)=="pim"){
        DC_R1_fc = DC_R1_fc_pim;
        DC_R2_fc = DC_R2_fc_pim;
        DC_R3_fc = DC_R3_fc_pim;
    }

    TH2F* histo_R1[6];
    TH2F* histo_R2[6];
    TH2F* histo_R3[6];

    for (int i = 1; i < 7; ++i){
        histo_R1[i-1]=(TH2F*)input_file->Get(Form("raw_%s_chi2_ndf_edge_R1_%i", particle, i));
        histo_R1[i-1]->SetName(Form("R1_%i",i));
        histo_R2[i-1]=(TH2F*)input_file->Get(Form("raw_%s_chi2_ndf_edge_R2_%i", particle, i));
        histo_R2[i-1]->SetName(Form("R2_%i",i));
        histo_R3[i-1]=(TH2F*)input_file->Get(Form("raw_%s_chi2_ndf_edge_R3_%i", particle, i));
        histo_R3[i-1]->SetName(Form("R3_%i",i));
    }

    TProfile* profiles_R1[6];
    TProfile* profiles_R2[6];
    TProfile* profiles_R3[6];

    TCanvas *canvas= new TCanvas("canvas","canvas",1920,600);
    canvas->Divide(3,1);

    canvas->cd(1);
    gPad->SetGrid();
    TLegend* legend = new TLegend(0.75, 0.75, 0.9, 0.9);
    for (int i = 1; i<7; ++i){
        profiles_R1[i-1] = histo_R1[i-1]->ProfileX();
        profiles_R1[i-1]->SetName(Form("PX_R1_%i",i));
        profiles_R1[i-1]->SetLineColor(i);
        profiles_R1[i-1]->SetMarkerStyle(8);
        profiles_R1[i-1]->SetMarkerColor(i);
        profiles_R1[i-1]->Draw("same");
        legend->AddEntry(profiles_R1[i-1], Form("Sector %i",i));
    }
    profiles_R1[0]->GetYaxis()->SetRangeUser(0, 30);
    TLine *fc_R1 = new TLine(DC_R1_fc, 0, DC_R1_fc, 30);
    fc_R1->SetLineColor(kRed);
    fc_R1->SetLineWidth(2);
    fc_R1->Draw("same");
    legend->Draw("same");

    canvas->cd(2);
    gPad->SetGrid();
    for (int i = 1; i<7; ++i){
        profiles_R2[i-1] = histo_R2[i-1]->ProfileX();
        profiles_R2[i-1]->SetName(Form("PX_R2_%i",i));
        profiles_R2[i-1]->SetLineColor(i);
        profiles_R2[i-1]->SetMarkerStyle(8);
        profiles_R2[i-1]->SetMarkerColor(i);
        profiles_R2[i-1]->Draw("same");
    }
    profiles_R2[0]->GetYaxis()->SetRangeUser(0, 30);
    TLine *fc_R2 = new TLine(DC_R2_fc, 0, DC_R2_fc, 30);
    fc_R2->SetLineColor(kRed);
    fc_R2->SetLineWidth(2);
    fc_R2->Draw("same");
    legend->Draw("same");

    canvas->cd(3);
    gPad->SetGrid();
    TLegend* legend3 = new TLegend(0.75, 0.75, 0.9, 0.9);
    for (int i = 1; i<7; ++i){
        profiles_R3[i-1] = histo_R3[i-1]->ProfileX();
        profiles_R3[i-1]->SetName(Form("PX_R3_%i",i));
        profiles_R3[i-1]->SetLineColor(i);
        profiles_R3[i-1]->SetMarkerStyle(8);
        profiles_R3[i-1]->SetMarkerColor(i);
        profiles_R3[i-1]->Draw("same");
        legend3->AddEntry(profiles_R3[i-1], Form("Sector %i",i));
    }
    profiles_R3[0]->GetYaxis()->SetRangeUser(0, 30);
    TLine *fc_R3 = new TLine(DC_R3_fc, 0, DC_R3_fc, 30);
    fc_R3->SetLineColor(kRed);
    fc_R3->SetLineWidth(2);
    fc_R3->Draw("same");
    legend->Draw("same");

    canvas->SaveAs(output_location+TString(particle)+"_ndf_chi2.pdf");
    canvas->SaveAs(output_location+TString(particle)+"_ndf_chi2.png");
}

void chi2_ndf_edge(int run_N=000000, TString Target="unkw", TString type="data"){
    gStyle->SetOptStat(0);

    //Transform input run number to Tstring with correct number of digits
    TString run_N_str = TString::Format("%06d", run_N);;

    TFile* input_file = new TFile("output_"+fwd_rec+"/"+torus_pol+"/"+type+"/"+detector+"/"+Target+"/"+run_N_str+"/out_clas12.root","READ");
    TString output_location = "output_"+fwd_rec+"/"+torus_pol+"/"+type+"/"+detector+"/"+Target+"/"+run_N_str+"/";

    plot_profiles(input_file, "e", output_location);
    plot_profiles(input_file, "pi", output_location);
    plot_profiles(input_file, "pim", output_location);

}