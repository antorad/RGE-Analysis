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
    TH1F *th1, *th1_raw;
    TLine *line;
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
    th2_raw->SetMarkerColor(kRed);
    th2_raw->SetMinimum(1);
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
    th2_raw->SetMarkerColor(kRed);
    th2_raw->SetMinimum(1);
    th2_raw->Draw("SCAT");;
    th2->Draw("COLZsame");
    th2_raw->SetTitle("Electron Drift Chamber hits Region 2");
    //Making them pretty
    canvas->SetLogz();
    //Save
    canvas->SaveAs(output_location+"e_DC_R2.png");

    canvas->cd();
    th2_raw =(TH2F*)input->Get("raw_e_xy_DCR3");
    th2 =(TH2F*)input->Get("e_xy_DCR3");
    th2_raw->SetMarkerColor(kRed);
    th2_raw->SetMinimum(1);
    th2_raw->Draw("SCAT");
    th2->Draw("COLZsame");
    th2_raw->SetTitle("Electron Drift Chamber hits Region 3");
    //Making them pretty
    canvas->SetLogz();
    //Save
    canvas->SaveAs(output_location+"e_DC_R3.png");

    canvas->cd();
    th2 =(TH2F*)input->Get("h_pos_betaxP");
    th2->Draw("COLZ");
    th2->SetTitle("Positive hadrons P vs #beta");
    //Making them pretty
    canvas->SetLogz();
    //Save
    canvas->SaveAs(output_location+"pos_betaxP.png");

    canvas->cd();
    th2 =(TH2F*)input->Get("pi_betaxP");
    th2->Draw("COLZ");
    th2->SetTitle("Positive pions P vs #beta");
    //Making them pretty
    canvas->SetLogz();
    //Save
    canvas->SaveAs(output_location+"pi_betaxP.png");

    canvas->cd();
    th2 =(TH2F*)input->Get("h_neg_betaxP");
    th2->Draw("COLZ");
    th2->SetTitle("Negative hadrons P vs #beta");
    //Making them pretty
    canvas->SetLogz();
    //Save
    canvas->SaveAs(output_location+"neg_betaxP.png");

    canvas->cd();
    th2 =(TH2F*)input->Get("pim_betaxP");
    th2->Draw("COLZ");
    th2->SetTitle("Negative pions P vs #beta");
    //Making them pretty
    canvas->SetLogz();
    //Save
    canvas->SaveAs(output_location+"pim_betaxP.png");

    canvas->cd();
    th1 =(TH1F*)input->Get("raw_e_nu");
    th1->SetTitle("Electron candidates #nu");
    th1->GetXaxis()->SetTitle("#nu [GeV]");
    line = new TLine(y_bjorken_max*10.564, 0, y_bjorken_max*10.564, th1->GetMaximum());
    line->SetLineColor(kRed);
    //Save
    th1->Draw();
    line->Draw("same");
    canvas->SaveAs(output_location+"raw_e_nu.png");

    canvas->cd();
    th1 =(TH1F*)input->Get("raw_e_q2");
    th1->SetTitle("Electron candidates Q^{2}");
    th1->GetXaxis()->SetTitle("#nu [GeV]");
    line = new TLine(Q2_min, 0, Q2_min, th1->GetMaximum());
    line->SetLineColor(kRed);
    //Save
    th1->Draw();
    line->Draw("same");
    canvas->SaveAs(output_location+"raw_e_q2.png");

    canvas->cd();
    th1 =(TH1F*)input->Get("raw_e_w2");
    th1->SetTitle("Electron candidates W^{2}");
    th1->GetXaxis()->SetTitle("#nu [GeV]");
    line = new TLine(W2_min, 0, W2_min, th1->GetMaximum());
    line->SetLineColor(kRed);
    //Save
    th1->Draw();
    line->Draw("same");
    canvas->SaveAs(output_location+"raw_e_w2.png");

    canvas->cd();
    th1 =(TH1F*)input->Get("pi_zh");
    th1->SetTitle("Positive Pion Z_{h} ");
    th1->GetXaxis()->SetTitle("Z_{h}");
    //Save
    th1->Draw();
    canvas->SaveAs(output_location+"pi_zh.png");

    canvas->cd();
    th1 =(TH1F*)input->Get("pi_pt2");
    th1->SetTitle("Positive Pion P_{t}^{2} ");
    th1->GetXaxis()->SetTitle("P_{t}^{2}");
    //Save
    th1->Draw();
    canvas->SaveAs(output_location+"pi_pt2.png");

    canvas->cd();
    th1 =(TH1F*)input->Get("pi_phi_PQ");
    th1->SetTitle("Positive Pion #phi_{PQ} ");
    th1->GetXaxis()->SetTitle("#phi_{PQ} [deg]");
    //Save
    th1->Draw();
    canvas->SaveAs(output_location+"pi_phi_PQ.png");

    canvas->cd();
    th1 =(TH1F*)input->Get("pim_zh");
    th1->SetTitle("Negative Pion Z_{h} ");
    th1->GetXaxis()->SetTitle("Z_{h}");
    //Save
    th1->Draw();
    canvas->SaveAs(output_location+"pim_zh.png");

    canvas->cd();
    th1 =(TH1F*)input->Get("pim_pt2");
    th1->SetTitle("Negative Pion P_{t}^{2} ");
    th1->GetXaxis()->SetTitle("P_{t}^{2}");
    //Save
    th1->Draw();
    canvas->SaveAs(output_location+"pim_pt2.png");

    canvas->cd();
    th1 =(TH1F*)input->Get("pim_phi_PQ");
    th1->SetTitle("Negative Pion #phi_{PQ} ");
    th1->GetXaxis()->SetTitle("#phi_{PQ} [deg]");
    //Save
    th1->Draw();
    canvas->SaveAs(output_location+"pim_phi_PQ.png");

    input->Close();
}