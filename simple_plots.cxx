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

//plot 1D plots
void draw_plot(TNtuple* tuple, TCut cut, char const* var, int nbins, float xmin, float xmax,
				TString xtitle, TString ytitle, TString output, TString location, TFile* output_file){
	TCanvas *canvas = new TCanvas("canvas","canvas",1000,600);
	canvas->cd();
	TString histo_to_draw;
	histo_to_draw.Form("%s>>histo(%i,%f,%f)", var, nbins, xmin, xmax);
	tuple->Draw(histo_to_draw,cut,"COLZ");
	TH1F *histo = (TH1F*)gDirectory->GetList()->FindObject("histo");
	histo->GetXaxis()->SetTitle(xtitle);
	histo->GetYaxis()->SetTitle(ytitle);
	histo->SetTitle(xtitle);
	histo->Draw("COLZ");
	histo->Write(output);
	canvas->SaveAs(location+output+".pdf");
	delete histo;
	delete canvas;
}

//plot 1D plots by sector
void draw_sector_plot(TNtuple* tuple, TCut cut, char const* var, int nbins, float xmin, float xmax,
					TString xtitle, TString ytitle, TString output, TString location, TFile* output_file){
	TCanvas *canvas= new TCanvas("canvas","canvas",1000,600);
	canvas->Divide(3,2);
	TH1F *histo[6];
	for (int i = 1; i < 7; ++i){
		canvas->cd(i);
		TCut sector_cut = (TString::Format("sector==%i", i)).Data();
		string title = (TString::Format("sec%i", i)).Data();
		TString histo_to_draw;
		histo_to_draw.Form("%s>>histo%i(%i,%f,%f)", var, i, nbins, xmin, xmax);
		tuple->Draw(histo_to_draw,cut&&sector_cut,"COLZ");
		histo[i-1] = (TH1F*)gDirectory->GetList()->FindObject(Form("histo%i",i));
		histo[i-1]->GetXaxis()->SetTitle(xtitle);
		histo[i-1]->GetYaxis()->SetTitle(ytitle);
		histo[i-1]->SetTitle(xtitle+Form(" Sector %i",i-1));
		histo[i-1]->Draw("COLZ");
		histo[i-1]->Write(output+Form("_%i",i-1));
	}
	canvas->SaveAs(location+output+".pdf");
	delete canvas;
}

//plot 2D plots
void draw_plot_2D(TNtuple* tuple, TCut cut, char const* var, int xnbins, float xmin, float xmax,
					TString xtitle, int ynbins, float ymin, float ymax, TString ytitle, 
					TString output, TString location, TFile* output_file){
	TCanvas *canvas= new TCanvas("canvas","canvas",1000,600);
	canvas->cd();
	TString histo_to_draw;
	histo_to_draw.Form("%s>>histo(%i,%f,%f,%i,%f,%f)", var, xnbins, xmin, xmax, ynbins, ymin, ymax);
	tuple->Draw(histo_to_draw, cut,"COLZ");
	TH2F *histo = (TH2F*)gDirectory->GetList()->FindObject("histo");
	histo->GetXaxis()->SetTitle(xtitle);
	histo->GetYaxis()->SetTitle(ytitle);
	histo->SetTitle(xtitle + " vs " + ytitle);
	histo->Draw("COLZ");
	histo->Write(output);
	canvas->SaveAs(location+output+".pdf");
	delete histo;
	delete canvas;
}

