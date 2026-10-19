#include "include.h"

TString mainVar="";
TString secVar="";
TString hadron="";

TCanvas plot_same_canvas_inb(TH1D* h_mr[5][N_Pt2]){
    //canvas
    TCanvas *canvas = new TCanvas("canvas","canvas",1800,400);
    canvas->Divide(N_Pt2, 1);
    for (int j = 0; j < N_Pt2; ++j){
        canvas->cd(j+1);
        canvas->SetGrid();

        //set range on y axis for each case
        if      (mainVar=="Zh") {h_mr[0][j]->GetYaxis()->SetRangeUser(0,1.6);}
        else if (mainVar=="Nu") {h_mr[0][j]->GetYaxis()->SetRangeUser(0.2,1.2);}
        else if (mainVar=="Q2") {h_mr[0][j]->GetYaxis()->SetRangeUser(0.5,1.2);}
        else if (mainVar=="Pt2") {h_mr[0][j]->GetYaxis()->SetRangeUser(0.5,4.0);}
        else if (mainVar=="Phi_PQ") {h_mr[0][j]->GetYaxis()->SetRangeUser(0.5,1.2);}

        //Set axis names
        if(hadron=="pion") {h_mr[0][j]->GetYaxis()->SetTitle("#frac{N_{A}#pi^{+}}{N_{D2}#pi^{+}}#frac{N_{D2}e^{-}}{N_{A}e^{-}}");}
        if(hadron=="pion_minus") {h_mr[0][j]->GetYaxis()->SetTitle("#frac{N_{A}#pi^{-}}{N_{D2}#pi^{-}}#frac{N_{D2}e^{-}}{N_{A}e^{-}}");}
        h_mr[0][j]->GetXaxis()->SetTitle(mainVar);
        h_mr[0][j]->SetTitle(Form("%.2f < ",Pt2_bins[j])+secVar+" < "+Form("%.2f",Pt2_bins[j+1]));

        //set colors
        h_mr[0][j]->SetMarkerColor(1);
        h_mr[0][j]->SetLineColor(1);
        h_mr[1][j]->SetMarkerColor(2);
        h_mr[1][j]->SetLineColor(2);
        h_mr[2][j]->SetMarkerColor(4);
        h_mr[2][j]->SetLineColor(4);
        h_mr[3][j]->SetMarkerColor(209);
        h_mr[3][j]->SetLineColor(209);
        h_mr[4][j]->SetMarkerColor(205);
        h_mr[4][j]->SetLineColor(205);

        //draw plots without horizontal error bars
        for (int i = 0; i < 5; ++i){
            h_mr[i][j]->Draw("E1X0same");
        }

        //Draw legens in the top right corner
        TLegend* legend = new TLegend(0.75, 0.75, 0.9, 0.9);
        if (mainVar=="Pt2" || mainVar=="Q2" || mainVar=="Nu"){legend = new TLegend(0.75, 0.25, 0.9, 0.1);}
        legend->AddEntry(h_mr[0][0], "Carbon", "p");
        legend->AddEntry(h_mr[1][0], "Aluminum", "p");
        legend->AddEntry(h_mr[2][0], "Copper", "p");
        legend->AddEntry(h_mr[3][0], "Tin", "p");
        legend->AddEntry(h_mr[4][0], "Lead", "p");
        //legend->Draw("same");
    }

    //Save final plot as pdf
    canvas->SaveAs("output_"+fwd_rec+"/"+torus_pol+"/AC_MR/"+detector+"/mr_2d_"+hadron+"_"+mainVar+"_"+secVar+".pdf");
    return canvas;
}

TCanvas plot_same_canvas_out(TH1D* h_mr[2]){
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
    h_mr[1]->SetMarkerColor(205);
    h_mr[1]->SetLineColor(205);

    //draw plots without horizontal error bars
    for (int i = 0; i < 2; ++i){
        h_mr[i]->Draw("E1X0same");
    }

    //Draw legens in the top right corner
    TLegend* legend = new TLegend(0.75, 0.75, 0.9, 0.9);
    if (mainVar=="Pt2" || mainVar=="Q2" || mainVar=="Nu"){legend = new TLegend(0.75, 0.25, 0.9, 0.1);}
    legend->AddEntry(h_mr[0], "Carbon", "p");
    legend->AddEntry(h_mr[1], "Lead", "p");
    legend->Draw("same");

    //Save final plot as pdf
    canvas->SaveAs("output_"+fwd_rec+"/"+torus_pol+"/AC_MR/"+detector+"/mr_"+hadron+"_"+mainVar+".pdf");
    return canvas;
}

void plot_mr_2d(int Hadron_pid=211, TString var1="Zh", TString var2="Pt2"){
	gStyle->SetOptStat(0);
    mainVar=var1;
    secVar=var2;

    //output directory
    TString output_location = "output_"+fwd_rec+"/"+torus_pol;

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
    TString targets_inb[5] = {"C", "Al", "Cu", "Sn", "Pb"};
    TFile* input_files_inb[5];
    TH1D* h_mr_corr_inb[5][N_Pt2];  //------------->Change to N_(secVar), for now Pt2 for testing

    TString targets_out[2] = {"C", "Pb"};
    TFile* input_files_out[2];
    TH1D* h_mr_corr_out[2][N_Pt2];  //------------->Change to N_(secVar), for now Pt2 for testing

    //Total MR comparison
    if (torus_pol=="inb"){
        for (int i = 0; i < 5; ++i){
            input_files_inb[i] = new TFile(output_location+"/AC_MR/"+detector+"/"+targets_inb[i]+"/mr_"+mainVar+"_"+secVar+"_"+hadron+".root","READ");
            for (int j = 0; j < N_Pt2; ++j){ //------------->Change to N_(secVar), for now Pt2 for testing
                h_mr_corr_inb[i][j] = (TH1D*)input_files_inb[i]->Get(Form("MR_corr_%i", j));
            }
        }
        plot_same_canvas_inb(h_mr_corr_inb);
    }
    else if (torus_pol=="out"){
        for (int i = 0; i < 2; ++i){
            input_files_out[i] = new TFile(output_location+"/AC_MR/"+detector+"/"+targets_out[i]+"/mr_"+mainVar+"_"+secVar+"_"+hadron+".root","READ");
            for (int j = 0; j < N_Pt2; ++j){ //------------->Change to N_(secVar), for now Pt2 for testing
                h_mr_corr_out[i][j] = (TH1D*)input_files_out[i]->Get("MR_corr");
            }
        }
       // plot_same_canvas_out(h_mr_corr_out);
    }
}