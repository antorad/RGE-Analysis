#include "TROOT.h"
#include "TFile.h"
#include "TChain.h"
#include "TNtuple.h"
#include "TMath.h"
#include "TList.h"
#include "TString.h"
#include "TCut.h"
#include <iostream>
#include <string>
#include <cmath>

using namespace std;

void draw_plot(TNtuple* tuple, TCut cut, char const* var, int nbins, int xmin, int xmax,
				 TString title, TString output, TCanvas* canvas, TString location){
	canvas->cd();
	TString histo_to_draw;
	histo_to_draw.Form("%s>>histo(%i,%i,%i)", var, nbins, xmin, xmax);
	tuple->Draw(histo_to_draw,cut,"COLZ");
	TH1F *histo = (TH1F*)gDirectory->GetList()->FindObject("histo");
	histo->GetXaxis()->SetTitle(title);
	histo->Draw("COLZ");
	canvas->SaveAs(location+output+".pdf");
}

void draw_plot_2D(TNtuple* tuple, TCut cut, char const* var, int nbins, int xmin, int xmax,
					 TString title, TString output, TCanvas* canvas, TString location){
	canvas->cd();
	TString histo_to_draw;
	histo_to_draw.Form("%s>>histo(%i,%i,%i)", var, nbins, xmin, xmax);
	tuple->Draw(histo_to_draw,cut,"COLZ");
	TH1F *histo = (TH1F*)gDirectory->GetList()->FindObject("histo");
	histo->GetXaxis()->SetTitle(title);
	histo->Draw("COLZ");
	canvas->SaveAs(location+output+".pdf");
}

