void calculate_mr(TString Target="C", int Hadron_pid=211){
	//get TNtuple input created from simple_plots
	TFile *input = new TFile("output/"+Target+"/out_clas12.root","READ");
	TNtuple* pion_tuple = (TNtuple*)input->Get("pion_ntuple");
	TNtuple* elec_tuple = (TNtuple*)input->Get("elec_tuple");

    //create canvas for the plots
	TCanvas *canvas= new TCanvas("canvas","canvas",1000,600);
	canvas->cd();

    //Multiplicity ratio variable, nbins and ranges
    TString var = "z_h";
    Int_t n_bins = 10;
    Float_t zh_min = 0.;
    Float_t zh_max = 1.;

    //Cuts for the ṕlots
	TCut Beta_cut="(beta>0)&&(beta<1.2)";
	TCut P_cut="(p>0)&&(p<12)";
	TCut vz_d2="(v_z>-8.64)&&(v_z<-2.78)";
	TCut vz_solid="(v_z>-2.78)&&(v_z<1.04)";
	TCut DIS_cut="(Q2>1)&&(sqrt(W2)>2)&&(y_bjorken<0.85)";

    //Counting of the number of electron in each target
    int n_e_d2 = elec_tuple->Draw("v_z>>h_e_d2", Beta_cut&&P_cut&&DIS_cut&&vz_d2, "goff");
    int n_e_solid = elec_tuple->Draw("v_z>>h_e_solid", Beta_cut&&P_cut&&DIS_cut&&vz_solid, "goff");
    std::cout << "number of elec in d2 = " << n_e_d2 << std::endl;
    std::cout << "number of elec in solid = " << n_e_solid << std::endl;

    //Histogram for the Zh distibution for number of electrons.
    TH1F *elec_d2_zh_hist = new TH1F("elec_d2_zh_hist", "", n_bins, zh_min, zh_max);
    TH1F *elec_solid_zh_hist = new TH1F("elec_solid_zh_hist", "", n_bins, zh_min, zh_max);

    for (int i = 1; i <= n_bins; i++) {
        elec_d2_zh_hist->SetBinContent(i, n_e_d2);
        elec_solid_zh_hist->SetBinContent(i, n_e_solid);
    }

    //Zh histogram for pions
    //deuterium target
    pion_tuple->Draw(var+">>"+var+Form("_d2(%i, %f, %f)", n_bins, zh_min, zh_max),
    					Beta_cut&&P_cut&&DIS_cut&&vz_d2, "COLZ");
    TH1F *pion_zh_d2_hist = (TH1F *)gDirectory->GetList()->FindObject(var + "_d2");
    //solid target
    pion_tuple->Draw(var+">>"+var+Form("_solid(%i, %f, %f)", n_bins, zh_min, zh_max),
                     Beta_cut&&P_cut&&DIS_cut&&vz_solid, "COLZ");
    TH1F *pion_zh_solid_hist = (TH1F *)gDirectory->GetList()->FindObject(var + "_solid");

    //Error propagation
    pion_zh_d2_hist->Sumw2();
    pion_zh_solid_hist->Sumw2();
    elec_d2_zh_hist->Sumw2();
    elec_solid_zh_hist->Sumw2();

    //Multiplicity and multiplicity ratio histograms
    TH1F *m_d2_zh = new TH1F("Multiplicity D2" + var, "Multiplicity D2" + var, n_bins, zh_min, zh_max);
    TH1F *m_solid_zh = new TH1F("Multiplicity Solid" + var, "Multiplicity solid" + var,
    							n_bins, zh_min, zh_max);
    TH1F *mr_zh = new TH1F("Multiplicity ratio" + var, "Multiplicity ratio" + var, n_bins, zh_min, zh_max);

    //Error propagation
    m_d2_zh->Sumw2();
    m_solid_zh->Sumw2();
    mr_zh->Sumw2();

    //Calculation of multiplicity and multiplicity ratio  
    //For some reason by using:mr_zh->Divide(m_solid_zh, m_d2_zh, n_e_d2, n_e_solid)
    //gives an incorrect calculation. Check why at some point.
    m_d2_zh->Divide(pion_zh_d2_hist, elec_d2_zh_hist);
    m_solid_zh->Divide(pion_zh_solid_hist, elec_solid_zh_hist);
    mr_zh->Divide(m_solid_zh, m_d2_zh);
    mr_zh->Draw("COLZ");
    canvas->SaveAs(Target+"_mr_zh.pdf");

    std::cout << "Zh MR finished" << std::endl;
}