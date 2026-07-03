#include "include.h"

void vertex_dependence(int run_N=000000, TString Target="C", TString type="data"){
    //Transform input run number to Tstring with correct number of digits
    TString run_N_str = TString::Format("%06d", run_N);

    gStyle->SetOptStat(0);

    //output directory
    TString output_location;
    if      (run_location=="own_pc"){output_location="output/"+type+"/"+Target+"/"+run_N_str;}
    else if (run_location=="farm"){output_location="/volatile/clas12/antorad/rge/MR_analysis/"+type+"/pass1/dc/"+Target+"/"+run_N_str;}

    //input directory
    TString input_location;
    if      (run_location=="own_pc"){input_location="output/"+type+"/"+Target+"/"+run_N_str;}
    else if (run_location=="farm"){input_location="/volatile/clas12/antorad/rge/MR_analysis/"+type+"/pass1/dc/"+Target+"/"+run_N_str;}

    //get input electron tuple created with simple_plots
    TFile *input = new TFile(input_location+"/out_clas12.root","READ");
    TNtuple* e_tuple = (TNtuple*)input->Get("elec_tuple");

    TFile *output = new TFile(output_location+"/vertex_dependance.root","RECREATE");
    output->cd();

    cout<<"Reading input"<<endl;
    //------Read branches with variables needed for cuts and plots------
    Float_t vz, theta, phi, p, sector;
    e_tuple->SetBranchAddress("vz",&vz);
    e_tuple->SetBranchAddress("theta",&theta);
    e_tuple->SetBranchAddress("phi",&phi);
    e_tuple->SetBranchAddress("p",&p);
    e_tuple->SetBranchAddress("sector",&sector);

    // Histogram array [p][theta]
    TH1F* h_vertex[6][10];
    TCanvas* canvas[6];
    TLegend* legend[6];

    for (int p_bin = 0; p_bin < 6; ++p_bin) {
        TString canvas_name = Form("canvas_%i", p_bin);
        canvas[p_bin] = new TCanvas(canvas_name, canvas_name,1000,600);
        for (int th_bin = 0; th_bin < 10; ++th_bin) {
            TString histo_name = Form("vertex_p%i_theta%i", p_bin, th_bin);
            h_vertex[p_bin][th_bin] = new TH1F(histo_name, histo_name, 300, -10, 5);
        }
    }

    // Loop entries
    Long64_t n_entries = e_tuple->GetEntries();
    for (Long64_t i=0;i<n_entries;i++) {
        e_tuple->GetEntry(i);
        theta=theta*57.2958; //rad to deg
        //dermine which bin the event belongs to
        int p_bin = int(p-2.);
        int th_bin = int((theta-8.)/2.);
        if (theta<28&&theta>8&&p<8&&p>2){h_vertex[p_bin][th_bin]->Fill(vz);}
    }

    //canvas
    int th_min;
    for (int i=0; i<6; i++){
        canvas[i]->cd();
        legend[i] = new TLegend(0.75, 0.50, 0.9, 0.9);
        for (int j=0; j<10; j++){
            if (h_vertex[i][j]->Integral()>0){
                h_vertex[i][j]->Scale(1.0/h_vertex[i][j]->Integral());
                h_vertex[i][j]->GetYaxis()->SetRangeUser(0,0.03);
                h_vertex[i][j]->GetYaxis()->SetTitle("Normalized count");
                h_vertex[i][j]->GetXaxis()->SetTitle("v_{z}");
                h_vertex[i][j]->SetLineColor(j*5+51);
                h_vertex[i][j]->Write();
                h_vertex[i][j]->SetTitle(Form("%i < p < %i GeV electrons", i+2, i+3));
                h_vertex[i][j]->Draw("HIST same");
                th_min=j*2+8;
                legend[i]->AddEntry(h_vertex[i][j], Form("%i<#theta<%i",th_min, th_min+2));
            }
        }
        legend[i]->Draw("same");
        canvas[i]->Write();
    }
    output->Close();
}