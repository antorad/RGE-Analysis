void plot_mr_all(int Hadron_pid=211, TString var="z_h"){
    gStyle->SetOptStat(0);
    //output directory
    TString output_location = "output/";

    //hadron selection
    TString hadron;
    if (Hadron_pid==211){hadron="pion";}
    else if (Hadron_pid==-211){hadron="pion_minus";}
    else if (Hadron_pid==2212){hadron="proton";}
    else if (Hadron_pid==321){hadron="kaon";}
    //in case a pid not valid in given, give a message and end macro
    else {
        cout<<"PID not valid"<<endl;
        return;}

    //get TH1Fs from root files created with calculate_mr
    //input files
    TFile *input_C = new TFile(output_location+"/C/mr_clas12.root","READ");
    TFile *input_Al = new TFile(output_location+"/Al/mr_clas12.root","READ");
    TFile *input_Cu = new TFile(output_location+"/Cu/mr_clas12.root","READ");
    TFile *input_Sn = new TFile(output_location+"/Sn/mr_clas12.root","READ");
    TFile *input_Pb = new TFile(output_location+"/Pb/mr_clas12.root","READ");
    //mr histograms
    TH1F* mr_hist_C = (TH1F*)input_C->Get(hadron+"_"+var+"_mratio");
    TH1F* mr_hist_Al = (TH1F*)input_Al->Get(hadron+"_"+var+"_mratio");
    TH1F* mr_hist_Cu = (TH1F*)input_Cu->Get(hadron+"_"+var+"_mratio");
    TH1F* mr_hist_Sn = (TH1F*)input_Sn->Get(hadron+"_"+var+"_mratio");
    TH1F* mr_hist_Pb = (TH1F*)input_Pb->Get(hadron+"_"+var+"_mratio");

    //canvas
    TCanvas *canvas= new TCanvas("canvas","canvas",1000,600);
    canvas->cd();
    canvas->SetGrid();

    //set range on y axis for each case
    if (var=="z_h") {mr_hist_C->GetYaxis()->SetRangeUser(0,1.6);}
    if (var=="z_h" && hadron=="proton") {mr_hist_C->GetYaxis()->SetRangeUser(0,6);}
    if (var=="nu") {mr_hist_C->GetYaxis()->SetRangeUser(0.6,1.2);}
    if (var=="nu" && hadron=="proton") {mr_hist_C->GetYaxis()->SetRangeUser(1,4);}

    mr_hist_C->SetMarkerColor(1);
    mr_hist_Al->SetMarkerColor(2);
    mr_hist_Cu->SetMarkerColor(3);
    mr_hist_Sn->SetMarkerColor(4);
    mr_hist_Pb->SetMarkerColor(5);

    mr_hist_C->Draw("EX0same");
    mr_hist_Al->Draw("EX0same");
    mr_hist_Cu->Draw("EX0same");
    mr_hist_Sn->Draw("EX0same");
    mr_hist_Pb->Draw("EX0same");

    TLegend* legend = new TLegend(0.75, 0.75, 0.9, 0.9);
    legend->AddEntry(mr_hist_C, "Carbon", "p");
    legend->AddEntry(mr_hist_Al, "Aluminum", "p");
    legend->AddEntry(mr_hist_Cu, "Copper", "p");
    legend->AddEntry(mr_hist_Sn, "Tin", "p");
    legend->AddEntry(mr_hist_Pb, "Lead", "p");
    legend->Draw("same");

    canvas->SaveAs("output/mr_"+hadron+"_"+var+".pdf");
}