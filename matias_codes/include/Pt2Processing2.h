#include "Integration2.h"
#include "TF1.h"
#include "TH1.h"
#include "TROOT.h"
#include "TString.h"

const double ChiWeight = 0.95;
const double ndfWeight = 0.05;
const int N_FITS = 15;
const double maxChiQndf = 7;

double getCutoff(double *ChiArr, double *ndfArr, double *cutoff, int arrSize) {

    double TotChi = 0;
    double TotNdf = 0;
    double TotWeight = 0;
    double weight = 0;
    double weightSumCutoff = 0;

    // VARIABLES TOTAL WEIGHT LOOP
    for (int i = 0; i < arrSize; i++) {
        if (TMath::IsNaN(cutoff[i]) || cutoff[i] == 0) {
            continue;
        }
        TotChi += ChiArr[i];
        TotNdf += ndfArr[i];
    }
    // END VARIABLES TOTAL WEIGHT LOOP

    // SUM WEIGHT*PT2 LOOP
    for (int i = 0; i < arrSize; i++) {
        if (TMath::IsNaN(cutoff[i]) || cutoff[i] == 0) {
            continue;
        }
        weight =
            (TMath::Gaus(ChiArr[i], 1, 0.2) * ChiWeight + (ndfArr[i] / TotNdf) * ndfWeight) /
            (ChiWeight + ndfWeight);

        // std::cout << "ChiSQndf = " << ChiArr[i] << "  ndf = " << ndfArr[i]
        //<< " weight = " << weight << " Cutoff = " << cutoff[i] << std::endl;

        TotWeight += weight;
        weightSumCutoff += weight * cutoff[i];
    }
    // END SUM WEIGHT*PT2 LOOP

    double finalCutoff = weightSumCutoff / TotWeight;
    std::cout << "weighted Pt2 cutoff = " << finalCutoff << std::endl;
    return finalCutoff;
}

void GetCleanPt2Distribution(TH1F *hist, double cutoff) {

    int cutoffBin;
    // Search in which bin of Pt2 is the cutoff Value
    for (int binCounter = 1; binCounter <= hist->GetNbinsX(); binCounter++) {
        if (hist->GetBinCenter(binCounter) > cutoff) {
            cutoffBin = binCounter;
            break;
        }
    }

    for (int binCounter = cutoffBin; binCounter <= hist->GetNbinsX(); binCounter++) {
        hist->SetBinContent(binCounter, 0.);
        hist->SetBinError(binCounter, 0.);
    }
}

double GetInterpolatedPt2Point(double x, double prevBin, double nextBin, double prevBinCenter,
                               double nextBinCenter) {

    double m = (nextBin - prevBin) / (nextBinCenter - prevBinCenter);
    double yRef = prevBin;
    double xRef = prevBinCenter;

    return (x - xRef) * m + yRef; // Returns straigh line interpolation
}

double GetInterpolatedPt2PointError(double x, double prevBinError, double nextBinError,
                                    double prevBinCenter, double nextBinCenter) {

    double m = (nextBinError - prevBinError) / (nextBinCenter - prevBinCenter);
    double yRef = prevBinError;
    double xRef = prevBinCenter;

    return (x - xRef) * m + yRef; // Returns straigh line interpolation
}

void GetInterpolatedPt2Distribution(TH1F *hist) {

    for (int binCounter = 1; binCounter <= hist->GetNbinsX();
         binCounter++) {                            // Loops in every bin
        if (hist->GetBinContent(binCounter) == 0) { // If the bin if empty
                                                    // Save the values next to the bin
            double prevBin = hist->GetBinContent(binCounter - 1);
            double nextBin = hist->GetBinContent(binCounter + 1);

            int counter = 0;
            int exitCounter = 0;
            while (nextBin == 0.) { // While the next bin if empty
                counter++;
                nextBin = hist->GetBinContent(binCounter + 1 + counter);
                if (binCounter + 1 + counter >= N_Pt2) { // Finish it in the next bin
                    exitCounter++;                       // If there is any not empy bin before
                    break;
                }
            }

            if (exitCounter != 0) {
                break;
            }

            double prevBinError = hist->GetBinError(binCounter - 1);
            double nextBinError = hist->GetBinError(binCounter + 1 + counter);
            double prevBinCenter = hist->GetBinCenter(binCounter - 1);
            double nextBinCenter = hist->GetBinCenter(binCounter + 1 + counter);
            double missingBinCenter = hist->GetBinCenter(binCounter);

            double interpolatedPt2 = GetInterpolatedPt2Point(
                missingBinCenter, prevBin, nextBin, prevBinCenter, nextBinCenter);
            hist->SetBinContent(binCounter, interpolatedPt2);

            double interpolatedPt2Error = GetInterpolatedPt2PointError(
                missingBinCenter, prevBinError, nextBinError, prevBinCenter, nextBinCenter);
            hist->SetBinError(binCounter, interpolatedPt2Error);
        }
    }
}

