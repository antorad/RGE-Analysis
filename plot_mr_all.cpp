#include "include.h"

TString mainVar="";
TString hadron="";

TCanvas plot_same_canvas(TH1D* h_mr[5]){
    //canvas
    TCanvas *canvas= new TCanvas("canvas","canvas",1000,600);
    canvas->cd();
    canvas->SetGrid();

    //set range on y axis for each case
    if      (mainVar=="Zh") {h_mr[0]->GetYaxis()->SetRangeUser(0,1.6);}
    else if (mainVar=="Nu") {h_mr[0]->GetYaxis()->SetRangeUser(0.2,1.2);}
    else if (mainVar=="Q2") {h_mr[0]->GetYaxis()->SetRangeUser(0.5,1.2);}
    else if (mainVar=="Pt2") {h_mr[0]->GetYaxis()->SetRangeUser(0.5,4.0);}
    else if (mainVar=="Phi_PQ") {h_mr[0]->GetYaxis()->SetRangeUser(0.5,1.2);}

    //Set axis names
    if(hadron=="pion") {h_mr[0]->GetYaxis()->SetTitle("#frac{N_{A}#pi^{+}}{N_{D2}#pi^{+}}#frac{N_{D2}e^{-}}{N_{A}e^{-}}");}
    if(hadron=="pion_minus") {h_mr[0]->GetYaxis()->SetTitle("#frac{N_{A}#pi^{-}}{N_{D2}#pi^{-}}#frac{N_{D2}e^{-}}{N_{A}e^{-}}");}
    h_mr[0]->GetXaxis()->SetTitle(mainVar);

    //set colors
    h_mr[0]->SetMarkerColor(1);
    h_mr[0]->SetLineColor(1);
    h_mr[1]->SetMarkerColor(2);
    h_mr[1]->SetLineColor(2);
    h_mr[2]->SetMarkerColor(4);
    h_mr[2]->SetLineColor(4);
    h_mr[3]->SetMarkerColor(209);
    h_mr[3]->SetLineColor(209);
    h_mr[4]->SetMarkerColor(205);
    h_mr[4]->SetLineColor(205);

    //draw plots without horizontal error bars
    for (int i = 0; i < 5; ++i){
        h_mr[i]->Draw("E1X0same");
    }

    //Draw legens in the top right corner
    TLegend* legend = new TLegend(0.75, 0.75, 0.9, 0.9);
    if (mainVar=="Pt2" || mainVar=="Q2" || mainVar=="Nu"){legend = new TLegend(0.75, 0.25, 0.9, 0.1);}
    legend->AddEntry(h_mr[0], "Carbon", "p");
    legend->AddEntry(h_mr[1], "Aluminum", "p");
    legend->AddEntry(h_mr[2], "Copper", "p");
    legend->AddEntry(h_mr[3], "Tin", "p");
    legend->AddEntry(h_mr[4], "Lead", "p");
    legend->Draw("same");

    //Save final plot as pdf
    canvas->SaveAs("output/AC_MR/mr_"+hadron+"_"+mainVar+".pdf");
    return canvas;
}

void plot_mr_all(int Hadron_pid=211, TString Var="Zh"){
	gStyle->SetOptStat(0);
    mainVar=Var;

    //output directory
    TString output_location = "output/";

    //hadron selection
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

    //Total MR comparison
    for (int i = 0; i < 5; ++i){
        input_files[i] = new TFile(output_location+"/AC_MR/"+targets[i]+"/mr_"+mainVar+"_"+hadron+".root","READ");
        h_mr_corr[i] = (TH1D*)input_files[i]->Get("MR_corr");
    }
    plot_same_canvas(h_mr_corr);
}
//Create a n array with the 5 target plots and then create functions with the array and output file as argument 