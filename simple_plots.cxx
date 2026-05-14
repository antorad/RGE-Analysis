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
#include "include.h"

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
	cout<<"Creating output directories "<<endl;
	std::string command = std::string("mkdir -p ") + output_location.Data();
	gSystem->Exec(command.c_str());
	TFile *output = new TFile(output_location+"out_clas12.root","RECREATE");

	Float_t pid, Q2, nu, v_z, z_h, p, p_T2, p_L2, E_total, E_ECIN, E_PCAL, E_ECOU, event_num, v_z_elec, phi, x_bjorken, y_bjorken, W2, charge, beta, sector, phi_PQ, theta, v_x, v_y; 
	Float_t rad2deg = 57.2958;

	cout<<"Reading input tuple"<<endl;
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
	input_tuple->SetBranchAddress("E_PCAL",&E_PCAL);
	input_tuple->SetBranchAddress("event_num",&event_num);
	input_tuple->SetBranchAddress("y_bjorken",&y_bjorken);
	input_tuple->SetBranchAddress("W2",&W2);
	input_tuple->SetBranchAddress("charge",&charge);
	input_tuple->SetBranchAddress("beta",&beta);
	input_tuple->SetBranchAddress("phi",&phi);
	input_tuple->SetBranchAddress("sector",&sector);
	input_tuple->SetBranchAddress("phi_PQ",&phi_PQ);
	input_tuple->SetBranchAddress("theta",&theta);
	input_tuple->SetBranchAddress("v_x",&v_x);
	input_tuple->SetBranchAddress("v_y",&v_y);
	input_tuple->SetBranchAddress("x_bjorken",&x_bjorken);

	//------output ntuples------
	Float_t hadron_vars[22];
	Float_t elec_vars[17];
	const char* hadron_varslist = "pid:Q2:nu:v_z:p:p_T2:p_L2:E_total:E_ECIN:E_ECOU:z_h:v_z_elec:x_bjorken:y_bjorken:W2:beta:phi:sector:phi_PQ:theta:v_x:v_y";
	const char* elec_varslist = "pid:Q2:nu:v_z:p:E_total:E_ECIN:E_ECOU:x_bjorken:y_bjorken:W2:beta:phi:sector:theta:v_x:v_y";
	TNtuple *pion_tuple = new TNtuple("pion_ntuple","pions",hadron_varslist);
	TNtuple *hadron_tuple = new TNtuple("hadron_ntuple","hadrons",hadron_varslist);
	TNtuple *pion_minus_tuple = new TNtuple("pion_minus_ntuple","positives",hadron_varslist);
	TNtuple *proton_tuple = new TNtuple("proton_ntuple","positives",hadron_varslist);
	TNtuple *elec_tuple = new TNtuple("elec_tuple","electrons",elec_varslist);

	v_z_elec = -99;
	bool valid_electron = false;

	//Sampling fraction parameters
	float sf_up_lim[6][4];
    float sf_lo_lim[6][4];
    for (int i=0; i<6; i++){
        for (int j=0; j<4; j++)
        {
            sf_up_lim[i][j] = mu_sf[i][j]+3.5*sigma_sf[i][j];
            sf_lo_lim[i][j] = mu_sf[i][j]-3.5*sigma_sf[i][j];
        }
    }
    cout<<"Starting processing loop "<<endl;
	//Selection of particles to plot
	Long64_t n_entries = input_tuple->GetEntries();
	for (Long64_t i=0;i<n_entries;i++) { //changed n_entries to 1000000 for testing
		input_tuple->GetEntry(i);
		if (pid == 11){valid_electron = false;}
		//This part assumes that all hadrons after an electron come from that electron to save its v_z
		// Check if the particle fullfills being the scattered electron.
		if  (pid==11 					 		//basic electron cut from CLAS12 event builder
			&& Q2>1 && W2>4 && y_bjorken<0.8 //DIS cuts
			&& p>2 && p<8						//momentum cut
			&& theta*rad2deg>5					//theta cut
			//&& PCAL_V>14 PCAL_W>14				//PCAL fiducial cuts
			////REC::Calorimeter::lv and lu???
			//&& DC_R1_edge>4.5 && DC_R2_edge>3.5 && DC_R3_edge>7.5 //DC fiducial cuts
			////REC::Traj::edge with ::layer to identify region??
			&& ((p<4.5)||(p>4.5&&E_PCAL/p>(-0.22/0.15)*E_ECIN/p+0.22)) // Partial and full sampling fration
			&& ((	sector == 1 && E_total/p < sf_up_lim[0][0]+ sf_up_lim[0][1]*E_total + sf_up_lim[0][2]*pow(E_total,2) + sf_up_lim[0][3]*pow(E_total,3)
								&& E_total/p > sf_lo_lim[0][0]+ sf_lo_lim[0][1]*E_total + sf_lo_lim[0][2]*pow(E_total,2) + sf_lo_lim[0][3]*pow(E_total,3))
				|| (sector == 2 && E_total/p < sf_up_lim[1][0]+ sf_up_lim[1][1]*E_total + sf_up_lim[1][2]*pow(E_total,2) + sf_up_lim[1][3]*pow(E_total,3)
								&& E_total/p > sf_lo_lim[1][0]+ sf_lo_lim[1][1]*E_total + sf_lo_lim[1][2]*pow(E_total,2) + sf_lo_lim[1][3]*pow(E_total,3))
				|| (sector == 3 && E_total/p < sf_up_lim[2][0]+ sf_up_lim[2][1]*E_total + sf_up_lim[2][2]*pow(E_total,2) + sf_up_lim[2][3]*pow(E_total,3)
								&& E_total/p > sf_lo_lim[2][0]+ sf_lo_lim[2][1]*E_total + sf_lo_lim[2][2]*pow(E_total,2) + sf_lo_lim[2][3]*pow(E_total,3))
				|| (sector == 4 && E_total/p < sf_up_lim[3][0]+ sf_up_lim[3][1]*E_total + sf_up_lim[3][2]*pow(E_total,2) + sf_up_lim[3][3]*pow(E_total,3)
								&& E_total/p > sf_lo_lim[3][0]+ sf_lo_lim[3][1]*E_total + sf_lo_lim[3][2]*pow(E_total,2) + sf_lo_lim[3][3]*pow(E_total,3))
				|| (sector == 5 && E_total/p < sf_up_lim[4][0]+ sf_up_lim[4][1]*E_total + sf_up_lim[4][2]*pow(E_total,2) + sf_up_lim[4][3]*pow(E_total,3)
								&& E_total/p > sf_lo_lim[4][0]+ sf_lo_lim[4][1]*E_total + sf_lo_lim[4][2]*pow(E_total,2) + sf_lo_lim[4][3]*pow(E_total,3))
				|| (sector == 6 && E_total/p < sf_up_lim[5][0]+ sf_up_lim[5][1]*E_total + sf_up_lim[5][2]*pow(E_total,2) + sf_up_lim[5][3]*pow(E_total,3)
								&& E_total/p > sf_lo_lim[5][0]+ sf_lo_lim[5][1]*E_total + sf_lo_lim[5][2]*pow(E_total,2) + sf_lo_lim[5][3]*pow(E_total,3)))
			) {
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
			elec_vars[14] = theta;
			elec_vars[15] = v_x;
			elec_vars[16] = v_y;
			elec_tuple->Fill(elec_vars);
			v_z_elec = v_z;
			valid_electron = true;
		}

		// Check if the particle is not an electron.
		else if (valid_electron){
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
			hadron_vars[19] = theta;
			hadron_vars[20] = v_x;
			hadron_vars[21] = v_y;
			if (pid!=11 && pid!=-11){hadron_tuple->Fill(hadron_vars);}
			if (pid==211){pion_tuple->Fill(hadron_vars);}
			else if (pid==-211){pion_minus_tuple->Fill(hadron_vars);}
			else if (pid==2212){proton_tuple->Fill(hadron_vars);}
		}
	}
	cout<<"Writing output tuples into disk "<<endl;
	//------root file output writing------
	output->cd();
	pion_tuple->Write();
	pion_minus_tuple->Write();
	//proton_tuple->Write();
	//hadron_tuple->Write();
	elec_tuple->Write();

	//------ PLOTS------
	cout<<"Creating plots "<<endl;
	//----ELECTRONS----
	//z vertex (total)
	draw_plot(elec_tuple, P_cut, "v_z",100,-15,6, "V_{z} [cm]", "dN/dV_{z}" , "e_v_z",
				output_location, output);

	//z vertex by sector
	draw_sector_plot(elec_tuple, P_cut, "v_z",100,-15,6, "V_{z} [cm]", "dN/dV_{z}",
						"e_vz_sector", output_location, output);

	//x vertex
	draw_plot(elec_tuple, P_cut, "v_x",100,-5,5, "V_{x} [cm]", "dN/dV_{z}" , "e_v_x",
				output_location, output);

	//y vertex
	draw_plot(elec_tuple, P_cut, "v_y",100,-5,5, "V_{y} [cm]", "dN/dV_{z}" , "e_v_y",
				output_location, output);
	//W2
	draw_plot(elec_tuple, P_cut, "W2",100,0,20, "W^{2}", "dN/dW^{2}" , "e_w2", output_location, output);

	//Q2
	draw_plot(elec_tuple, P_cut, "Q2",100,0,12, "Q^{2}", "dN/dQ^{2}" , "e_q2", output_location, output);

	//Nu
	draw_plot(elec_tuple, P_cut, "nu",100,0,12, "#nu", "dN/d#nu" , "e_nu", output_location, output);

	//Phi 
	draw_plot(elec_tuple, P_cut, "phi",360,-180,180, "#phi [deg]", "dN/d#phi", "e_phi",
				output_location, output);

	//X_b
	draw_plot(elec_tuple, P_cut, "x_bjorken",100, 0,1, "#x_{b}", "dN/dx_{b}", "e_x_b",
				output_location, output);

	//P vs Etot/P
	draw_plot_2D(elec_tuple, Main_cut, "E_total/p:E_total", 100,0,2,"P [GeV]",
					100, 0.150, 0.325, "E_{tot}/E_tot", "sf", output_location, output);

	//theta vs phi
	draw_plot_2D(elec_tuple, Main_cut, "phi:theta",100, 0, 1, "#theta",
					 180,-180,180,"#phi", "e_ThetaxPhi", output_location, output);

	//P vs theta
	draw_plot_2D(elec_tuple, Main_cut, "theta:p",100, 0, 12, "P",
					 100,0,1,"#theta", "e_PxTheta", output_location, output);

	//x_b vs Q2
	draw_plot_2D(elec_tuple, Main_cut, "Q2:x_bjorken",100, 0, 1, "X_{b}",
					 100,0,11,"Q^{2}}", "e_x_bxQ2", output_location, output);

	//Nu vs Q2
	draw_plot_2D(elec_tuple, Main_cut, "Q2:nu",100, 2, 11, "Nu",
					 100,1,11,"Q2", "e_NuxQ2", output_location, output);

	//----PION PLUS----
	//z vertex
	draw_plot(pion_tuple, P_cut&&DIS_cut, "v_z",100,-15,6, "V_{z} [cm]", "dN/dV_{z}", "pi_v_z",
				output_location, output);

	//z vertex difference (e-pi)
	draw_plot(pion_tuple, P_cut&&DIS_cut, "v_z_elec-v_z",100,-10,10, "V_{z e} - V_{z #pi} [cm]", "dN/dV_{z}", "pi_v_z_diff",
				output_location, output);

	//z_h
	draw_plot(pion_tuple, P_cut&&DIS_cut, "z_h",180,0,1, "Z_{h}", "dN/dZ_{h}", "pi_zh",
				output_location, output);

	//pt2
	draw_plot(pion_tuple, P_cut&&DIS_cut, "p_T2",100,0,5, "P_{T}^{2}", "dN/dP_{T}^{2}", "pi_pt2",
				output_location, output);

	//phi
	draw_plot(pion_tuple, P_cut&&DIS_cut, "phi",360,-180,180, "#phi [deg]", "dN/d#phi", "pi_phi",
				output_location, output);

	//Pt2 vz Zh
	draw_plot_2D(pion_tuple, P_cut&&DIS_cut, "z_h:p_T2",100, 0, 5, "P_{T}^{2}",
					 100, 0, 1,"Z_{h}", "pi_Pt2xZ", output_location, output);

	//----PION MINUS----
	//z vertex
	draw_plot(pion_minus_tuple, P_cut&&DIS_cut, "v_z",100,-15,6, "V_{z} [cm]", "dN/dV_{z}", "pim_v_z",
				output_location, output);

	//z vertex difference (e-pi)
	draw_plot(pion_minus_tuple, P_cut&&DIS_cut, "v_z_elec-v_z",100,-10,10, "V_{z e} - V_{z #pi} [cm]", "dN/dV_{z}", "pim_v_z_diff",
				output_location, output);

	//z_h
	draw_plot(pion_minus_tuple, P_cut&&DIS_cut, "z_h",100,0,1, "Z_{h}", "dN/dZ_{h}", "pim_zh",
				output_location, output);

	//pt2
	draw_plot(pion_minus_tuple, P_cut&&DIS_cut, "p_T2",100,0,5, "P_{T}^{2}", "dN/dP_{T}^{2}", "pim_pt2",
				output_location, output);

	//phi
	draw_plot(pion_minus_tuple, P_cut&&DIS_cut, "phi",360,-180,180, "#phi [deg]", "dN/d#phi", "pim_phi",
				output_location, output);

	//Pt2 vz Zh
	draw_plot_2D(pion_minus_tuple, P_cut&&DIS_cut, "z_h:p_T2",100, 0, 5, "P_{T}^{2}",
					 100, 0, 1,"Z_{h}", "pim_Pt2xZ", output_location, output);

	//----ALL HADRONS----
	//z vertex
	draw_plot(hadron_tuple, P_cut&&DIS_cut, "v_z",100,-15,6, "V_{z} [cm]", "dN/dV_{z}", "h_v_z",
				output_location, output);

	//z vertex difference (e-pi)
	draw_plot(hadron_tuple, P_cut&&DIS_cut, "v_z_elec-v_z",100,-10,10, "V_{z e} - V_{z #pi} [cm]", "dN/dV_{z}", "h_v_z_diff",
				output_location, output);

	//z_h
	draw_plot(hadron_tuple, P_cut&&DIS_cut, "z_h",100,0,1, "Z_{h}", "dN/dZ_{h}", "h_zh",
				output_location, output);

	//pt2
	draw_plot(hadron_tuple, P_cut&&DIS_cut, "p_T2",100,0,5, "P_{T}^{2}", "dN/dP_{T}^{2}", "h_pt2",
				output_location, output);

	//phi
	draw_plot(hadron_tuple, P_cut&&DIS_cut, "phi",360,-180,180, "#phi [deg]", "dN/d#phi", "h_phi",
				output_location, output);

	//Pt2 vz Zh
	draw_plot_2D(hadron_tuple, P_cut&&DIS_cut, "z_h:p_T2",100, 0, 5, "P_{T}^{2}",
					 100, 0, 1,"Z_{h}", "h_Pt2xZ", output_location, output);

	cout<<"Finished plotting. Cleaning "<<endl;
	delete pion_tuple;
	delete hadron_tuple;
	delete pion_minus_tuple;
	delete proton_tuple;
	delete elec_tuple;
	delete output;
}