void simple_plots(int run_N=000000){

//TO DO: modify to give multiple files to add using TChain probably, using an input like 20150-20165 or a text files with a run list.
	char buffer [10];
	sprintf(buffer,"%0*d", 6, run_N);
	TString run_N_str=TString(buffer);
	cout<<"run number: "<<run_N_str<<endl;
	TString output_location = "output/"+run_N_str+"/";
	cout<<"output location: "<<output_location<<endl;

	TFile *input = new TFile("data/ntuples_dc_"+run_N_str+".root","READ");
	TNtuple* input_tuple = (TNtuple*)input->Get("data");

	TFile *output = new TFile(output_location+"out_clas12.root","RECREATE");

	Float_t pid, Q2, nu, v_z, z_h, p, E_total, E_ECIN, E_ECOU, event_num, v_z_elec, phi, y_bjorken, W2, charge, beta; 
	Float_t rad2deg = 57.2958;

//------Read branches with variables used and needed for cuts------
	input_tuple->SetBranchAddress("pid",&pid);
	input_tuple->SetBranchAddress("Q2",&Q2);
	input_tuple->SetBranchAddress("nu",&nu);
	input_tuple->SetBranchAddress("v_z",&v_z);
	input_tuple->SetBranchAddress("z_h",&z_h);
	input_tuple->SetBranchAddress("p",&p);
	input_tuple->SetBranchAddress("E_total",&E_total);
	input_tuple->SetBranchAddress("E_ECIN",&E_ECIN);
	input_tuple->SetBranchAddress("E_ECOU",&E_ECOU);
	input_tuple->SetBranchAddress("event_num",&event_num);
	input_tuple->SetBranchAddress("y_bjorken",&y_bjorken);
	input_tuple->SetBranchAddress("W2",&W2);
	input_tuple->SetBranchAddress("charge",&charge);
	input_tuple->SetBranchAddress("beta",&beta);
	input_tuple->SetBranchAddress("phi",&phi);

//------output ntuple------
	Float_t pion_vars[14];
	Float_t positive_vars[14];
	Float_t elec_vars[12];
	const char* pion_varslist = "pid:Q2:nu:v_z:p:E_total:E_ECIN:E_ECOU:z_h:v_z_elec:y_bjorken:W2:beta:phi";
	const char* positive_varslist = "pid:Q2:nu:v_z:p:E_total:E_ECIN:E_ECOU:z_h:v_z_elec:y_bjorken:W2:beta:phi";
	const char* elec_varslist = "pid:Q2:nu:v_z:p:E_total:E_ECIN:E_ECOU:y_bjorken:W2:beta:phi";
	TNtuple *pion_tuple = new TNtuple("pion_ntuple","pions",pion_varslist);
	TNtuple *positive_tuple = new TNtuple("positive_ntuple","positives",positive_varslist);
	TNtuple *elec_tuple = new TNtuple("elec_tuple","electrons",elec_varslist);


v_z_elec = -99;
// Selection of particles to plot
	Long64_t n_entries = input_tuple->GetEntries();
	for (Long64_t i=0;i<n_entries;i++) {
		input_tuple->GetEntry(i);

		//This part assumes that all hadrons after an electron coes from taht electron to save its v_z
		// Check if the particle fullfills being the sactteried electron.
		if  (pid==11) {
			elec_vars[0] = pid;
			elec_vars[1] = Q2;
			elec_vars[2] = nu;
			elec_vars[3] = v_z;
			elec_vars[4] = p;
			elec_vars[5] = E_total;
			elec_vars[6] = E_ECIN;
			elec_vars[7] = E_ECOU;
			elec_vars[8] = y_bjorken;
			elec_vars[9] = W2;
			elec_vars[10] = beta;
			elec_vars[11] = phi*rad2deg;
			elec_tuple->Fill(elec_vars);
			v_z_elec = v_z;
		}

//TO DO: maybe check to not process teh same particle in the 3 ifs

		// Check if the particle is positive.
		if  (charge>0) {
			positive_vars[0] = pid;
			positive_vars[1] = Q2;
			positive_vars[2] = nu;
			positive_vars[3] = v_z;
			positive_vars[4] = p;
			positive_vars[5] = E_total;
			positive_vars[6] = E_ECIN;
			positive_vars[7] = E_ECOU;
			positive_vars[8] = z_h;
			positive_vars[9] = v_z_elec;
			positive_vars[10] = y_bjorken;
			positive_vars[11] = W2;
			positive_vars[12] = beta;
			positive_vars[13] = phi*rad2deg;
			positive_tuple->Fill(positive_vars);
		}

		// Check if the particle is a positive pion
		if  (pid==211) {
			pion_vars[0] = pid;
			pion_vars[1] = Q2;
			pion_vars[2] = nu;
			pion_vars[3] = v_z;
			pion_vars[4] = p;
			pion_vars[5] = E_total;
			pion_vars[6] = E_ECIN;
			pion_vars[7] = E_ECOU;
			pion_vars[8] = z_h;
			pion_vars[9] = v_z_elec;
			pion_vars[10] = y_bjorken;
			pion_vars[11] = W2;
			pion_vars[12] = beta;
			pion_vars[13] = phi*rad2deg;
			pion_tuple->Fill(pion_vars);
		}
	}

	//------root file output writing------
	output->cd();
	pion_tuple->Write();
	positive_tuple->Write();
	elec_tuple->Write();
	//output->Close();

	//------ plots------;
	TCanvas *c= new TCanvas("c","c",1000,600);
	//c->cd();

//TO DO: make a list of variables to plot and make a function to plot it instead of copy paste.

//TO DO add plots by sector

	//cuts for the ṕlots
	TCut Beta_cut="(beta>0)&&(beta<1.2)";
	TCut P_cut="(p>0)&&(p<12)";
	TCut Zv_d2="(v_z>-8.64)&&(v_z<-2.78)";
	TCut Zv_solid="(v_z>-2.78)&&(v_z<1.04)";
	TCut DIS_cut="(Q2>1)&&(sqrt(W2)>2)&&(y_bjorken<0.85)";

	//ELECTRONS
	//z vertex
	//elec_tuple->Draw("v_z>>e_v_z(100,-15,6)",P_cut,"COLZ");
	//TH1F *e_v_z = (TH1F*)gDirectory->GetList()->FindObject("e_v_z");
	//e_v_z->GetXaxis()->SetTitle("V_z");
	//e_v_z->Draw("COLZ");
	//c->SaveAs(output_location+"e_v_z.pdf");
	TCut total_cut = P_cut;
	draw_plot(elec_tuple, total_cut, "v_z",100,-15,6, "V_z", "e_v_z", c, output_location);

	elec_tuple->Draw("phi>>e_phi(360,-180,180)",P_cut,"COLZ");
	TH1F *e_phi = (TH1F*)gDirectory->GetList()->FindObject("e_phi");
	e_phi->GetXaxis()->SetTitle("Phi");
	e_phi->Draw("COLZ");
	c->SaveAs(output_location+"e_phi.pdf");

	//PIONS
	pion_tuple->Draw("v_z>>pi_v_z(100,-15,6)",P_cut,"COLZ");
	TH1F *pi_v_z = (TH1F*)gDirectory->GetList()->FindObject("pi_v_z");
	pi_v_z->GetXaxis()->SetTitle("V_z");
	pi_v_z->Draw("COLZ");
	c->SaveAs(output_location+"pi_v_z.pdf");

	//pions
	pion_tuple->Draw("z_h>>pi_zh_d2(100,0,1)",P_cut&&Zv_d2,"COLZ");
	TH1F *pi_zh_d2 = (TH1F*)gDirectory->GetList()->FindObject("pi_zh_d2");
	pi_zh_d2->GetXaxis()->SetTitle("Z_h");
	pi_zh_d2->Draw("COLZ");
	c->SaveAs(output_location+"pi_zh_d2.pdf");

	//pions
	pion_tuple->Draw("z_h>>pi_zh_pb(100,0,1)",P_cut&&Zv_solid&&DIS_cut,"COLZ");
	TH1F *pi_zh_pb = (TH1F*)gDirectory->GetList()->FindObject("pi_zh_pb");
	pi_zh_pb->GetXaxis()->SetTitle("Z_h");
	pi_zh_pb->Draw("COLZ");
	c->SaveAs(output_location+"pi_zh_pb.pdf");

	pion_tuple->Draw("v_z_elec>>v_z_elec(100,-15,6)",P_cut&&DIS_cut,"COLZ");
	TH1F *pi_v_z_elec = (TH1F*)gDirectory->GetList()->FindObject("v_z_elec");
	pi_v_z_elec->GetXaxis()->SetTitle("V_z");
	pi_v_z_elec->Draw("COLZ");
	c->SaveAs(output_location+"v_z_elec.pdf");

	//multiplicity ratio
	int n_e_d2 = elec_tuple->Draw("v_z>>hist",P_cut&&Zv_d2&&DIS_cut,"goff");
	int n_e_pb = elec_tuple->Draw("v_z>>hist",P_cut&&Zv_solid&&DIS_cut,"goff");
	cout<<"elec d2 = "<<n_e_d2<<endl;
	cout<<"elec pb = "<<n_e_pb<<endl;

	pion_tuple->Draw("z_h>>z_h_d2(10,0,1)",P_cut&&Zv_d2&&DIS_cut,"COLZ");
	TH1F *z_h_d2 = (TH1F*)gDirectory->GetList()->FindObject("z_h_d2");
	pion_tuple->Draw("z_h>>z_h_pb(10,0,1)",P_cut&&Zv_solid&&DIS_cut,"COLZ");
	TH1F *z_h_pb = (TH1F*)gDirectory->GetList()->FindObject("z_h_pb");

	z_h_d2->Sumw2();
	z_h_pb->Sumw2();

	TH1F* mr = new TH1F("mr", "mr", 10,0,1);
	mr->Divide(z_h_pb, z_h_d2, n_e_d2, n_e_pb);
	mr->Draw("COLZ");
	mr->SetMarkerStyle(21);
	c->SaveAs(output_location+"mr.pdf");

	//positive particles, p vs beta
	positive_tuple->Draw("beta:p>>p_beta(500,0,12,500,0,1.2)",Beta_cut&&P_cut,"COLZ");
	TH2F *p_beta = (TH2F*)gDirectory->GetList()->FindObject("p_beta");
	p_beta->GetXaxis()->SetTitle("p");
	p_beta->GetYaxis()->SetTitle("beta");
	p_beta->Draw("COLZ");
	c->SaveAs(output_location+"p_beta.pdf");
}