//process the input file and crate all the plots
void processChain(TChain* input_tuple, TString output_location) {
	//create output file and output directory in case it doenst exist
	std::string command = std::string("mkdir -p ") + output_location.Data();
	gSystem->Exec(command.c_str());
	TFile *output = new TFile(output_location+"out_clas12.root","RECREATE");

	Float_t pid, Q2, nu, v_z, z_h, p, p_T2, p_L2, E_total, E_ECIN, E_ECOU, event_num, v_z_elec, phi, x_bjorken, y_bjorken, W2, charge, beta, sector, phi_PQ; 
	Float_t rad2deg = 57.2958;

	//------Read branches with variables needed for cuts and plots------
	input_tuple->SetBranchAddress("pid",&pid);
	input_tuple->SetBranchAddress("Q2",&Q2);
	input_tuple->SetBranchAddress("nu",&nu);
	input_tuple->SetBranchAddress("v_z",&v_z);
	input_tuple->SetBranchAddress("z_h",&z_h);
	input_tuple->SetBranchAddress("p",&p);
	input_tuple->SetBranchAddress("p_T2",&p_T2);
	input_tuple->SetBranchAddress("E_total",&E_total);
	input_tuple->SetBranchAddress("E_ECIN",&E_ECIN);
	input_tuple->SetBranchAddress("E_ECOU",&E_ECOU);
	input_tuple->SetBranchAddress("event_num",&event_num);
	input_tuple->SetBranchAddress("y_bjorken",&y_bjorken);
	input_tuple->SetBranchAddress("W2",&W2);
	input_tuple->SetBranchAddress("charge",&charge);
	input_tuple->SetBranchAddress("beta",&beta);
	input_tuple->SetBranchAddress("phi",&phi);
	input_tuple->SetBranchAddress("sector",&sector);
	input_tuple->SetBranchAddress("phi_PQ",&phi_PQ);

	//------output ntuples------
	Float_t hadron_vars[19];
	Float_t elec_vars[14];
	const char* hadron_varslist = "pid:Q2:nu:v_z:p:p_T2:p_L2:E_total:E_ECIN:E_ECOU:z_h:v_z_elec:x_bjorken:y_bjorken:W2:beta:phi:sector:phi_PQ";
	const char* elec_varslist = "pid:Q2:nu:v_z:p:E_total:E_ECIN:E_ECOU:x_bjorken:y_bjorken:W2:beta:phi:sector";
	TNtuple *pion_tuple = new TNtuple("pion_ntuple","pions",hadron_varslist);
	TNtuple *positive_tuple = new TNtuple("positive_ntuple","positives",hadron_varslist);
	TNtuple *pion_minus_tuple = new TNtuple("pion_minus_ntuple","positives",hadron_varslist);
	TNtuple *proton_tuple = new TNtuple("proton_ntuple","positives",hadron_varslist);
	TNtuple *elec_tuple = new TNtuple("elec_tuple","electrons",elec_varslist);


	v_z_elec = -99;
	//Selection of particles to plot
	Long64_t n_entries = input_tuple->GetEntries();
	for (Long64_t i=0;i<n_entries;i++) {
		input_tuple->GetEntry(i);

		//This part assumes that all hadrons after an electron coes from taht electron to save its v_z
		// Check if the particle fullfills being the scattered electron.
		if  (pid==11) {
			elec_vars[0] = pid;
			elec_vars[1] = Q2;
			elec_vars[2] = nu;
			elec_vars[3] = v_z;
			elec_vars[4] = p;
			elec_vars[5] = E_total;
			elec_vars[6] = E_ECIN;
			elec_vars[7] = E_ECOU;
			elec_vars[8] = x_bjorken;
			elec_vars[9] = y_bjorken;
			elec_vars[10] = W2;
			elec_vars[11] = beta;
			elec_vars[12] = phi*rad2deg;
			elec_vars[13] = sector;
			elec_tuple->Fill(elec_vars);
			v_z_elec = v_z;
		}

		// Check if the particle is not an electron.
		else{
			hadron_vars[0] = pid;
			hadron_vars[1] = Q2;
			hadron_vars[2] = nu;
			hadron_vars[3] = v_z;
			hadron_vars[4] = p;
			hadron_vars[5] = p_T2;
			hadron_vars[6] = p_L2;
			hadron_vars[7] = E_total;
			hadron_vars[8] = E_ECIN;
			hadron_vars[9] = E_ECOU;
			hadron_vars[10] = z_h;
			hadron_vars[11] = v_z_elec;
			hadron_vars[12] = x_bjorken;
			hadron_vars[13] = y_bjorken;
			hadron_vars[14] = W2;
			hadron_vars[15] = beta;
			hadron_vars[16] = phi*rad2deg;
			hadron_vars[17] = sector;
			hadron_vars[18] = phi_PQ; 
			if (charge>0){positive_tuple->Fill(hadron_vars);}
			if (pid==211){pion_tuple->Fill(hadron_vars);}
			else if (pid==-211){pion_minus_tuple->Fill(hadron_vars);}
			else if (pid==2212){proton_tuple->Fill(hadron_vars);}
		}
	}

	//------root file output writing------
	output->cd();
	pion_tuple->Write();
	pion_minus_tuple->Write();
	proton_tuple->Write();
	positive_tuple->Write();
	elec_tuple->Write();

	//------ PLOTS------
	//cuts for the ṕlots
	TCut Beta_cut="(beta>0)&&(beta<1.2)";
	TCut P_cut="(p>0)&&(p<12)";
	TCut DIS_cut="(Q2>1)&&(sqrt(W2)>2)&&(y_bjorken<0.85)";

	//----ELECTRONS----
	//z vertex (total)
	draw_plot(elec_tuple, P_cut, "v_z",100,-15,6, "V_{z} [cm]", "dN/dV_{z}" , "e_v_z",
				output_location, output);

	//z vertex by sector
	draw_sector_plot(elec_tuple, P_cut, "v_z",100,-15,6, "V_{z} [cm]", "dN/dV_{z}",
						"sector_e_vz", output_location, output);
	//W2
	draw_plot(elec_tuple, P_cut, "W2",100,0,20, "W^{2}", "dN/dW^{2}" , "e_w2", output_location, output);

	//Q2
	draw_plot(elec_tuple, P_cut, "Q2",100,0,12, "Q^{2}", "dN/dQ^{2}" , "e_q2", output_location, output);

	//Nu
	draw_plot(elec_tuple, P_cut, "nu",100,0,12, "#nu", "dN/d#nu" , "e_nu", output_location, output);

	//phi distribution
	draw_plot(elec_tuple, P_cut, "phi",360,-180,180, "#phi [deg]", "dN/d#phi", "e_phi",
				output_location, output);

	//ein vs eout
	draw_plot_2D(elec_tuple, Beta_cut&&P_cut&&DIS_cut, "E_total/p:p", 100,0,12,"P [GeV]",
					100, 0, 0.5, "E_{tot}/P", "etot_p", output_location, output);

	//----PIONS----
	//z vertex
	draw_plot(pion_tuple, P_cut, "v_z",100,-15,6, "V_{z} [cm]", "dN/dV_{z}", "pi_v_z",
				output_location, output);

	//z_h
	draw_plot(pion_tuple, P_cut&&DIS_cut, "z_h",100,0,1, "Z_{h}", "dN/dZ_{h}", "pi_zh",
				output_location, output);

	//phi distribution
	draw_plot(pion_tuple, P_cut, "phi",360,-180,180, "#phi [deg]", "dN/d#phi", "pi_phi",
				output_location, output);

	//----POSITIVE PARTICLES----
	//p vs beta
	draw_plot_2D(positive_tuple, Beta_cut&&P_cut, "beta:p", 500,0,12,"P [GeV]", 500, 0, 1.2, "#beta",
					"p_beta", output_location, output);

	//delete all objects;
	//output->Close();
	delete pion_tuple;
	delete positive_tuple;
	delete elec_tuple;
	delete output;
}