//Main function that recieves a txt with a list of run number asn the name of the output file
void simple_plots(const char* inputFileName, TString Target, TString type="data"){
	ROOT::EnableImplicitMT();

	cout<<"Running macro based on file name"<<endl;
	// Open the input text file
    std::ifstream inputFile(inputFileName);

    // Create a TChain to combine input TNuples
    TChain* input_tuple = new TChain("data");
    TChain* input_tuple_mc = new TChain("MC");

    // Read each line from the text file and add the corresponding ROOT file to the TChain
    char buffer [10];
    std::string line;
    while (std::getline(inputFile, line)) {
    	int run_N = stoi(line);
    	sprintf(buffer,"%0*d", 6, run_N);
		TString run_N_str=TString(buffer);
        if (type=="data"){
        	cout<<"Adding run: "<<line<<endl;
        	input_tuple->Add(type+"/ntuples_dc_"+run_N_str+".root");
        }
        if (type=="simul"){
        	cout<<"Addind simul job: "<<line<<endl;
        	input_tuple->Add(type+"/"+Target+"/ntuples_dc_"+run_N_str+".root");
        	input_tuple_mc->Add(type+"/"+Target+"/ntuples_dc_"+run_N_str+".root");
        }
    }

    // Close the input file
    inputFile.close();

    //process the Tchain to make plots and output tuples
    TString output_location = "output/"+type+"/"+Target+"/";
    gSystem->Exec("mkdir -vp "+output_location);
    cout<<"Output location: "<<output_location<<endl;

    processChain(input_tuple, output_location);
    cout<<"Data tuple done"<<endl;

	if (type=="simul") {
		output_location=output_location+"thrown/";
		processChain(input_tuple_mc, output_location);
		cout<<"MC tuple done"<<endl;
	}

    //delete all objects
    delete input_tuple;
    delete input_tuple_mc;
}

