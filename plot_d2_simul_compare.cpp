#include "include.h"

TCanvas plot_same_canvas_inb(TH1D* h_mr[5], TString out_file_name, TString var){
    //canvas
    TCanvas *canvas= new TCanvas("canvas","canvas",1000,600);
    canvas->cd();
    canvas->SetGrid();

    //set range on y axis for each case
    h_mr[0]->GetYaxis()->SetRangeUser(0,h_mr[0]->GetMaximum()*1.1);

    //Set axis names
    h_mr[0]->GetYaxis()->SetTitle("Entries");
    h_mr[0]->GetXaxis()->SetTitle(var);

    //set colors
    h_mr[0]->SetLineColor(1);
    h_mr[1]->SetLineColor(2);
    h_mr[2]->SetLineColor(4);
    h_mr[3]->SetLineColor(209);
    h_mr[4]->SetLineColor(205);

    //draw plots without horizontal error bars
    for (int i = 0; i < 5; ++i){
        h_mr[i]->Draw("same");
    }

    //Draw legens in the top right corner
    TLegend* legend = new TLegend(0.75, 0.75, 0.9, 0.9);
    legend = new TLegend(0.75, 0.25, 0.9, 0.1);
    legend->AddEntry(h_mr[0], "Carbon", "p");
    legend->AddEntry(h_mr[1], "Aluminum", "p");
    legend->AddEntry(h_mr[2], "Copper", "p");
    legend->AddEntry(h_mr[3], "Tin", "p");
    legend->AddEntry(h_mr[4], "Lead", "p");
    legend->Draw("Lsame");

    //Save final plot as pdf
    canvas->SaveAs(out_file_name+".pdf");
    canvas->SaveAs(out_file_name+".png");
    return canvas;
}

void plot_d2_simul_compare(){
    gStyle->SetOptStat(0);

    //output directory
    TString output_location = "output_"+fwd_rec+"/"+torus_pol;
    TString out_file_name;
    TString var;

    //array for files and inputs
    TString vars_to_plot[8] = {"e_nu", "e_q2", "e_w2", "e_phi", "e_x_b", "pi_zh", "pi_pt2", "pi_phi_PQ"};
    TFile* input_files_inb[5];
    TH1D* acc_nu[5];

    //After cuts
    for (int v=0; v<8; v++){
        var = vars_to_plot[v];
        for (int i = 0; i < 5; ++i){
            input_files_inb[i] = new TFile(output_location+"/acc/"+detector+"/D2/0000"+(i+1)+"1/out_clas12.root","READ");
            acc_nu[i] = (TH1D*)input_files_inb[i]->Get(var);
        }
        out_file_name = "output_"+fwd_rec+"/"+torus_pol+"/acc/"+detector+"/D2/"+var;
        plot_same_canvas_inb(acc_nu, out_file_name, var);
    }

    for (int v=0; v<8; v++){
        var = vars_to_plot[v];
        for (int i = 0; i < 5; ++i){
            input_files_inb[i] = new TFile(output_location+"/thrown/"+detector+"/D2/0000"+(i+1)+"1/out_clas12.root","READ");
            acc_nu[i] = (TH1D*)input_files_inb[i]->Get(vars_to_plot[v]);
        }
        out_file_name = "output_"+fwd_rec+"/"+torus_pol+"/thrown/"+detector+"/D2/"+var;
        plot_same_canvas_inb(acc_nu, out_file_name, var);
    }

    //Before cuts
    for (int v=0; v<5; v++){
        var = "raw_"+vars_to_plot[v];
        for (int i = 0; i < 5; ++i){
            input_files_inb[i] = new TFile(output_location+"/acc/"+detector+"/D2/0000"+(i+1)+"1/out_clas12.root","READ");
            acc_nu[i] = (TH1D*)input_files_inb[i]->Get(var);
        }
        out_file_name = "output_"+fwd_rec+"/"+torus_pol+"/acc/"+detector+"/D2/"+var;
        plot_same_canvas_inb(acc_nu, out_file_name, var);
    }

    for (int v=0; v<5; v++){
        var = "raw_"+vars_to_plot[v];
        for (int i = 0; i < 5; ++i){
            input_files_inb[i] = new TFile(output_location+"/thrown/"+detector+"/D2/0000"+(i+1)+"1/out_clas12.root","READ");
            acc_nu[i] = (TH1D*)input_files_inb[i]->Get(var);
        }
        out_file_name = "output_"+fwd_rec+"/"+torus_pol+"/thrown/"+detector+"/D2/"+var;
        plot_same_canvas_inb(acc_nu, out_file_name, var);
    }
}