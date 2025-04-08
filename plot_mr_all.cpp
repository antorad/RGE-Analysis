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

    //array for files and inputs
    TString targets[5] = {"C", "Al", "Cu", "Sn", "Pb"};
    TFile* input_files[5];
    TH1D* h_mr_corr[5];

    for (int i = 0; i < 5; ++i){
        input_files[i] = new TFile(output_location+"/"+targets[i]+"/mr_"+mainVar+"_"+hadron+".root","READ");
        h_mr_corr[i] = (TH1D*)input_files[i]->Get("MR_corr");
    }

    //canvas
    TCanvas *canvas= new TCanvas("canvas","canvas",1000,600);
    canvas->cd();
    canvas->SetGrid();

    //set range on y axis for each case
    if (mainVar=="Zh") {h_mr_corr[0]->GetYaxis()->SetRangeUser(0,1.6);}
    if (mainVar=="Zh" && hadron=="proton") {h_mr_corr[0]->GetYaxis()->SetRangeUser(0,6);}
    if (mainVar=="Nu") {h_mr_corr[0]->GetYaxis()->SetRangeUser(0.6,1.2);}
    if (mainVar=="Nu" && hadron=="proton") {h_mr_corr[0]->GetYaxis()->SetRangeUser(1,4);}

    //Set axis names
    h_mr_corr[0]->GetYaxis()->SetTitle("#frac{N_{A}#pi^{+}}{N_{D2}#pi^{+}}#frac{N_{D2}e^{-}}{N_{A}e^{-}}");
    h_mr_corr[0]->GetXaxis()->SetTitle(mainVar);

    //set colors
    h_mr_corr[0]->SetMarkerColor(1);
    h_mr_corr[0]->SetLineColor(1);
    h_mr_corr[1]->SetMarkerColor(2);
    h_mr_corr[1]->SetLineColor(2);
    h_mr_corr[2]->SetMarkerColor(4);
    h_mr_corr[2]->SetLineColor(4);
    h_mr_corr[3]->SetMarkerColor(209);
    h_mr_corr[3]->SetLineColor(209);
    h_mr_corr[4]->SetMarkerColor(205);
    h_mr_corr[4]->SetLineColor(205);

    //draw plots without horizontal error bars
    for (int i = 0; i < 5; ++i){
        h_mr_corr[i]->Draw("E1X0same");
    }

    //Draw legens in the top right corner
    TLegend* legend = new TLegend(0.75, 0.75, 0.9, 0.9);
    legend->AddEntry(h_mr_corr[0], "Carbon", "p");
    legend->AddEntry(h_mr_corr[1], "Aluminum", "p");
    legend->AddEntry(h_mr_corr[2], "Copper", "p");
    legend->AddEntry(h_mr_corr[3], "Tin", "p");
    legend->AddEntry(h_mr_corr[4], "Lead", "p");
    legend->Draw("same");

    //Save final plot as pdf
    canvas->SaveAs("output/mr_"+hadron+"_"+mainVar+".pdf");

////////////////////////////////////////////////////////////////////////
//////////         	   Bin by bin MR comparison	              //////////
////////////////////////////////////////////////////////////////////////

}
//Create a n array with the 5 target plots ans  then create functions with the array and output file as argument 