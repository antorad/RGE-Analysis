// This code apply the two acceptance factors to the data and
// generate a PhiPq histogram for each bin of Q2, nu, z_h, p_t2
// The hadrons variables used were calculate taking the vectorial
// It can be compile with
// g++ -Wall -fPIC -I../include `root-config --cflags` AccCorrection.cpp -o
// ./bin/AccCorrection  `root-config --glibs` ./include/Acc.h For the target
// name use (C,Fe,Pb) for the solids targets and (DC,DFe,DPb) for the liquid
// target

#include "Acc.h"
#include <iostream>

int Data_Phi(std::string target, TFile *fileOutput);

int main() {

    TFile *fileOutput =
        new TFile(outputDirectory + "Data_Phi.root", "RECREATE");

    gROOT->cd();

    std::cout << "Acceptance Correction for C" << std::endl;
    Data_Phi("C", fileOutput);
    std::cout << "Acceptance Correction for Al" << std::endl;
    Data_Phi("Al", fileOutput);
    std::cout << "Acceptance Correction for Cu" << std::endl;
    Data_Phi("Cu", fileOutput);
    std::cout << "Acceptance Correction for Sn" << std::endl;
    Data_Phi("Sn", fileOutput);
    std::cout << "Acceptance Correction for Pb" << std::endl;
    Data_Phi("Pb", fileOutput);

    std::cout << "Acceptance Correction for DC" << std::endl;
    Data_Phi("DC", fileOutput);
    std::cout << "Acceptance Correction for DAl" << std::endl;
    Data_Phi("DAl", fileOutput);
    std::cout << "Acceptance Correction for DCu" << std::endl;
    Data_Phi("DCu", fileOutput);
    std::cout << "Acceptance Correction for Sn" << std::endl;
    Data_Phi("DSn", fileOutput);
    std::cout << "Acceptance Correction for DPb" << std::endl;
    Data_Phi("DPb", fileOutput);

    fileOutput->Close();
    return 0;
}

