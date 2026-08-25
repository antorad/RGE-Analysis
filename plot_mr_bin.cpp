#include "include.h"

TString mainVar="";
TString hadron="";

TCanvas multiplot_inb(TFile* input_files[5]){
    //Canvas
    TCanvas* canvas = new TCanvas("canvas", "Grid of Plots", 1200, 1000);
    canvas->cd();

    // --- Define margins ---
    float left_margin   = 0.10;
    float right_margin  = 0.10;
    float bottom_margin = 0.10;
    float top_margin    = 0.10;

    // Compute pad sizes
    float pad_width  = (1.0 - left_margin - right_margin) / N_Nu;
    float pad_height = (1.0 - bottom_margin - top_margin) / N_Q2;

    // --- Plot limits ---
    float xmin, xmax, ymin, ymax;
    if (mainVar=="Zh"){xmin = 0; xmax = 1; ymin = 0; ymax = 1.6;}
    if (mainVar=="Pt2"){xmin = 0; xmax = 2.5; ymin = 0.5; ymax = 3.0;}
    if (mainVar=="Phi_PQ"){xmin = -3.15; xmax = 3.15; ymin = 0; ymax = 2.0;}

    // --- Create pads and plots ---
    for (int Nu_Counter = 0; Nu_Counter < N_Nu; ++Nu_Counter) {
        for (int Q2_Counter = 0; Q2_Counter < N_Q2; ++Q2_Counter) {
            int index = Nu_Counter * N_Q2 + Q2_Counter;

            // Pad position
            float xlow = left_margin + Nu_Counter * pad_width;
            float xup  = xlow + pad_width;
            float ylow = bottom_margin + Q2_Counter * pad_height;
            float yup  = ylow + pad_height;

            TPad* pad = new TPad(Form("pad_%d", index), "", xlow, ylow, xup, yup);
            pad->SetNumber(index + 1);

            // Set margins: 0 for internal pads
            float lm = (Nu_Counter == 0) ? 0.15 : 0.00;
            float rm = (Nu_Counter == N_Nu - 1) ? 0.05 : 0.00;
            float bm = (Q2_Counter == 0) ? 0.15 : 0.00;
            float tm = (Q2_Counter == N_Q2 - 1) ? 0.05 : 0.00;

            pad->SetLeftMargin(lm);
            pad->SetRightMargin(rm);
            pad->SetBottomMargin(bm);
            pad->SetTopMargin(tm);
            pad->Draw();
            pad->cd();

            // Create histogram
            for (int i = 0; i < 5; ++i){
                cout<<"plotting bin "<<Q2_Counter<<"_"<<Nu_Counter<<endl;
                TH1D* h = (TH1D*)input_files[i]->Get(Form("MR_corr_%i_%i",Nu_Counter,Q2_Counter));
                h->SetMinimum(ymin);
                h->SetMaximum(ymax);

                // Axis style
                h->GetXaxis()->SetTitleOffset(0.9);
                h->GetYaxis()->SetTitleOffset(0.9);
                h->GetXaxis()->SetLabelSize(0.05);
                h->GetYaxis()->SetLabelSize(0.05);
                h->GetXaxis()->SetTitleSize(0.05);
                h->GetYaxis()->SetTitleSize(0.05);

                //Set colors
                if (i==0){
                    h->SetMarkerColor(1);
                    h->SetLineColor(1);}
                if (i==1){
                    h->SetMarkerColor(2);
                    h->SetLineColor(2);}
                if (i==2){
                    h->SetMarkerColor(4);
                    h->SetLineColor(4);}
                if (i==3){
                    h->SetMarkerColor(209);
                    h->SetLineColor(209);}
                if (i==4){
                    h->SetMarkerColor(205);
                    h->SetLineColor(205);}

                // Hide internal axis labels
                if (Nu_Counter != 0) h->GetYaxis()->SetLabelSize(0);
                if (Q2_Counter != 0) h->GetXaxis()->SetLabelSize(0);

                h->Draw("same");
            }
            canvas->cd();
        }
    }
    // --- Draw column and row titles ---
    TLatex latex;
    latex.SetTextSize(0.020);
    latex.SetTextAlign(22); // center alignment

    // Column titles (top)
    for (int j = 0; j < N_Nu; ++j) {
        float x_center = left_margin + (j + 0.5) * pad_width;
        float y_top = 1.0 - top_margin + 0.025;
        TString title = Form("%.1f<Nu<%.1f", Nu_bins[j], Nu_bins[j+1]);
        latex.DrawLatexNDC(x_center, y_top, title);
    }

    // Row titles (right)
    latex.SetTextAngle(90); // vertical
    for (int i = 0; i < N_Q2; ++i) {
        float y_center = bottom_margin + (i + 0.5) * pad_height;
        float x_right = left_margin + N_Nu * pad_width + 0.03;
        TString title = Form("%.1f<Q^{2}<%.1f", Q2_bins[i], Q2_bins[i+1]); // reverse order if desired
        latex.DrawLatexNDC(x_right, y_center, title);
    }

       // --- Global axis titles ---
    latex.SetTextAngle(0);
    latex.SetTextAlign(22);
    latex.SetTextSize(0.03);
    latex.DrawLatexNDC(0.8, bottom_margin - 0.03, mainVar);

    latex.SetTextAngle(90);
    if (hadron=="pion"){latex.DrawLatexNDC(left_margin - 0.03, 0.8, "#frac{N_{A}#pi^{+}}{N_{D2}#pi^{+}}#frac{N_{D2}e^{-}}{N_{A}e^{-}}");}
    if (hadron=="pion_minus"){latex.DrawLatexNDC(left_margin - 0.03, 0.8, "#frac{N_{A}#pi^{-}}{N_{D2}#pi^{-}}#frac{N_{D2}e^{-}}{N_{A}e^{-}}");}

    //Save plot as pdf
    canvas->SaveAs("output_"+fwd_rec+"/"+torus_pol+"/AC_MR/mr_binbybin_"+hadron+"_"+mainVar+".pdf");
    return canvas;
}

