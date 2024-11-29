gStyle->SetOptStat(0);

void fancy_plots(TString Target="Pb", int Hadron_pid=211){
    ROOT::EnableImplicitMT();
    //output directory
    TString output_location = "output/"+Target+"/";

    //get TNtuple input created from simple_plots
    TFile *input = new TFile("output/"+Target+"/out_clas12.root","READ");
    TNtuple* hadron_tuple = (TNtuple*)input->Get("pi_ntuple");
    TNtuple* elec_tuple = (TNtuple*)input->Get("elec_tuple");

    //Get histograms to make the pretty
    TCanvas *canvas = new TCanvas("canvas","canvas",1000,600);
    TH1F* hist_pt2_d2 =(TH1F*)input->Get("pi_pt2_d2");
    TH1F* hist_pt2_sol =(TH1F*)input->Get("pi_pt2_sol");
    //Making them pretty
    canvas->SetLogy();
    canvas->SetGrid();
    //sol
    hist_pt2_sol->SetTitle("P_{T}^{2} for #pi^{+}");
    hist_pt2_sol->SetMarkerStyle(21);
    hist_pt2_sol->SetMarkerColor(4);
    hist_pt2_sol->SetLineColor(4);
    hist_pt2_sol->DrawNormalized("COLZ");
    //liq
    hist_pt2_d2->SetTitle("P_{T}^{2} for #pi^{+}");
    hist_pt2_d2->SetMarkerStyle(21);
    hist_pt2_d2->SetMarkerColor(2);
    hist_pt2_d2->SetLineColor(2);
    hist_pt2_d2->DrawNormalized("sameCOLZ");
    //TLegend
    TLegend* legend = new TLegend(0.7,0.8,0.9,0.9);
    legend->AddEntry(hist_pt2_d2, "Deuterium");
    legend->AddEntry(hist_pt2_sol, "Lead");
    legend->Draw("same");
    canvas->SaveAs("pt2_lead.pdf");

    //output
    TFile* output = new TFile("pt2_lead.root", "RECREATE");
    output->cd();
    hist_pt2_sol->Write();
    hist_pt2_d2->Write();
    canvas->Write();
}