//Main function that recieves a txt with a list of run number asn the name of the output file
void simple_plots(const char* inputFileName, TString output_name){
	ROOT::EnableImplicitMT();
	// Open the input text file
    std::ifstream inputFile(inputFileName);

    // Create a TChain to combine input TNuples
    TChain* input_tuple = new TChain("data");

    // Read each line from the text file and add the corresponding ROOT file to the TChain
    std::string rootFileName;
    while (std::getline(inputFile, rootFileName)) {
        input_tuple->Add("data/ntuples_dc_0"+TString(rootFileName)+".root");
    }

    // Close the input file
    inputFile.close();

    //process the Tchain to make plots
    TString output_location = "output/"+output_name+"/";
    processChain(input_tuple, output_location);

    //delete all objects
    delete input_tuple;
}

//Main function that recieves a run number as the input
void simple_plots(int run_N=000000){
	ROOT::EnableImplicitMT();
	//Transform input run number to Tstring with correct number of digits
	char buffer [10];
	sprintf(buffer,"%0*d", 6, run_N);
	TString run_N_str=TString(buffer);
	TString output_location = "output/"+run_N_str+"/";
	cout<<output_location<<endl;

	//extract input TNuple from input file
	TFile *input = new TFile("data/ntuples_dc_"+run_N_str+".root","READ");
	cout<<"File found "<<run_N_str<<endl;
	TChain* input_tuple = (TChain*)input->Get("data");

	//process the TNtuple to make plots
	cout<<"tuple done"<<endl;
	processChain(input_tuple, output_location);

	//delete all objects
	delete input_tuple;
	delete input;
}