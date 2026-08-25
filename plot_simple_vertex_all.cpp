#include "include.h"

void plot_simple_vertex_all(){
    gStyle->SetOptStat(0);
    //output directory
    TString output_location = "output_"+fwd_rec+"/data";

    //get TH1Fs from root files created with calculate_mr
    //input files
    TFile *input_C = new TFile(output_location+"/C/out_clas12.root","READ");
    TFile *input_Al = new TFile(output_location+"/Al/out_clas12.root","READ");
    TFile *input_Cu = new TFile(output_location+"/Cu/out_clas12.root","READ");
    TFile *input_Sn = new TFile(output_location+"/Sn/out_clas12.root","READ");
    TFile *input_Pb = new TFile(output_location+"/Pb/out_clas12.root","READ");
    //vertex histograms
    TH1F* mr_hist_C = (TH1F*)input_C->Get("e_vz");
    TH1F* mr_hist_Al = (TH1F*)input_Al->Get("e_vz");
    TH1F* mr_hist_Cu = (TH1F*)input_Cu->Get("e_vz");
    TH1F* mr_hist_Sn = (TH1F*)input_Sn->Get("e_vz");
    TH1F* mr_hist_Pb = (TH1F*)input_Pb->Get("e_vz");

    //canvas
    TCanvas *canvas= new TCanvas("canvas","canvas",1000,600);
    canvas->cd();
    canvas->SetGrid();

    //set colors
    mr_hist_C->SetMarkerColor(1);
    mr_hist_C->SetLineColor(1);
    mr_hist_Al->SetMarkerColor(2);
    mr_hist_Al->SetLineColor(2);
    mr_hist_Cu->SetMarkerColor(4);
    mr_hist_Cu->SetLineColor(4);
    mr_hist_Sn->SetMarkerColor(209);
    mr_hist_Sn->SetLineColor(209);
    mr_hist_Pb->SetMarkerColor(205);
    mr_hist_Pb->SetLineColor(205);

    //draw plots without horizontal error bars
    mr_hist_C->Draw("same");
    mr_hist_Al->Draw("same");
    mr_hist_Cu->Draw("same");
    mr_hist_Sn->Draw("same");
    mr_hist_Pb->Draw("same");

    mr_hist_C->GetYaxis()->SetRangeUser(0.,mr_hist_Pb->GetMaximum()*1.1);

    //Draw legens in the top right corner
    TLegend* legend = new TLegend(0.75, 0.75, 0.9, 0.9);
    legend->AddEntry(mr_hist_C, "Carbon");
    legend->AddEntry(mr_hist_Al, "Aluminum");
    legend->AddEntry(mr_hist_Cu, "Copper");
    legend->AddEntry(mr_hist_Sn, "Tin");
    legend->AddEntry(mr_hist_Pb, "Lead");
    legend->Draw("same");

    //Save final plot as pdf
    canvas->SaveAs("output_"+fwd_rec+"/data/e_v_z.pdf");

    TCanvas *canvas_sector= new TCanvas("canvas_sector","canvas_sector",1000,600);
    canvas_sector->Divide(3,2);
    TH1F *histo[6];
    TLine *vline[6][4];
    for (int i = 0; i < 6; ++i){
        canvas_sector->cd(i+1);
        histo[i] = (TH1F*)input_C->Get(Form("e_vz_sector_%i",i));
        histo[i]->GetXaxis()->SetTitle("v_{z}");
        histo[i]->GetYaxis()->SetTitle("N_{e}");
        histo[i]->SetTitle(Form("v_{z} Sector %i",i+1));
        histo[i]->Draw("COLZ");
        histo[i]->Draw();
        for (int j=0; j<4; j++){
            vline[i][j] = new TLine(vertex_cut[i][j],0, vertex_cut[i][j],histo[i]->GetMaximum());
            vline[i][j]->SetLineColor(kRed);
            vline[i][j]->Draw();
        }
    }
    canvas_sector->SaveAs("output_"+fwd_rec+"/data/e_v_z_sector.pdf");
}