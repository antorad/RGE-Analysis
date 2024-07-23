//Cuts for the ṕlots
TCut Beta_cut="(beta>0)&&(beta<1.2)";
TCut P_cut="(p>0)&&(p<12)";
TCut vz_d2="(v_z>-8.64)&&(v_z<-2.78)";
TCut vz_solid="(v_z>-2.78)&&(v_z<1.04)";
TCut DIS_cut="(Q2>1)&&(sqrt(W2)>2)&&(y_bjorken<0.85)";

void mr_varaible(TString var, int nbins, float xmin, float xmax, TString hadron,
                    TNtuple* h_tuple, TNtuple* e_tuple, TString output_location){

    //create canvas for the plots
    TCanvas *canvas= new TCanvas("canvas","canvas",1000,600);
    canvas->cd();

    //Counting of the number of electron in each target
    int n_e_d2 = e_tuple->Draw("v_z>>h_e_d2", Beta_cut&&P_cut&&DIS_cut&&vz_d2, "goff");
    int n_e_solid = e_tuple->Draw("v_z>>h_e_solid", Beta_cut&&P_cut&&DIS_cut&&vz_solid, "goff");
    std::cout << "number of elec in d2 = " << n_e_d2 << std::endl;
    std::cout << "number of elec in solid = " << n_e_solid << std::endl;

    //Histogram for the Zh distibution for number of electrons.
    TH1F *elec_d2_zh_hist = new TH1F("elec_d2_zh_hist", "", nbins, xmin, xmax);
    TH1F *elec_solid_zh_hist = new TH1F("elec_solid_zh_hist", "", nbins, xmin, xmax);

    for (int i = 1; i <= nbins; i++) {
        elec_d2_zh_hist->SetBinContent(i, n_e_d2);
        elec_solid_zh_hist->SetBinContent(i, n_e_solid);
    }

    //Zh histogram for pions
    //deuterium target
    h_tuple->Draw(var+">>"+var+Form("_d2(%i, %f, %f)", nbins, xmin, xmax),
                        Beta_cut&&P_cut&&DIS_cut&&vz_d2, "COLZ");
    TH1F *h_zh_d2_hist = (TH1F *)gDirectory->GetList()->FindObject(var + "_d2");
    //solid target
    h_tuple->Draw(var+">>"+var+Form("_solid(%i, %f, %f)", nbins, xmin, xmax),
                     Beta_cut&&P_cut&&DIS_cut&&vz_solid, "COLZ");
    TH1F *h_zh_solid_hist = (TH1F *)gDirectory->GetList()->FindObject(var + "_solid");

    //Error propagation
    h_zh_d2_hist->Sumw2();
    h_zh_solid_hist->Sumw2();
    elec_d2_zh_hist->Sumw2();
    elec_solid_zh_hist->Sumw2();

    //Multiplicity and multiplicity ratio histograms
    TH1F *m_d2_zh = new TH1F("Multiplicity D2" + var, "Multiplicity D2" + var, nbins, xmin, xmax);
    TH1F *m_solid_zh = new TH1F("Multiplicity Solid" + var, "Multiplicity solid" + var,
                                nbins, xmin, xmax);
    TH1F *mr = new TH1F("Multiplicity ratio" + var, "Multiplicity ratio" + var, nbins, xmin, xmax);

    //Error propagation
    m_d2_zh->Sumw2();
    m_solid_zh->Sumw2();
    mr->Sumw2();

    //Calculation of multiplicity and multiplicity ratio  
    //For some reason by using: mr->Divide(m_solid_zh, m_d2_zh, n_e_d2, n_e_solid)
    //gives an incorrect calculation. TODO Check why at some point.
    m_d2_zh->Divide(h_zh_d2_hist, elec_d2_zh_hist);
    m_solid_zh->Divide(h_zh_solid_hist, elec_solid_zh_hist);
    mr->Divide(m_solid_zh, m_d2_zh);
    mr->GetXaxis()->SetTitle(var);
    mr->GetYaxis()->SetTitle("#frac{N_{A}#pi^{+}}{N_{D2}#pi^{+}}#frac{N_{D2}e^{-}}{N_{A}e^{-}}");
    mr->SetTitle("Multiplicity Ratio");
    mr->SetMarkerStyle(21);
    mr->Draw("COLZ");
    canvas->SaveAs(output_location+"mr_"+var+"_"+hadron+".pdf");

    std::cout << var <<" MR finished" << std::endl;

}

void calculate_mr(TString Target="C", int Hadron_pid=211){
    //output directory
    TString output_location = "output/"+Target+"/";

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

	//get TNtuple input created from simple_plots
	TFile *input = new TFile("output/"+Target+"/out_clas12.root","READ");
	TNtuple* hadron_tuple = (TNtuple*)input->Get(hadron+"_ntuple");
	TNtuple* elec_tuple = (TNtuple*)input->Get("elec_tuple");

    mr_varaible("z_h", 10, 0., 1., hadron, hadron_tuple, elec_tuple, output_location);

//TODO include electron variable mr (different factor can be applied bin by bin)
//TODO make code to save plots into root file
//TODO make code to plots all MR in same canvas
}