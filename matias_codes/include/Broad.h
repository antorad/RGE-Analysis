#include "Binning.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TMath.h"
#include "TMultiGraph.h"
#include "TROOT.h"
#include "TStopwatch.h"
#include "TString.h"
#include <iostream>

TGraphErrors *TH1TOTGraph(TH1 *h1) {

    TGraphErrors *g1 = new TGraphErrors();
    if (!h1) {
        std::cout << "TH1TOTGraph: histogram not found !" << std::endl;
        return g1;
    }
    Double_t x, y, ex, ey;
    for (Int_t i = 1; i <= h1->GetNbinsX(); i++) {
        y = h1->GetBinContent(i);
        ey = h1->GetBinError(i);
        x = h1->GetBinCenter(i);
        // ex=h1->GetBinWidth(i);
        ex = 0.5 * h1->GetBinWidth(i);
        g1->SetPoint(i - 1, x, y);
        g1->SetPointError(i - 1, ex, ey);
    }
    return g1;
}

// Calculate the Pt broadening and plot in funcion of the size of the target
void PtBroadeningFullIntegrated(TString inputDirectory, TString plotDirectory,
                                TString inputName, TString outputName) {

    TFile *inputFile = new TFile(inputDirectory + inputName, "READ");
    TFile *outputFile = new TFile(outputDirectory + outputName, "RECREATE");
    gROOT->cd();
    // A^(1\3) C , Fe , Pb
    double nuclearSizes[N_STARGETS] = {
        TMath::Power(12.01, 1. / 3.), TMath::Power(26.98, 1. / 3.),
        TMath::Power(63.546, 1. / 3.), TMath::Power(118.71, 1. / 3.),
        TMath::Power(207.2, 1. / 3.)};

    for (int nPion = 1; nPion <= N_PION; nPion++) {
        for (int i = 0; i < N_STARGETS; i++) {

            TH1F *histSolid = (TH1F *)inputFile->Get(
                Form("meanPt2_%s_%i", SolTargets[i], nPion));
            TH1F *histLiquid = (TH1F *)inputFile->Get(
                Form("meanPt2_%s_%i", LiqTargets[i], nPion));
            // Calculate the Broadening (subtract of the means)
            TH1F *histBroadening =
                new TH1F(Form("histBroadening_%i", i), "", 1, Zh_MIN, Zh_MAX);
            histBroadening->Add(histSolid, histLiquid, 1, -1);

            // Set the points in TGraphErrors
            TGraphErrors *graph = new TGraphErrors();
            graph->SetPoint(0, nuclearSizes[i],
                            histBroadening->GetBinContent(1));
            graph->SetPointError(0, 0, histBroadening->GetBinError(1));

            outputFile->cd();
            graph->Write(
                Form("PtBroad_FullIntegrated_%s_%i", SolTargets[i], nPion));
            gROOT->cd();

            delete histSolid;
            delete histLiquid;
            delete histBroadening;
            delete graph;
        }
    } // End number pion event loop
    inputFile->Close();
    outputFile->Close();
}