TCanvas multiplot_out(TFile* input_files[2]){
    //Canvas
    TCanvas* canvas = new TCanvas("canvas", "Grid of Plots", 1200, 1000);
    canvas->cd();

    // --- Define margins ---
    float left_margin   = 0.10;
    float right_margin  = 0.10;
    float bottom_margin = 0.10;
    float top_margin    = 0.10;

    // Compute pad sizes
    float pad_width  = (1.0 - left_margin - right_margin) / N_Nu;
    float pad_height = (1.0 - bottom_margin - top_margin) / N_Q2;

    // --- Plot limits ---
    float xmin, xmax, ymin, ymax;
    if (mainVar=="Zh"){xmin = 0; xmax = 1; ymin = 0; ymax = 1.6;}
    if (mainVar=="Pt2"){xmin = 0; xmax = 2.5; ymin = 0.5; ymax = 3.0;}
    if (mainVar=="Phi_PQ"){xmin = -3.15; xmax = 3.15; ymin = 0; ymax = 2.0;}

    // --- Create pads and plots ---
    for (int Nu_Counter = 0; Nu_Counter < N_Nu; ++Nu_Counter) {
        for (int Q2_Counter = 0; Q2_Counter < N_Q2; ++Q2_Counter) {
            int index = Nu_Counter * N_Q2 + Q2_Counter;

            // Pad position
            float xlow = left_margin + Nu_Counter * pad_width;
            float xup  = xlow + pad_width;
            float ylow = bottom_margin + Q2_Counter * pad_height;
            float yup  = ylow + pad_height;

            TPad* pad = new TPad(Form("pad_%d", index), "", xlow, ylow, xup, yup);
            pad->SetNumber(index + 1);

            // Set margins: 0 for internal pads
            float lm = (Nu_Counter == 0) ? 0.15 : 0.00;
            float rm = (Nu_Counter == N_Nu - 1) ? 0.05 : 0.00;
            float bm = (Q2_Counter == 0) ? 0.15 : 0.00;
            float tm = (Q2_Counter == N_Q2 - 1) ? 0.05 : 0.00;

            pad->SetLeftMargin(lm);
            pad->SetRightMargin(rm);
            pad->SetBottomMargin(bm);
            pad->SetTopMargin(tm);
            pad->Draw();
            pad->cd();

            // Create histogram
            for (int i = 0; i < 2; ++i){
                cout<<"plotting bin "<<Q2_Counter<<"_"<<Nu_Counter<<endl;
                TH1D* h = (TH1D*)input_files[i]->Get(Form("MR_corr_%i_%i",Nu_Counter,Q2_Counter));
                h->SetMinimum(ymin);
                h->SetMaximum(ymax);

                // Axis style
                h->GetXaxis()->SetTitleOffset(0.9);
                h->GetYaxis()->SetTitleOffset(0.9);
                h->GetXaxis()->SetLabelSize(0.05);
                h->GetYaxis()->SetLabelSize(0.05);
                h->GetXaxis()->SetTitleSize(0.05);
                h->GetYaxis()->SetTitleSize(0.05);

                //Set colors
                if (i==0){
                    h->SetMarkerColor(1);
                    h->SetLineColor(1);}
                if (i==1){
                    h->SetMarkerColor(205);
                    h->SetLineColor(205);}

                // Hide internal axis labels
                if (Nu_Counter != 0) h->GetYaxis()->SetLabelSize(0);
                if (Q2_Counter != 0) h->GetXaxis()->SetLabelSize(0);

                h->Draw("same");
            }
            canvas->cd();
        }
    }
    // --- Draw column and row titles ---
    TLatex latex;
    latex.SetTextSize(0.020);
    latex.SetTextAlign(22); // center alignment

    // Column titles (top)
    for (int j = 0; j < N_Nu; ++j) {
        float x_center = left_margin + (j + 0.5) * pad_width;
        float y_top = 1.0 - top_margin + 0.025;
        TString title = Form("%.1f<Nu<%.1f", Nu_bins[j], Nu_bins[j+1]);
        latex.DrawLatexNDC(x_center, y_top, title);
    }

    // Row titles (right)
    latex.SetTextAngle(90); // vertical
    for (int i = 0; i < N_Q2; ++i) {
        float y_center = bottom_margin + (i + 0.5) * pad_height;
        float x_right = left_margin + N_Nu * pad_width + 0.03;
        TString title = Form("%.1f<Q^{2}<%.1f", Q2_bins[i], Q2_bins[i+1]); // reverse order if desired
        latex.DrawLatexNDC(x_right, y_center, title);
    }

       // --- Global axis titles ---
    latex.SetTextAngle(0);
    latex.SetTextAlign(22);
    latex.SetTextSize(0.03);
    latex.DrawLatexNDC(0.8, bottom_margin - 0.03, mainVar);

    latex.SetTextAngle(90);
    if (hadron=="pion"){latex.DrawLatexNDC(left_margin - 0.03, 0.8, "#frac{N_{A}#pi^{+}}{N_{D2}#pi^{+}}#frac{N_{D2}e^{-}}{N_{A}e^{-}}");}
    if (hadron=="pion_minus"){latex.DrawLatexNDC(left_margin - 0.03, 0.8, "#frac{N_{A}#pi^{-}}{N_{D2}#pi^{-}}#frac{N_{D2}e^{-}}{N_{A}e^{-}}");}

    //Save plot as pdf
    canvas->SaveAs("output_"+fwd_rec+"/"+torus_pol+"/AC_MR/mr_binbybin_"+hadron+"_"+mainVar+".pdf");
    return canvas;
}

void plot_mr_bin(int Hadron_pid=211, TString var="Zh"){
	gStyle->SetOptStat(0);
    mainVar=var;

    //output directory
    TString output_location = "output_"+fwd_rec+"/"+torus_pol+"/AC_MR/";

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
    TH1D* h_mr_corr_inb[5];

    TString targets_out[2] = {"C", "Pb"};
    TFile* input_files_out[2];
    TH1D* h_mr_corr_out[2];

    if (torus_pol=="inb"){
        for (int i = 0; i < 5; ++i){
            input_files_inb[i] = new TFile(output_location+targets_inb[i]+"/mr_"+mainVar+"_"+hadron+".root","READ");
        }
        multiplot_inb(input_files_inb);
    }
    if (torus_pol=="out"){
        for (int i = 0; i < 2; ++i){
            input_files_out[i] = new TFile(output_location+targets_out[i]+"/mr_"+mainVar+"_"+hadron+".root","READ");
        }
        multiplot_out(input_files_out);
    }
}