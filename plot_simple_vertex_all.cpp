void plot_simple_vertex_all(){
    gStyle->SetOptStat(0);
    //output directory
    TString output_location = "output/data";

    //get TH1Fs from root files created with calculate_mr
    //input files
    TFile *input_C = new TFile(output_location+"/C/out_clas12.root","READ");
    TFile *input_Al = new TFile(output_location+"/Al/out_clas12.root","READ");
    TFile *input_Cu = new TFile(output_location+"/Cu/out_clas12.root","READ");
    TFile *input_Sn = new TFile(output_location+"/Sn/out_clas12.root","READ");
    TFile *input_Pb = new TFile(output_location+"/Pb/out_clas12.root","READ");
    //vertex histograms
    TH1F* mr_hist_C = (TH1F*)input_C->Get("e_v_z");
    TH1F* mr_hist_Al = (TH1F*)input_Al->Get("e_v_z");
    TH1F* mr_hist_Cu = (TH1F*)input_Cu->Get("e_v_z");
    TH1F* mr_hist_Sn = (TH1F*)input_Sn->Get("e_v_z");
    TH1F* mr_hist_Pb = (TH1F*)input_Pb->Get("e_v_z");

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

    //Draw legens in the top right corner
    TLegend* legend = new TLegend(0.75, 0.75, 0.9, 0.9);
    legend->AddEntry(mr_hist_C, "Carbon");
    legend->AddEntry(mr_hist_Al, "Aluminum");
    legend->AddEntry(mr_hist_Cu, "Copper");
    legend->AddEntry(mr_hist_Sn, "Tin");
    legend->AddEntry(mr_hist_Pb, "Lead");
    legend->Draw("same");

    //Save final plot as pdf
    canvas->SaveAs("output/data/e_v_z.pdf");
}