void PtBroadeningZh(TString inputDirectory, TString outputDirectory) {

    TFile *inputFile = new TFile(inputDirectory + "meanPt2_Zh.root", "READ");
    TFile *outputFileHist =
        new TFile(outputDirectory + "Pt_broad_hist_Zh.root", "RECREATE");
    gROOT->cd();
    TFile *outputFile =
        new TFile(outputDirectory + "Pt_broad_z_h.root", "RECREATE");
    gROOT->cd();

    TString meanPt2;
    // if (UseCutOff == 1) {
    // meanPt2 = "meanPt2_%s_%i_interpolated";
    //}
    // if (UseCutOff == 2) {
    // meanPt2 = "meanPt2_%s_%i_clean";
    //}
    // if (UseCutOff == 3) {
    meanPt2 = "meanPt2_%s_%i";
    //}

    for (int nPion = 1; nPion <= N_PION;
         nPion++) {                            // Loops in every number of pion
        for (int i = 0; i < N_STARGETS; i++) { // Loops in the diffent targets

            TH1F *histSolid =
                (TH1F *)inputFile->Get(Form(meanPt2, SolTargets[i], nPion));
            TH1F *histLiquid =
                (TH1F *)inputFile->Get(Form(meanPt2, LiqTargets[i], nPion));

            TH1F *histBroadening =
                new TH1F(Form("histBroadening_%i", i), "", N_Zh, Zh_BINS);
            histBroadening->Add(histSolid, histLiquid, 1, -1);

            TGraphErrors *graph = (TGraphErrors *)TH1TOTGraph(histBroadening);
            outputFileHist->cd();
            histBroadening->Write(
                Form("PtBroad_z_h_%s_%i", SolTargets[i], nPion));
            outputFile->cd();
            graph->Write(Form("PtBroad_z_h_%s_%i", SolTargets[i], nPion - 1));
            gROOT->cd();

            delete histSolid;
            delete histLiquid;
            delete histBroadening;
            delete graph;
        }
    } // End number pion event loop
    inputFile->Close();
    outputFileHist->Close();
    outputFile->Close();
}

void PtBroadeningQ2(TString inputDirectory, TString outputDirectory) {

    TFile *inputFile = new TFile(inputDirectory + "meanPt2_Q2.root", "READ");
    TFile *outputFile =
        new TFile(outputDirectory + "Pt_broad_Q2.root", "RECREATE");
    gROOT->cd();

    TString meanPt2;
    meanPt2 = "meanPt2_%s_%i";

    for (int nPion = 1; nPion <= N_PION;
         nPion++) {                            // Loops in every number of pion
        for (int i = 0; i < N_STARGETS; i++) { // Loops in the diffent targets

            TH1F *histSolid =
                (TH1F *)inputFile->Get(Form(meanPt2, SolTargets[i], nPion));
            TH1F *histLiquid =
                (TH1F *)inputFile->Get(Form(meanPt2, LiqTargets[i], nPion));

            TH1F *histBroadening =
                new TH1F(Form("histBroadening_%i", i), "", N_Q2, Q2_BINS);
            histBroadening->Add(histSolid, histLiquid, 1, -1);

            TGraphErrors *graph = (TGraphErrors *)TH1TOTGraph(histBroadening);
            outputFile->cd();
            graph->Write(Form("PtBroad_Q2_%s_%i", SolTargets[i], nPion - 1));
            gROOT->cd();

            delete histSolid;
            delete histLiquid;
            delete histBroadening;
            delete graph;
        }
    } // End number pion event loop
    inputFile->Close();
    outputFile->Close();
}

void PtBroadeningNu(TString inputDirectory, TString outputDirectory) {

    TFile *inputFile = new TFile(inputDirectory + "meanPt2_Nu.root", "READ");
    TFile *outputFile =
        new TFile(outputDirectory + "Pt_broad_nu.root", "RECREATE");
    gROOT->cd();

    TString meanPt2;
    meanPt2 = "meanPt2_%s_%i";

    for (int nPion = 1; nPion <= N_PION;
         nPion++) {                            // Loops in every number of pion
        for (int i = 0; i < N_STARGETS; i++) { // Loops in the diffent targets

            TH1F *histSolid =
                (TH1F *)inputFile->Get(Form(meanPt2, SolTargets[i], nPion));
            TH1F *histLiquid =
                (TH1F *)inputFile->Get(Form(meanPt2, LiqTargets[i], nPion));

            TH1F *histBroadening =
                new TH1F(Form("histBroadening_%i", i), "", N_Nu, Nu_BINS);
            histBroadening->Add(histSolid, histLiquid, 1, -1);

            TGraphErrors *graph = (TGraphErrors *)TH1TOTGraph(histBroadening);
            outputFile->cd();
            graph->Write(Form("PtBroad_nu_%s_%i", SolTargets[i], nPion - 1));
            gROOT->cd();

            delete histSolid;
            delete histLiquid;
            delete histBroadening;
            delete graph;
        }
    } // End number pion event loop

    inputFile->Close();
    outputFile->Close();
}
