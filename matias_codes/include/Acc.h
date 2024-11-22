#include <iostream>
#include <string>
#include "Binning.h"
#include "TMath.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TNtuple.h"
#include "TVector2.h"
#include "TStopwatch.h"
#include "TROOT.h"
#include "TH1.h"
#include "TCut.h"
#include "TEventList.h"

// If the histogram if empty return 1 if not return 0
int EmptyHist(TH1F* h) {

  int empty = 0;
  for(int i = 1 ; i <= h->GetNbinsX() ; i++) {
    if(h->GetBinContent(i) == 0){ empty++; }
  }
  if(empty == h->GetNbinsX()) { return 1; }
  else { return 0; }

}

//TString Set3DBinCut(int Q2Bin, int NuBin, int ZhBin, const char* dataType) {

    //char* addDataType;
    //if(dataType == "data") addDataType = "";
    //if(dataType == "generated")  addDataType = "_gen";
    //if(dataType == "reconstructed")  addDataType = "_rec";

    
    //TString Q2Cut       = Form("Q2>%f&&Q2<%f", Q2_BINS[Q2Bin], Q2_BINS[Q2Bin+1]);
    //TString NuCut       = Form("Nu>%f&&Nu<%f", Nu_BINS[Nubin], Nu_BINS[Nubin+1]);
    //TString ZhCut       = Form("Zh>%f&&Zh<%f", Zh_BINS[ZhBin], Zh_BINS[ZhBin+1]);
    //TString Q2Cut_gen   = Form("Q2_gen>%f&&Q2_gen<%f", Q2_BINS[Q2Bin], Q2_BINS[Q2Bin+1]);
    //TString NuCut_gen   = Form("Nu_gen>%f&&Nu_gen<%f", Nu_BINS[Nubin], Nu_BINS[Nubin+1]);
    //TString ZhCut_gen   = Form("Zh_gen>%f&&Zh_gen<%f", Zh_BINS[ZhBin], Zh_BINS[ZhBin+1]);
    //TString Q2Cut_rec   = Form("Q2_rec>%f&&Q2_rec<%f", Q2_BINS[Q2Bin], Q2_BINS[Q2Bin+1]);
    //TString NuCut_rec   = Form("Nu_rec>%f&&Nu_rec<%f", Nu_BINS[Nubin], Nu_BINS[Nubin+1]);
    //TString ZhCut_rec   = Form("Zh_rec>%f&&Zh_rec<%f", Zh_BINS[ZhBin], Zh_BINS[ZhBin+1]);

    //cutsData  = Q2Cut&&NuCut&&ZhCut&&YCCut&&VCData;
    //cutsSimul_gen = Q2Cut_gen&&NuCut_gen&&ZhCut_gen&&GenCut;
    //cutsSimul_rec = Q2Cut_rec&&NuCut_rec&&ZhCut_rec&&RecCut;

//}

// Check if the number of the correctly detected events is bigger than one
void AccCondition(TH1F* hist) {

  for(Int_t bin = 1; bin <= hist->GetNbinsX(); bin++) {
    if(hist->GetBinContent(bin) == 1) {
      hist->SetBinContent(bin, 0);
      hist->SetBinError(bin, 0);
    }
  }

}

// If Acceptance Factor > 1 set it to 0
void AccHist1(TH1F* hist) {

  for(Int_t bin = 1; bin <= hist->GetNbinsX(); bin++) {
    if(hist->GetBinContent(bin) >= 1) {
      hist->SetBinContent(bin, 0);
      hist->SetBinError(bin, 0);
    }
  }

}

void AccHist0To1(TH1F* hist) {

  for(Int_t bin = 1; bin <= hist->GetNbinsX(); bin++) {
    if(hist->GetBinContent(bin) == 0) {
      hist->SetBinContent(bin, 1);
      hist->SetBinError(bin, 0);
    }
  }

}


