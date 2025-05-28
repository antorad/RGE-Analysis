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

    //Get histograms for data, acc and thr
    TFile *input_data 	= new TFile("output/data/"+Target+"/mr_clas12.root","READ");
    TFile *input_acc 	= new TFile("output/simul/"+Target+"/mr_clas12.root","READ");
    TFile *input_thrown = new TFile("output/simul/"+Target+"/thrown/mr_clas12.root","READ");
	cout<<"1"<<endl;
    TH1F* h_solid_data	  = (TH1F*)input_data->Get(hadron+"_z_h"+"_solid");
    TH1F* h_d2_data 	  = (TH1F*)input_data->Get(hadron+"_z_h"+"_d2");
    TH1F* elec_solid_data = (TH1F*)input_data->Get(hadron+"_z_h"+"_elec_solid");
    TH1F* elec_d2_data 	  = (TH1F*)input_data->Get(hadron+"_z_h"+"_elec_d2");
	cout<<"2"<<endl;
    TH1F* h_solid_acc	 = (TH1F*)input_acc->Get(hadron+"_z_h"+"_solid");
    TH1F* h_d2_acc 	  	 = (TH1F*)input_acc->Get(hadron+"_z_h"+"_d2");
    TH1F* elec_solid_acc = (TH1F*)input_acc->Get(hadron+"_z_h"+"_elec_solid");
    TH1F* elec_d2_acc 	 = (TH1F*)input_acc->Get(hadron+"_z_h"+"_elec_d2");
	cout<<"3"<<endl;
    TH1F* h_solid_thrown	= (TH1F*)input_thrown->Get(hadron+"_z_h"+"_solid");
    TH1F* h_d2_thrown 	  	= (TH1F*)input_thrown->Get(hadron+"_z_h"+"_d2");
    TH1F* elec_solid_thrown = (TH1F*)input_thrown->Get(hadron+"_z_h"+"_elec_solid");
    TH1F* elec_d2_thrown 	= (TH1F*)input_thrown->Get(hadron+"_z_h"+"_elec_d2");
	cout<<"4"<<endl;
    //Acceptance correction calculation
    TH1F *h_mr, *e_mr, *mr;
    TH1F *h_ac_sol, *h_ac_liq, *h_sol_corr, *h_d2_corr;
    TH1F *elec_ac_sol, *elec_ac_liq, *elec_sol_corr, *elec_d2_corr;
	cout<<"5"<<endl;
    h_ac_sol=(TH1F*)h_solid_thrown->Clone();
    h_ac_sol->Divide(h_solid_acc);
    h_ac_liq=(TH1F*)h_d2_thrown->Clone();
    h_ac_liq->Divide(h_d2_acc);
    h_sol_corr=(TH1F*)h_solid_data->Clone();
    h_sol_corr->Multiply(h_ac_sol);
    h_d2_corr=(TH1F*)h_d2_data->Clone();
    h_d2_corr->Multiply(h_ac_liq);
	cout<<"6"<<endl;
    elec_ac_sol=(TH1F*)elec_solid_thrown->Clone();
    elec_ac_sol->Divide(elec_solid_acc);
    elec_ac_liq=(TH1F*)elec_d2_thrown->Clone();
    elec_ac_liq->Divide(elec_d2_acc);
    elec_sol_corr=(TH1F*)elec_solid_data->Clone();
    elec_sol_corr->Multiply(elec_ac_sol);
    elec_d2_corr=(TH1F*)elec_d2_data->Clone();
    elec_d2_corr->Multiply(elec_ac_liq);
	cout<<"7"<<endl;
	h_mr=(TH1F*)h_sol_corr->Clone();
    h_mr->Divide(h_d2_corr);
    e_mr=(TH1F*)elec_sol_corr->Clone();
    e_mr->Divide(elec_d2_corr);
    mr=(TH1F*)h_mr->Clone();
    mr->Divide(e_mr);
    mr->Draw();
    cout<<"8"<<endl;
    TFile *output = new TFile("output/"+Target+"/mr_1d_zh_"+hadron+".root","RECREATE");
    output->cd();
    h_ac_sol->Write("h_ac_sol");
    h_ac_liq->Write("h_ac_liq");
    h_sol_corr->Write("h_sol_corr");
    h_d2_corr->Write("h_d2_corr");
    elec_ac_sol->Write("elec_ac_sol");
    elec_ac_liq->Write("elec_ac_liq");
    elec_sol_corr->Write("elec_sol_corr");
    elec_d2_corr->Write("elec_d2_corr");
    h_mr->Write("h_mr");
    e_mr->Write("e_mr");
    mr->Write("mr");
    output->Close();
}