#include "include.h"
gStyle->SetOptStat(0);

void fancy_plots(int run_N=20156, TString Target="C", TString type="data"){
    ROOT::EnableImplicitMT();
    TString run_N_str = TString::Format("%06d", run_N);
    //output directory
    TString output_location = "plots/";

    //----------------------------------------------------
    //---------------PLOTS FROM OUT_CLAS12----------------
    //----------------------------------------------------
    //get TNtuple input created from simple_plots
    TFile *input = new TFile("output_"+fwd_rec+"/"+torus_pol+"/"+type+"/"+detector+"/"+Target+"/"+run_N_str+"/out_clas12.root","READ");

    TH2F *th2, *th2_raw;
    TCanvas *canvas = new TCanvas("canvas","canvas",1000,600);

    //Get histograms to plot
    canvas->cd();
    th2 =(TH2F*)input->Get("e_x_bxQ2");
    th2->Draw("COLZ");
    th2->SetTitle("Electron x_{b} vs Q^{2}");
    //Making them pretty
    canvas->SetLogz();
    //Save
    canvas->SaveAs(output_location+"e_xb_Q2.png");

    canvas->cd();
    th2 =(TH2F*)input->Get("e_ThetaxPhi");
    th2->Draw("COLZ");
    th2->SetTitle("Electron #theta vs #phi");
    //Making them pretty
    canvas->SetLogz();
    //Save
    canvas->SaveAs(output_location+"e_theta_phi.png");

    canvas->cd();
    th2 =(TH2F*)input->Get("e_PxTheta");
    th2->Draw("COLZ");
    th2->SetTitle("Electron P vs #theta");
    //Making them pretty
    canvas->SetLogz();
    //Save
    canvas->SaveAs(output_location+"e_p_theta.png");

    canvas->cd();
    th2 =(TH2F*)input->Get("e_NuxQ2");
    th2->Draw("COLZ");
    th2->SetTitle("Electron #nu vs Q_{2}");
    //Making them pretty
    canvas->SetLogz();
    //Save
    canvas->SaveAs(output_location+"e_nu_q2.png");

    canvas->cd();
    th2_raw =(TH2F*)input->Get("raw_e_xy_DCR1");
    th2 =(TH2F*)input->Get("e_xy_DCR1");
    th2_raw->Draw();
    th2_raw->SetMarkerColor(kRed);
    th2_raw->Draw("SCAT");
    th2->Draw("COLZsame");
    th2_raw->SetTitle("Electron Drift Chamber hits Region 1");
    //Making them pretty
    canvas->SetLogz();
    //Save
    canvas->SaveAs(output_location+"e_DC_R1.png");

    canvas->cd();
    th2_raw =(TH2F*)input->Get("raw_e_xy_DCR2");
    th2 =(TH2F*)input->Get("e_xy_DCR2");
    th2_raw->Draw();
    th2_raw->SetMarkerColor(kRed);
    th2_raw->Draw("SCAT");
    th2->Draw("COLZsame");
    th2_raw->SetTitle("Electron Drift Chamber hits Region 2");
    //Making them pretty
    canvas->SetLogz();
    //Save
    canvas->SaveAs(output_location+"e_DC_R2.png");

    canvas->cd();
    th2_raw =(TH2F*)input->Get("raw_e_xy_DCR3");
    th2 =(TH2F*)input->Get("e_xy_DCR3");
    th2_raw->Draw();
    th2_raw->SetMarkerColor(kRed);
    th2_raw->Draw("SCAT");
    th2->Draw("COLZsame");
    th2_raw->SetTitle("Electron Drift Chamber hits Region 3");
    //Making them pretty
    canvas->SetLogz();
    //Save
    canvas->SaveAs(output_location+"e_DC_R3.png");

    canvas->cd();
    th2_raw =(TH2F*)input->Get("h_pos_betaxP");
    th2 =(TH2F*)input->Get("pi_betaxP");
    th2_raw->Draw("COLZ");
    th2->SetContour(1);
    th2->SetLineWidth(3);
    th2->SetLineColor(kRed);
    th2->Draw("CONT3same");
    th2->SetTitle("Electron Drift Chamber hits Region 3");
    //Making them pretty
    canvas->SetLogz();
    //Save
    canvas->SaveAs(output_location+"pi_betaxP.png");

    canvas->cd();
    th2_raw =(TH2F*)input->Get("h_neg_betaxP");
    th2 =(TH2F*)input->Get("pim_betaxP");
    th2_raw->Draw("COLZ");
    th2->SetContour(1);
    th2->SetLineWidth(3);
    th2->SetLineColor(kRed);
    th2->Draw("CONT3same");
    th2->SetTitle("Electron Drift Chamber hits Region 3");
    //Making them pretty
    canvas->SetLogz();
    //Save
    canvas->SaveAs(output_location+"pim_betaxP.png");

    input->Close();
}