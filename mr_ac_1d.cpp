void calculate_mr_ac(TString hadron, TString var, TFile* in_data, TFile* in_acc, TFile* in_thr, TFile* output){

    //Get Z_h distributions and electron numbers from data
    TH1F* h_solid_data    = (TH1F*)in_data->Get(hadron+"_"+var+"_solid");
    TH1F* h_d2_data       = (TH1F*)in_data->Get(hadron+"_"+var+"_d2");
    TH1F* elec_solid_data = (TH1F*)in_data->Get(hadron+"_"+var+"_elec_solid");
    TH1F* elec_d2_data    = (TH1F*)in_data->Get(hadron+"_"+var+"_elec_d2");
    //Get Z_h distributions and electron numbers from acc
    TH1F* h_solid_acc    = (TH1F*)in_acc->Get(hadron+"_"+var+"_solid");
    TH1F* h_d2_acc       = (TH1F*)in_acc->Get(hadron+"_"+var+"_d2");
    TH1F* elec_solid_acc = (TH1F*)in_acc->Get(hadron+"_"+var+"_elec_solid");
    TH1F* elec_d2_acc    = (TH1F*)in_acc->Get(hadron+"_"+var+"_elec_d2");
    //Get Z_h distributions and electron numbers from thrown
    TH1F* h_solid_thrown    = (TH1F*)in_thr->Get(hadron+"_"+var+"_solid");
    TH1F* h_d2_thrown       = (TH1F*)in_thr->Get(hadron+"_"+var+"_d2");
    TH1F* elec_solid_thrown = (TH1F*)in_thr->Get(hadron+"_"+var+"_elec_solid");
    TH1F* elec_d2_thrown    = (TH1F*)in_thr->Get(hadron+"_"+var+"_elec_d2");

    //Acceptance correction calculation

    //Define TH1 needed later
    TH1F *h_mr, *e_mr, *mr;
    TH1F *h_ac_sol, *h_ac_liq, *h_sol_corr, *h_d2_corr;
    TH1F *elec_ac_sol, *elec_ac_liq, *elec_sol_corr, *elec_d2_corr;

    //Aceptace correction for solid zh
    h_ac_sol=(TH1F*)h_solid_thrown->Clone();
    h_ac_sol->Divide(h_solid_acc);
    //Aceptace correction for d2 zh
    h_ac_liq=(TH1F*)h_d2_thrown->Clone();
    h_ac_liq->Divide(h_d2_acc);
    //Corrected zh distribution for solid
    h_sol_corr=(TH1F*)h_solid_data->Clone();
    h_sol_corr->Multiply(h_ac_sol);
    //Corrected zh distribution for liquid
    h_d2_corr=(TH1F*)h_d2_data->Clone();
    h_d2_corr->Multiply(h_ac_liq);

    //Aceptace correction for number of electrons in solid
    elec_ac_sol=(TH1F*)elec_solid_thrown->Clone();
    elec_ac_sol->Divide(elec_solid_acc);
    //Aceptace correction for number of electrons in d2
    elec_ac_liq=(TH1F*)elec_d2_thrown->Clone();
    elec_ac_liq->Divide(elec_d2_acc);
    //Corrected for number of electrons in solid
    elec_sol_corr=(TH1F*)elec_solid_data->Clone();
    elec_sol_corr->Multiply(elec_ac_sol);
    //Corrected for number of electrons in d2
    elec_d2_corr=(TH1F*)elec_d2_data->Clone();
    elec_d2_corr->Multiply(elec_ac_liq);

    //Multiplicity ratios for zh
    h_mr=(TH1F*)h_sol_corr->Clone();
    h_mr->Divide(h_d2_corr);
    //Multiplicity ratios for electrons
    e_mr=(TH1F*)elec_sol_corr->Clone();
    e_mr->Divide(elec_d2_corr);
    //Multiplicity ratio normalized by electrons
    mr=(TH1F*)h_mr->Clone();
    mr->Divide(e_mr);
    mr->Draw();

    //Write output
    h_ac_sol->Write(var+"_h_ac_sol");
    h_ac_liq->Write(var+"_h_ac_liq");
    h_sol_corr->Write(var+"_h_sol_corr");
    h_d2_corr->Write(var+"_h_d2_corr");
    elec_ac_sol->Write(var+"_elec_ac_sol");
    elec_ac_liq->Write(var+"_elec_ac_liq");
    elec_sol_corr->Write(var+"_elec_sol_corr");
    elec_d2_corr->Write(var+"_elec_d2_corr");
    h_mr->Write(var+"_h_mr");
    e_mr->Write(var+"_e_mr");
    mr->Write(var+"_mr");
}

void mr_ac_1d(TString Target="C", int Hadron_pid=211){
	ROOT::EnableImplicitMT();

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

    //Get mr files from data, acc and thrown
    TFile *input_data 	= new TFile("output/data/"+Target+"/mr_clas12.root","READ");
    TFile *input_acc 	= new TFile("output/simul/"+Target+"/mr_clas12.root","READ");
    TFile *input_thrown = new TFile("output/simul/"+Target+"/thrown/mr_clas12.root","READ");

    //Output
    TFile *output = new TFile("output/"+Target+"/mr_1d_"+hadron+".root","RECREATE");

    //Run MR with acceptance correction calculation
    output->cd();
    calculate_mr_ac(hadron, "z_h", input_data, input_acc, input_thrown, output);
    calculate_mr_ac(hadron, "nu", input_data, input_acc, input_thrown, output);
    calculate_mr_ac(hadron, "p_T2", input_data, input_acc, input_thrown, output);
    output->Close();
}