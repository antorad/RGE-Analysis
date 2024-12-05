gStyle->SetOptStat(0);

void fancy_plots(TString Target="Pb", int Hadron_pid=211){
    ROOT::EnableImplicitMT();
    //output directory
    TString output_location = "output/"+Target+"/";

    //get TNtuple input created from simple_plots
    TFile *input = new TFile("output/"+Target+"/out_clas12.root","READ");

    //Get histograms to make the pretty
    TCanvas *canvas = new TCanvas("canvas","canvas",1000,600);
    TH1F* hist_pt2_d2 =(TH1F*)input->Get("pi_pt2_d2");
    TH1F* hist_pt2_sol =(TH1F*)input->Get("pi_pt2_sol");
    //Making them pretty
    canvas->SetLogy();
    canvas->SetGrid();
    //liq
    hist_pt2_d2->SetTitle("P_{T}^{2} for #pi^{+}");
    hist_pt2_d2->SetMarkerStyle(21);
    hist_pt2_d2->SetMarkerColor(2);
    hist_pt2_d2->SetLineColor(2);
    hist_pt2_d2->Draw("sameCOLZ");
    //sol
    hist_pt2_sol->SetTitle("P_{T}^{2} for #pi^{+}");
    hist_pt2_sol->SetMarkerStyle(21);
    hist_pt2_sol->SetMarkerColor(4);
    hist_pt2_sol->SetLineColor(4);
    hist_pt2_sol->Draw("sameCOLZ");

    //TLegend
    TLegend* legend = new TLegend(0.7,0.8,0.9,0.9);
    legend->AddEntry(hist_pt2_d2, "Deuterium");
    legend->AddEntry(hist_pt2_sol, "Lead");
    legend->Draw("same");

    //output
    TFile* output = new TFile("pt2_lead.root", "RECREATE");
    output->cd();

    //Unnormalized version
    hist_pt2_sol->Write("pt2_solid");
    hist_pt2_d2->Write("pt2_liquid");
    canvas->SaveAs("pt2_lead.pdf");
    canvas->Write();

    //Normalized version
    hist_pt2_d2->DrawNormalized("COLZ");
    hist_pt2_sol->DrawNormalized("sameCOLZ");
    legend->Draw("same");
    canvas->Write();
    canvas->SaveAs("pt2_lead_nor.pdf");

    output->Close();
}