int Data_Phi(std::string target, TFile *fileOutput) {

    // if (argc != 2) {
    // std::cout << "Insert (just) the target name as a parameter"
    //<< std::endl;
    // return 0;
    //}

    TStopwatch t;

    Pt2_BINS[0] = 0.;
    for (int i = 1; i <= N_Pt2; i++) {
        Pt2_BINS[i] = Pt2_BINS[i - 1] + Delta_Pt2;
        // std::cout << "Pt Bin " << i << ": " << Pt2_BINS[i] << std::endl;
    }

    std::cout << "Start" << std::endl;

    // std::string target = argv[1];
    //  Creating a array of chars instead of a string to use Form method
    int n = target.length();
    char targetArr[n + 1];
    strcpy(targetArr, target.c_str());

    int m;
    TString fileDataName;
    // Select the data for the chosen solid target
    TCut vertexCut;
    // Premiliminary cuts to separate events from liquid and solid target
    float vz_d2_min = -8.01;
    float vz_d2_max = -3.62;
    float vz_solid_min = -1.84;
    float vz_solid_max = 0.09;

    if (targetArr[0] == 'D') {
        m = n;
        vertexCut =
            Form("p>0 && v_z_elec>%f && v_z_elec<%f", vz_d2_min, vz_d2_max);

    } else {
        m = n + 1;
        vertexCut = Form("p>0 && v_z_elec>%f && v_z_elec<%f", vz_solid_min,
                         vz_solid_max);
    }

    std::cout << "m" << m << std::endl;
    // Select the target of the simultion
    // char simulTarget[m];
    char solidTarget[m];
    solidTarget[m - 1] = '\0';
    int j;
    for (int i = 0; i < m; i++) {
        if (targetArr[0] == 'D')
            j = i + 1;
        else
            j = i;
        solidTarget[i] = targetArr[j];
        std::cout << "target : " << i << " " << targetArr[i] << std::endl;
        std::cout << "solid target : " << i << " " << solidTarget[i]
                  << std::endl;
    }
    // if (targetArr[0] == 'D') {
    // simulTarget[0] = 'D';
    // simulTarget[1] = '\0';
    //} else {
    // for (int i = 0; i < m; i++) {
    // simulTarget[i] = targetArr[i];
    //}
    //}

    // Open the input and output files
    fileDataName = Form(outputDirectory + "%s/out_clas12_211_%s.root",
                        solidTarget, solidTarget);
    TFile *fileData = new TFile(fileDataName, "READ");
    // TFile* fileSimul  = new
    // TFile(Form("/eos/user/m/mbarrial/Data/Acc/SimulTuple_%s.root",
    // simulTarget), "READ");
    // TFile *fileOutput = new TFile(
    // Form(outputDirectory + "%s/Data_Phi_%s.root", solidTarget, targetArr),
    //"RECREATE");
    gROOT->cd();

    // Create some variables to use inside the for loops
    TString tupleDataName;
    TCut Q2Cut, NuCut, ZhCut, Pt2Cut, VCData, cutsData, cutsSimul, GenCut,
        DecCut, GenDecCut;
    // Do not take in count events of 0 generated pion in the simultion
    // beacause the is not hadrons variables to select a bin
    // std::cout << Form("Simul target %s, Target %s", simulTarget,
    // targetArr)
    //<< " and " << VCData << std::endl;

    TCut DISCuts = "W2>4 && y_bjorken<0.85 && Q2>1.";

    // Create all the necessary histograms
    TH1F *histData = new TH1F("Data", "", N_Phi, -180, 180);

    // Store the sum of the weights A.K.A the erros (in the other histograms
    // if save it by other methods)
    histData->Sumw2();

    for (int gen = 1; gen <= N_PION;
         gen++) { // Loops in every number of generated pions
        for (int Q2Counter = 0; Q2Counter < N_Q2;
             Q2Counter++) { // Loops in every Q2 bin
            for (int NuCounter = 0; NuCounter < N_Nu;
                 NuCounter++) { // Loops in every nu bin
                for (int ZhCounter = 0; ZhCounter < N_Zh;
                     ZhCounter++) { // Loops in every z_h bin

                    std::cout << "Bin selected: " << gen << Q2Counter
                              << NuCounter << ZhCounter << std::endl;

                    // Select the cuts for each bin
                    Q2Cut = Form("Q2>%f&&Q2<%f", Q2_BINS[Q2Counter],
                                 Q2_BINS[Q2Counter + 1]);
                    NuCut = Form("nu>%f&&nu<%f", Nu_BINS[NuCounter],
                                 Nu_BINS[NuCounter + 1]);
                    ZhCut = Form("z_h>%f&&z_h<%f", Zh_BINS[ZhCounter],
                                 Zh_BINS[ZhCounter + 1]);

                    cutsData = Q2Cut && NuCut && ZhCut && vertexCut && DISCuts;
                    TNtuple *ntupleData =
                        (TNtuple *)fileData->Get("pion_ntuple");

                    // Apply the cuts to the ntuples to increces the
                    // efficiency
                    ntupleData->Draw(">>listData", cutsData);
                    TEventList *evntData =
                        (TEventList *)gDirectory->Get("listData");
                    ntupleData->SetEventList(evntData);

                    for (int Pt2Counter = 0; Pt2Counter < N_Pt2;
                         Pt2Counter++) { // Loops in every p_t2 bin

                        // Select the p_t2 bin
                        Pt2Cut = Form("p_t2>%f&&p_t2<%f", Pt2_BINS[Pt2Counter],
                                      Pt2_BINS[Pt2Counter + 1]);

                        ntupleData->Project("Data", "phi_PQ", Pt2Cut);
                        if (EmptyHist(histData) == 1) {
                            continue;
                        } // If there isn't any event in data skip this bin
                        // Generate histograms of the all dectected pion,
                        // all generated pion, and the pions that was
                        // correct dectected

                        // Save the histograms in the output file
                        fileOutput->cd();
                        histData->Write(Form("Data_%s_%i%i%i%i_%i", targetArr,
                                             Q2Counter, NuCounter, ZhCounter,
                                             Pt2Counter, gen));

                        gROOT->cd();

                        // Set the histograms values to 0
                        histData->Reset();

                    } // End p_t2 loop
                    delete ntupleData;
                    delete evntData;
                } // End Q2 loop
            }     // End nu loop
        }         // End z_h loop
    }             // End pion number loop
    // fileOutput->Close();
    fileData->Close();
    t.Print();

    return 0;
}
