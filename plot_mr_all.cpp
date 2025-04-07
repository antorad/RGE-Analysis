#include "include.h"

void plot_mr_all(int Hadron_pid=211, TString mainVar="Zh"){
	gStyle->SetOptStat(0);
////////////////////////////////////////////////////////////////////////
//////////          	   Total MR comparison	              //////////
////////////////////////////////////////////////////////////////////////
    //output directory
    TString output_location = "output/";

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

    //get TH1Ds from root files created with calculate_mr
    //input files
    TFile *input_C  = new TFile(output_location+"/C/mr_"+mainVar+"_v2.root","READ");
    TFile *input_Al = new TFile(output_location+"/Al/mr_"+mainVar+"_v2.root","READ");
    TFile *input_Cu = new TFile(output_location+"/Cu/mr_"+mainVar+"_v2.root","READ");
    TFile *input_Sn = new TFile(output_location+"/Sn/mr_"+mainVar+"_v2.root","READ");
    TFile *input_Pb = new TFile(output_location+"/Pb/mr_"+mainVar+"_v2.root","READ");

    //mr histograms
    TH1D* mr_hist_C  = (TH1D*)input_C->Get ("MR_corr");
    TH1D* mr_hist_Al = (TH1D*)input_Al->Get("MR_corr");
    TH1D* mr_hist_Cu = (TH1D*)input_Cu->Get("MR_corr");
    TH1D* mr_hist_Sn = (TH1D*)input_Sn->Get("MR_corr");
    TH1D* mr_hist_Pb = (TH1D*)input_Pb->Get("MR_corr");

    //canvas
    TCanvas *canvas= new TCanvas("canvas","canvas",1000,600);
    canvas->cd();
    canvas->SetGrid();

    //set range on y axis for each case
    if (mainVar=="Zh") {mr_hist_C->GetYaxis()->SetRangeUser(0,1.6);}
    if (mainVar=="Zh" && hadron=="proton") {mr_hist_C->GetYaxis()->SetRangeUser(0,6);}
    if (mainVar=="Nu") {mr_hist_C->GetYaxis()->SetRangeUser(0.6,1.2);}
    if (mainVar=="Nu" && hadron=="proton") {mr_hist_C->GetYaxis()->SetRangeUser(1,4);}

    //Set axis names
    mr_hist_C->GetYaxis()->SetTitle("#frac{N_{A}#pi^{+}}{N_{D2}#pi^{+}}#frac{N_{D2}e^{-}}{N_{A}e^{-}}");
    mr_hist_C->GetXaxis()->SetTitle(mainVar);

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
    mr_hist_C->Draw("E1X0same");
    mr_hist_Al->Draw("E1X0same");
    mr_hist_Cu->Draw("E1X0same");
    mr_hist_Sn->Draw("E1X0same");
    mr_hist_Pb->Draw("E1X0same");

    //Draw legens in the top right corner
    TLegend* legend = new TLegend(0.75, 0.75, 0.9, 0.9);
    legend->AddEntry(mr_hist_C, "Carbon", "p");
    legend->AddEntry(mr_hist_Al, "Aluminum", "p");
    legend->AddEntry(mr_hist_Cu, "Copper", "p");
    legend->AddEntry(mr_hist_Sn, "Tin", "p");
    legend->AddEntry(mr_hist_Pb, "Lead", "p");
    legend->Draw("same");

    //Save final plot as pdf
    canvas->SaveAs("output/mr_"+hadron+"_"+mainVar+".pdf");

////////////////////////////////////////////////////////////////////////
//////////         	   Bin by bin MR comparison	              //////////
////////////////////////////////////////////////////////////////////////

}