//Cuts for the ṕlots
TCut Beta_cut="(beta>0)&&(beta<1.2)";
TCut P_cut="(p>0)&&(p<12)";
// Vertex cuts
TCut DIS_cut="(Q2>1)&&(sqrt(W2)>2)&&(y_bjorken<0.85)";
TCut vz_d2="(v_z>-8.01)&&(v_z<-3.62)";
TCut vz_solid="(v_z>-1.84)&&(v_z<0.09)";
TCut vz_d2_h="(v_z_elec>-8.01)&&(v_z_elec<-3.62)";
TCut vz_solid_h="(v_z_elec>-1.84)&&(v_z_elec<0.09)";

    //float vz_d2_min = -8.01;
    //float vz_d2_max = -3.62;
    //float vz_solid_min = -1.84;
    //float vz_solid_max = 0.09;

TH1F* make_var_histo(TString var, int nbins, float xmin, float xmax, TNtuple* h_tuple, TString target){
    //Assigning targtet cut for electron counting
    //because we need to check hadrons' corresponding electron vertex, we have to check different
    //variables depending on the tuple type, so the variables in the cut have different names 
    TCut target_cut;
    TString h_tuple_name = h_tuple->GetName();
    if (target=="d2" && h_tuple_name!="elec_tuple"){target_cut=vz_d2_h;}
    else if (target=="d2" && h_tuple_name=="elec_tuple"){target_cut=vz_d2;}
    else if (target=="solid" && h_tuple_name!="elec_tuple"){target_cut=vz_solid_h;}
    else if (target=="solid" && h_tuple_name=="elec_tuple"){target_cut=vz_solid;}

    //Histogram for the var distibution
    h_tuple->Draw(var+">>"+var+"_"+target+Form("(%i, %f, %f)", nbins, xmin, xmax),
                        Beta_cut&&P_cut&&DIS_cut&&target_cut, "COLZ");
    TH1F *h_d2_hist = (TH1F *)gDirectory->GetList()->FindObject(var+"_"+target);
    return h_d2_hist;
}

TH1F* make_var_ehisto(TString var, int nbins, float xmin, float xmax, TNtuple* e_tuple, TString target){
    // check if the variable is an electron variable or not
    //if it is an electron variable, make an histo as same as the hadron
    TH1F* elec_hist;
    if (var=="Q2" || var=="nu"){
        elec_hist = make_var_histo(var, nbins, xmin, xmax, e_tuple, target);
        elec_hist->SetName(var+"_"+target+"_e");
    }
    //if it is an hadron variable, only count the number of electrons at the target
    else{
        //Assigning target cut for electron counting
        TCut target_cut;
        if (target=="d2"){target_cut=vz_d2;}
        else if (target=="solid"){target_cut=vz_solid;}

        //Counting of the number of electron in each target by making an hist and counting entries
        int n_e = e_tuple->Draw("v_z>>h_e", Beta_cut&&P_cut&&DIS_cut&&target_cut, "goff");
        std::cout << "number of elec in " <<target<<" = " << n_e << std::endl;

        //Convert number of electron into flat histogram.
        elec_hist = new TH1F("elec_"+target+"_hist", "", nbins, xmin, xmax);
        for (int i = 1; i <= nbins; i++) {
            elec_hist->SetBinContent(i, n_e);
        }
    }
    return elec_hist;
}

void m_ratio(TString var, int nbins, float xmin, float xmax, TString hadron,
                    TNtuple* h_tuple, TNtuple* e_tuple, TString output_location){
    //Print message of wich vatiable is being calculated
    cout<<"Calculating Multiplicity Ratio of "<<var<<" variable"<<endl;

    //create canvas for the plots
    TCanvas *canvas= new TCanvas("canvas","canvas",1000,600);
    canvas->cd();

    //var histogram for electrons
    TH1F* elec_d2_hist = make_var_ehisto(var, nbins, xmin, xmax, e_tuple, "d2");
    TH1F* elec_solid_hist = make_var_ehisto(var, nbins, xmin, xmax, e_tuple, "solid");

    //var histogram for hadron
    TH1F *h_d2_hist = make_var_histo(var, nbins, xmin, xmax, h_tuple, "d2");
    TH1F *h_solid_hist = make_var_histo(var, nbins, xmin, xmax, h_tuple, "solid");

    //Error propagation
    h_d2_hist->Sumw2();
    h_solid_hist->Sumw2();
    elec_d2_hist->Sumw2();
    elec_solid_hist->Sumw2();

    //Multiplicity and multiplicity ratio histograms
    TH1F *m_d2 = new TH1F("Multiplicity D2 " + var, "Multiplicity D2" + var, nbins, xmin, xmax);
    TH1F *m_solid = new TH1F("Multiplicity Solid" + var, "Multiplicity solid" + var,
                                nbins, xmin, xmax);
    TH1F *mr = new TH1F("Multiplicity ratio " + var, "Multiplicity ratio" + var, nbins, xmin, xmax);

    //Error propagation
    m_d2->Sumw2();
    m_solid->Sumw2();
    mr->Sumw2();

    //Calculation of multiplicity and multiplicity ratio  
    //**For some reason by using: mr->Divide(m_solid, m_d2, n_e_d2, n_e_solid)
    //**gives an incorrect calculation. TODO Check why at some point.
    m_d2->Divide(h_d2_hist, elec_d2_hist);
    m_solid->Divide(h_solid_hist, elec_solid_hist);
    mr->Divide(m_solid, m_d2);
    mr->GetXaxis()->SetTitle(var);
    mr->GetYaxis()->SetTitle("#frac{N_{A}#pi^{+}}{N_{D2}#pi^{+}}#frac{N_{D2}e^{-}}{N_{A}e^{-}}");
    mr->SetTitle("Multiplicity Ratio");
    mr->SetMarkerStyle(21);
    mr->Draw("COLZ");
    canvas->SaveAs(output_location+"mr_"+var+"_"+hadron+".pdf");

    //debug
    //h_d2_hist->Draw();
    //canvas->SaveAs(output_location+"mr_"+var+"_"+hadron+"_hd2.pdf");
    //h_solid_hist->Draw();
    //canvas->SaveAs(output_location+"mr_"+var+"_"+hadron+"_hsolid.pdf");
    //elec_d2_hist->Draw();
    //canvas->SaveAs(output_location+"mr_"+var+"_"+hadron+"_ed2.pdf");
    //elec_solid_hist->Draw();
    //canvas->SaveAs(output_location+"mr_"+var+"_"+hadron+"_esolid.pdf");

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

    m_ratio("z_h", 10, 0., 1., hadron, hadron_tuple, elec_tuple, output_location);
    m_ratio("nu", 10, 0., 11., hadron, hadron_tuple, elec_tuple, output_location);
    //m_ratio("pt2", 10, 0., 5., hadron, hadron_tuple, elec_tuple, output_location);

//TODO make code to save plots into root file
//TODO make code to plots all MR in same canvas

//h->Write(0, TObject::kOverwrite); to ocerwrite object in tfile in case of need it at some point
}