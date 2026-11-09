#include "include.h"

TCanvas plot_same_canvas_inb(TH1D* h_mr[3], TString out_file_name, TString var){
    //canvas
    TCanvas *canvas= new TCanvas("canvas","canvas",1000,600);
    canvas->cd();
    canvas->SetGrid();

    //Normalize distributions
    h_mr[0]->Scale(1.0/h_mr[0]->Integral());
    h_mr[1]->Scale(1.0/h_mr[1]->Integral());
    h_mr[2]->Scale(1.0/h_mr[2]->Integral());

    //set range on y axis for each case
    double maxY = std::max({
    h_mr[0]->GetMaximum(),
    h_mr[1]->GetMaximum(),
    h_mr[2]->GetMaximum()
    });
    h_mr[0]->GetYaxis()->SetRangeUser(0, maxY*1.1);

    //Set axis names
    h_mr[0]->GetYaxis()->SetTitle("Normalized Entries");
    h_mr[0]->GetXaxis()->SetTitle(var);

    //set colors
    h_mr[0]->SetLineColor(1);
    h_mr[1]->SetLineColor(2);
    h_mr[2]->SetLineColor(4);

    //draw plots without horizontal error bars
    h_mr[0]->Draw("HIST");
    h_mr[1]->Draw("HISTsame");
    h_mr[2]->Draw("HISTsame");

    //Draw legens in the top right corner
    TLegend* legend = new TLegend(0.75, 0.75, 0.9, 0.9);
    legend->AddEntry(h_mr[0], "Data", "l");
    legend->AddEntry(h_mr[1], "Acc", "l");
    legend->AddEntry(h_mr[2], "Thrown", "l");
    legend->Draw("same");

    //Save final plot as pdf
    canvas->SaveAs(out_file_name+".pdf");
    canvas->SaveAs(out_file_name+".png");
    return canvas;
}

void plot_data_simul_comapare(){
    gStyle->SetOptStat(0);

    //output directory
    TString output_location = "output_"+fwd_rec+"/"+torus_pol;
    TString out_file_name;
    TString var;

    //array for files and inputs
    TString vars_to_plot[8] = {"e_nu", "e_q2", "e_w2", "e_phi", "e_x_b", "pi_zh", "pi_pt2", "pi_phi_PQ"};
    TString Target[5] = {"C", "Al", "Cu", "Sn", "Pb"};
    TH1D* histo[3];

    TFile* data_input_files_inb;
    TFile* acc_input_files_inb;
    TFile* thrown_input_files_inb;

    //After cuts
    for (int v=0; v<8; v++){
        var = vars_to_plot[v];
        for (int i = 0; i < 5; ++i){
            data_input_files_inb  = new TFile(output_location+"/data/"+detector+"/"+Target[i]+"/out_clas12.root","READ");
            acc_input_files_inb   = new TFile(output_location+"/acc/"+detector+"/"+Target[i]+"/out_clas12.root","READ");
            thrown_input_files_inb = new TFile(output_location+"/thrown/"+detector+"/"+Target[i]+"/out_clas12.root","READ");

            histo[0]=(TH1D*)data_input_files_inb->Get(var);
            histo[1]=(TH1D*)acc_input_files_inb->Get(var);
            histo[2]=(TH1D*)thrown_input_files_inb->Get(var);

            out_file_name = "output_"+fwd_rec+"/"+torus_pol+"/data_simul_"+Target[i]+"_"+var;
            plot_same_canvas_inb(histo, out_file_name, var);
        }
    }

    for (int v=0; v<5; v++){
        var = "raw_"+vars_to_plot[v];
        for (int i = 0; i < 5; ++i){
            data_input_files_inb  = new TFile(output_location+"/data/"+detector+"/"+Target[i]+"/out_clas12.root","READ");
            acc_input_files_inb   = new TFile(output_location+"/acc/"+detector+"/"+Target[i]+"/out_clas12.root","READ");
            thrown_input_files_inb = new TFile(output_location+"/thrown/"+detector+"/"+Target[i]+"/out_clas12.root","READ");

            histo[0]=(TH1D*)data_input_files_inb->Get(var);
            histo[1]=(TH1D*)acc_input_files_inb->Get(var);
            histo[2]=(TH1D*)thrown_input_files_inb->Get(var);

            out_file_name = "output_"+fwd_rec+"/"+torus_pol+"/data_simul_"+Target[i]+"_"+var;
            plot_same_canvas_inb(histo, out_file_name, var);
        }
    }

}