//Main function that recieves a run number as the input
void simple_plots(int run_N=000000, TString Target="unkw", TString type="data"){
	ROOT::EnableImplicitMT();

	cout<<"Running macro based on run number"<<endl;

    // Create a TChain to load input TNuples
    TChain* input_tuple = new TChain("data");
    TChain* input_tuple_mc = new TChain("MC");

	//Transform input run number to Tstring with correct number of digits
	TString run_N_str = TString::Format("%06d", run_N);;

	//Output directory
	TString output_location;
	output_location = "output/"+type+"/"+Target+"/"+run_N_str+"/";
	gSystem->Exec("mkdir -vp "+output_location);
	cout<<"Output location: "<<output_location<<endl;

    if (type=="data"){
    	input_tuple->Add(type+"/ntuples_dc_"+run_N_str+".root");
 	}
    if (type=="simul"){
    	input_tuple->Add(type+"/"+Target+"/ntuples_dc_"+run_N_str+".root");
    	input_tuple_mc->Add(type+"/"+Target+"/ntuples_dc_"+run_N_str+".root");
    }

    //process the Tchain to make plots and output tuples
	processChain(input_tuple, output_location);
	cout<<"Data tuple done"<<endl;

	if (type=="simul") {
		output_location=output_location+"thrown/";
		processChain(input_tuple_mc, output_location);
		cout<<"MC tuple done"<<endl;
	}

	//delete all objects
	delete input_tuple;
	delete input_tuple_mc;
}