void fitAndCutoff(std::string target, TFile *inputFile, TFile *outputFile) {

    std::cout << "Start" << std::endl;
    int n = target.length();
    char targetArr[n + 1];
    strcpy(targetArr, target.c_str());

    TH1F *meanPt2 = new TH1F("meanPt2", "", N_Zh, Zh_BINS);
    TH1F *meanPt2Clean = new TH1F("meanPt2Clean", "", N_Zh, Zh_BINS);
    TH1F *meanPt2CleanInterpolated = new TH1F("meanPt2CleanInterpolated", "", N_Zh, Zh_BINS);
    TH1F *hist, *histCutoff, *histInterpolated;

    for (int nPion = 1; nPion <= N_PION; nPion++) { // Loops in number of pion

        for (int Q2Counter = 0; Q2Counter < N_Q2; Q2Counter++) {         // Loops in Zh Bins
            for (int NuCounter = 0; NuCounter < N_Nu; NuCounter++) {     // Loops in Zh Bins
                for (int ZhCounter = 0; ZhCounter < N_Zh; ZhCounter++) { // Loops in Zh Bins

                    std::cout << "_______________Bin = NPion: " << nPion << " Zh:" << ZhCounter
                              << " ____________________" << std::endl;

                    hist =
                        (TH1F *)inputFile->Get(Form("corr_data_Pt2_%s_%i%i%i_%i", targetArr,
                                                    Q2Counter, NuCounter, ZhCounter, nPion));

                    if (hist == NULL) {
                        continue;
                    }
                    if (EmptyHist(hist) == 1) {
                        continue;
                    }

                    histCutoff = (TH1F *)hist->Clone();

                    double ChiArr[N_FITS];
                    double ndfArr[N_FITS];
                    double cutoff[N_FITS];

                    // START FIT LOOP
                    int ValidFits = 0;
                    double MinFitRange, MaxFitRange;

                    for (int i = 0; i < N_FITS; i++) {

                        MinFitRange = i * Delta_Pt2;
                        MaxFitRange = Pt2_MAX;

                        TF1 *FFit = new TF1("FFit", "[0]*TMath::Exp(-x/[1])", MinFitRange,
                                            MaxFitRange);
                        FFit->SetParameter(0, histCutoff->GetBinContent(1));
                        FFit->SetParameter(1, 0.2);

                        histCutoff->Fit(FFit, "SRQ");

                        // REGION VALIDITY TEST FUNCTION
                        TF1 *FValid =
                            new TF1("FValid", "[0]*TMath::Exp(-x/[1])", MinFitRange, 1000);
                        FValid->SetParameter(0, FFit->GetParameter(0));
                        FValid->SetParameter(1, FFit->GetParameter(1));

                        // FIT QUALLITY CHECK
                        if ((FFit->GetChisquare() / FFit->GetNDF() < maxChiQndf)) {
                            // START REGION VALIDATION
                            if (FValid->GetX(1) >= MaxFitRange) {
                                cutoff[i] = MaxFitRange;
                            } else {
                                cutoff[i] = FFit->GetX(1);
                            }
                            // END REGION VALIDATION
                            ChiArr[i] = FFit->GetChisquare() / FFit->GetNDF();
                            ndfArr[i] = FFit->GetNDF();

                            outputFile->cd();
                            histCutoff->Write(Form("corr_data_Pt2_%s_%i%i%i_%i_fit%i",
                                                   targetArr, Q2Counter, NuCounter, ZhCounter,
                                                   nPion, i));
                            gROOT->cd();

                            ValidFits++;
                        } // End fit quality check
                        else {
                            cutoff[i] = 0;
                            ChiArr[i] = 0;
                            ndfArr[i] = 0;
                        }
                    } // End fits loop

                    // GETTING CUTOFF
                    double DefCutoff;
                    if (ValidFits == 0) {

                        outputFile->cd();
                        hist->Write(Form("corr_data_Pt2_%s_%i%i%i_%i", targetArr, Q2Counter,
                                         NuCounter, ZhCounter, nPion));
                        histCutoff->Write(Form("corr_data_Pt2_%s_%i%i%i_%i_clean", targetArr,
                                               Q2Counter, NuCounter, ZhCounter, nPion));

                        meanPt2->SetBinContent(ZhCounter + 1, hist->GetMean());
                        meanPt2->SetBinError(ZhCounter + 1, hist->GetMeanError());
                        meanPt2Clean->SetBinContent(ZhCounter + 1, histCutoff->GetMean());
                        meanPt2Clean->SetBinError(ZhCounter + 1, histCutoff->GetMeanError());

                        GetInterpolatedPt2Distribution(histCutoff);
                        histCutoff->Write(Form("corr_data_Pt2_%s_%i%i%i_%i_interpolated",
                                               targetArr, Q2Counter, NuCounter, ZhCounter,
                                               nPion));
                        gROOT->cd();

                        meanPt2CleanInterpolated->SetBinContent(ZhCounter + 1,
                                                                histCutoff->GetMean());
                        meanPt2CleanInterpolated->SetBinError(ZhCounter + 1,
                                                              histCutoff->GetMeanError());

                        continue;

                    } else if (ValidFits == 1) {
                        DefCutoff =
                            TMath::MaxElement(N_FITS, cutoff); // Use the only different than 0
                        std::cout << "Single fit completed. Cutoff = " << DefCutoff
                                  << std::endl;
                    } else {
                        DefCutoff = getCutoff(ChiArr, ndfArr, cutoff, N_FITS);
                        std::cout << "Multiple fit completed. Cutoff = " << DefCutoff
                                  << std::endl;
                    }
                    // END GETTING CUTOFF

                    // CLEANING
                    GetCleanPt2Distribution(histCutoff, DefCutoff);
                    histInterpolated =
                        (TH1F *)histCutoff->Clone(); // Create another copy of the histogram to
                                                     // interpolated it Interpolating
                    GetInterpolatedPt2Distribution(histInterpolated);

                    outputFile->cd();

                    hist->Write(Form("corr_data_Pt2_%s_%i%i%i_%i", targetArr, Q2Counter,
                                     NuCounter, ZhCounter, nPion));
                    histCutoff->Write(Form("corr_data_Pt2_%s_%i%i%i_%i_clean", targetArr,
                                           Q2Counter, NuCounter, ZhCounter, nPion));
                    histInterpolated->Write(Form("corr_data_Pt2_%s_%i%i%i_%i_interpolated",
                                                 targetArr, Q2Counter, NuCounter, ZhCounter,
                                                 nPion));

                    meanPt2->SetBinContent(ZhCounter + 1, hist->GetMean());
                    meanPt2->SetBinError(ZhCounter + 1, hist->GetMeanError());
                    meanPt2Clean->SetBinContent(ZhCounter + 1, histCutoff->GetMean());
                    meanPt2Clean->SetBinError(ZhCounter + 1, histCutoff->GetMeanError());
                    meanPt2CleanInterpolated->SetBinContent(ZhCounter + 1,
                                                            histInterpolated->GetMean());
                    meanPt2CleanInterpolated->SetBinError(ZhCounter + 1,
                                                          histInterpolated->GetMeanError());

                    gROOT->cd();
                    hist->Reset();
                    histCutoff->Reset();
                    histInterpolated->Reset();
                } // END ZH LOOP
            }
        }

        outputFile->cd();

        meanPt2->Write(Form("meanPt2_%s_%i", targetArr, nPion));
        meanPt2Clean->Write(Form("meanPt2_%s_%i_clean", targetArr, nPion));
        meanPt2CleanInterpolated->Write(Form("meanPt2_%s_%i_interpolated", targetArr, nPion));

        gROOT->cd();

        meanPt2->Reset();
        meanPt2Clean->Reset();
        meanPt2CleanInterpolated->Reset();
    } // End numbers of pion loop

    delete meanPt2;
    delete meanPt2Clean;
    delete meanPt2CleanInterpolated;
    delete hist;
    delete histCutoff;
    delete histInterpolated;
}

void CallPt2Processing(TString inputDirectory, TString outputDirectory) {

    TStopwatch t2;

    TFile *inputFile = new TFile(inputDirectory + "corr_data_Pt2.root", "READ");
    TFile *outputFile =
        new TFile(outputDirectory + "corr_data_Pt2_processed.root", "RECREATE");
    gROOT->cd();

    std::cout << "Start cutoff and interpolation" << std::endl;
    for (int i = 0; i < N_STARGETS; i++) {
        fitAndCutoff(SolTargets[i], inputFile, outputFile);
        std::cout << SolTargets[i] << " target is Done!" << std::endl;
        fitAndCutoff(LiqTargets[i], inputFile, outputFile);
        std::cout << LiqTargets[i] << " target is Done!" << std::endl;
    }

    inputFile->Close();
    outputFile->Close();

    t2.Print();
}
