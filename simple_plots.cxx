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

Float_t pimass = 0.139570;
Float_t pmass = 0.938272;
Float_t c = 29.9702547;
int tar_n = -1;

bool psf_eval(int sector, double p, double E_PCAL, double E_ECIN){
	int bin;
	if 	 	(0<p && p<2) {bin=1;}
	else if (2<p && p<3) {bin=2;}
	else if (3<p && p<4) {bin=3;}
	else if (4<p && p<5) {bin=4;}
	else if (5<p && p<6) {bin=5;}
	else if (6<p && p<7) {bin=6;}
	else if (7<p && p<8) {bin=7;}
	else if (8<p && p<9) {bin=8;}
	else if (9<p && p<12){bin=9;}
	else {return false;}

	return (E_PCAL/p>psf_a[sector-1][bin-1]*E_ECIN/p+psf_b[sector-1][bin-1]);
}

bool sf_eval(int sector, double p, double E_total){
	double mu_sf = sf_pars[sector-1][0]+sf_pars[sector-1][1]/E_total+sf_pars[sector-1][2]/pow(E_total,2);
	double sigma_sf = sf_pars[sector-1][3]+sf_pars[sector-1][4]/E_total+sf_pars[sector-1][5]/pow(E_total,2);

	double spread = 3.5;
	double up_lim  = mu_sf+spread*sigma_sf;
	double low_lim = mu_sf-spread*sigma_sf;

	return (E_total/p < up_lim && E_total/p > low_lim);
}

double get_pion_D_T(double p, double path, double start_time, double TOF){
    double beta_pi	 = p/sqrt(p*p+pimass*pimass);
    double theo_time = (path/(c*beta_pi));
    double exp_time  = TOF - start_time;
    double D_T       = exp_time - theo_time;
    return D_T;
}

double get_prot_D_T(double p, double path, double start_time, double TOF){
    double beta_p	 = p/sqrt(p*p+pmass*pmass);
    double theo_time = (path/(c*beta_p));
    double exp_time  = TOF - start_time;
    double D_T       = exp_time - theo_time;
    return D_T;
}

bool valid_pion(double p, double D_T){ //using carbon for now (decide if using a universal cut or different for everyone)
	double up_lim = sqrt(pow(pion_cuts[0],2)+pow(pion_cuts[1]/pow(p,pion_cuts[2]),2));
	double low_lim = -sqrt(pow(pion_cuts[3],2)+pow(pion_cuts[4]/pow(p,pion_cuts[5]),2));

	return (D_T< up_lim && D_T > low_lim);
}

bool valid_pionm(double p, double D_T){ //using carbon for now (decide if using a universal cut or different for everyone)
	double up_lim = sqrt(pow(pionm_cuts[0],2)+pow(pionm_cuts[1]/pow(p,pionm_cuts[2]),2));
	double low_lim = -sqrt(pow(pionm_cuts[3],2)+pow(pionm_cuts[4]/pow(p,pionm_cuts[5]),2));

	return (D_T< up_lim && D_T > low_lim);
}

bool valid_cd_pion(double p, double D_T){ //using carbon for now (decide if using a universal cut or different for everyone)
	double up_lim = sqrt(pow(cd_pion_cuts[0],2)+pow(cd_pion_cuts[1]/pow(p,cd_pion_cuts[2]),2));
	double low_lim = -sqrt(pow(cd_pion_cuts[3],2)+pow(cd_pion_cuts[4]/pow(p,cd_pion_cuts[5]),2));

	return (D_T< up_lim && D_T > low_lim);
}

bool valid_cd_pionm(double p, double D_T){ //using carbon for now (decide if using a universal cut or different for everyone)
	double up_lim = sqrt(pow(cd_pionm_cuts[0],2)+pow(cd_pionm_cuts[1]/pow(p,cd_pionm_cuts[2]),2));
	double low_lim = -sqrt(pow(cd_pionm_cuts[3],2)+pow(cd_pionm_cuts[4]/pow(p,cd_pionm_cuts[5]),2));

	return (D_T< up_lim && D_T > low_lim);
}

int vertex_sel(int sector, double vz){ //using old cuts, change later for target dependent cuts
	int targ_type;

	if 		(vz>vertex_cut[tar_n][sector-1][0] && vz<vertex_cut[tar_n][sector-1][1]){targ_type = 1;} //liquid
	else if (vz>vertex_cut[tar_n][sector-1][2] && vz<vertex_cut[tar_n][sector-1][3]){targ_type = 2;} //solid
	else 	{targ_type=0;} //none

	return targ_type;
}

//plot 1D plots
void draw_plot(TNtuple* tuple, TCut cut, char const* var, int nbins, float xmin, float xmax,
				TString xtitle, TString ytitle, TString output, TString location, TFile* output_file){
	TCanvas *canvas = new TCanvas("canvas","canvas",1000,600);
	canvas->cd();
	TString histo_to_draw;
	histo_to_draw.Form("%s>>histo(%i,%f,%f)", var, nbins, xmin, xmax);
	tuple->Draw(histo_to_draw,cut,"COLZ");
	TH1F *histo = (TH1F*)gDirectory->GetList()->FindObject("histo");
	//if (var=="D_T") {gPad->SetLogy(1);}
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
		histo[i-1]->SetTitle(xtitle+Form(" Sector %i",i));
		histo[i-1]->Draw("COLZ");
		histo[i-1]->Write(output+Form("_%i",i));
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
	gPad->SetLogz(1);
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
void processTuple(TNtuple* input_tuple, TString output_location) {
	//create output file and output directory in case it doenst exist
	cout<<"Creating output directories "<<endl;
	std::string command = std::string("mkdir -p ") + output_location.Data();
	gSystem->Exec(command.c_str());
	TFile *output = new TFile(output_location+"out_clas12.root","RECREATE");

	Float_t pid, NDF, chi2, Q2, nu, vz, vt, z_h, p, p_T2, p_L2, E_total, E_ECIN, E_PCAL, E_ECOU, event_num, vx_elec, vy_elec, vz_elec, vt_elec, phi, x_bjorken, y_bjorken, W2, charge, beta, sector, phi_PQ, theta, vx, vy, status, PCAL_V, PCAL_W, DC_R1_edge, DC_R2_edge, DC_R3_edge, DC_R1_x, DC_R2_x, DC_R3_x, DC_R1_y, DC_R2_y, DC_R3_y, targ_type, path_tof, time_tof, start_time, tof_elec, D_T, D_Tp, D_Te; 
	Float_t rad2deg = 57.2958;

	cout<<"Reading input tuple"<<endl;
	//------Read branches with variables needed for cuts and plots------
	input_tuple->SetBranchAddress("pid",&pid);
	input_tuple->SetBranchAddress("NDF",&NDF);
	input_tuple->SetBranchAddress("chi2",&chi2);
	input_tuple->SetBranchAddress("Q2",&Q2);
	input_tuple->SetBranchAddress("nu",&nu);
	input_tuple->SetBranchAddress("vx",&vx);
	input_tuple->SetBranchAddress("vy",&vy);
	input_tuple->SetBranchAddress("vz",&vz);
	input_tuple->SetBranchAddress("vt",&vt);
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
	input_tuple->SetBranchAddress("x_bjorken",&x_bjorken);
	input_tuple->SetBranchAddress("PCAL_V",&PCAL_V);
	input_tuple->SetBranchAddress("PCAL_W",&PCAL_W);
	input_tuple->SetBranchAddress("DC_R1_edge",&DC_R1_edge);
	input_tuple->SetBranchAddress("DC_R2_edge",&DC_R2_edge);
	input_tuple->SetBranchAddress("DC_R3_edge",&DC_R3_edge);
	input_tuple->SetBranchAddress("DC_R1_x",&DC_R1_x);
	input_tuple->SetBranchAddress("DC_R2_x",&DC_R2_x);
	input_tuple->SetBranchAddress("DC_R3_x",&DC_R3_x);
	input_tuple->SetBranchAddress("DC_R1_y",&DC_R1_y);
	input_tuple->SetBranchAddress("DC_R2_y",&DC_R2_y);
	input_tuple->SetBranchAddress("DC_R3_y",&DC_R3_y);
	input_tuple->SetBranchAddress("status",&status);
	input_tuple->SetBranchAddress("path_tof",&path_tof);
	input_tuple->SetBranchAddress("time_tof",&time_tof);
	input_tuple->SetBranchAddress("start_time",&start_time);

	//------output ntuples------
	Float_t hadron_vars[38];
	Float_t elec_vars[27];
	const char* hadron_varslist = "pid:NDF:chi2:Q2:nu:vx:vy:vz:vt:p:p_T2:p_L2:E_total:E_ECIN:E_ECOU:z_h:vx_elec:vy_elec:vz_elec:vt_elec:x_bjorken:y_bjorken:W2:beta:phi:sector:phi_PQ:theta:targ_type:D_T:D_Tp:D_Te:DC_R1_x:DC_R2_x:DC_R3_x:DC_R1_y:DC_R2_y:DC_R3_y";
	const char* elec_varslist = "pid:NDF:chi2:Q2:nu:vx:vy:vz:vt:p:E_total:E_ECIN:E_ECOU:x_bjorken:y_bjorken:W2:beta:phi:sector:theta:targ_type:DC_R1_x:DC_R2_x:DC_R3_x:DC_R1_y:DC_R2_y:DC_R3_y";
	TNtuple *pion_tuple = new TNtuple("pion_ntuple","pions",hadron_varslist);
	TNtuple *hadron_tuple = new TNtuple("hadron_ntuple","hadrons",hadron_varslist);
	TNtuple *pion_minus_tuple = new TNtuple("pion_minus_ntuple","positives",hadron_varslist);
	TNtuple *proton_tuple = new TNtuple("proton_ntuple","protons",hadron_varslist);
	TNtuple *elec_tuple = new TNtuple("elec_tuple","electrons",elec_varslist);

	vz_elec = -99;
	vt_elec = -99;
	bool valid_electron = false;

    cout<<"Starting processing loop "<<endl;
	//Selection of particles
	Long64_t n_entries = input_tuple->GetEntries();
	for (Long64_t i=0;i<n_entries;i++) { //changed n_entries to 1000000 for testing
		input_tuple->GetEntry(i);
		if (pid == 11){valid_electron = false;}
		//This part assumes that all hadrons after an electron come from that electron to save its vz
		// Check if the particle fullfills being the scattered electron.
		if  (pid==11 && status<=-2000 && status>-3000				//basic FD electron cut from CLAS12 event builder
			&& Q2>Q2_min && W2>W2_min && y_bjorken<y_bjorken_max 	//DIS cuts
			//&& p>p_min && p<p_max									//momentum cut
			&& theta*rad2deg>theta_min								//theta cut
			&& PCAL_V>PCAL_V_fc && PCAL_W>PCAL_W_fc					//PCAL fiducial cuts
			&& DC_R1_edge>DC_R1_fc_e && DC_R2_edge>DC_R1_fc_e && DC_R3_edge>DC_R3_fc_e //DC fiducial cuts
			//sector based cuts
			&& psf_eval(sector, p, E_PCAL, E_ECIN) //Partial sampling fraction
			&& sf_eval(sector, p, E_total) //Partial sampling fraction
			) {
			targ_type = vertex_sel(sector, vz);
			elec_vars[0]  = pid;
			elec_vars[1]  = NDF;
			elec_vars[2]  = chi2;
			elec_vars[3]  = Q2;
			elec_vars[4]  = nu;
			elec_vars[5]  = vx;
			elec_vars[6]  = vy;
			elec_vars[7]  = vz;
			elec_vars[8]  = vt;
			elec_vars[9]  = p;
			elec_vars[10]  = E_total;
			elec_vars[11]  = E_ECIN;
			elec_vars[12]  = E_ECOU;
			elec_vars[13] = x_bjorken;
			elec_vars[14] = y_bjorken;
			elec_vars[15] = W2;
			elec_vars[16] = beta;
			elec_vars[17] = phi*rad2deg;
			elec_vars[18] = sector;
			elec_vars[19] = theta;
			elec_vars[20] = targ_type;
			elec_vars[21] = DC_R1_x;
			elec_vars[22] = DC_R2_x;
			elec_vars[23] = DC_R3_x;
			elec_vars[24] = DC_R1_y;
			elec_vars[25] = DC_R2_y;
			elec_vars[26] = DC_R3_y;
			elec_tuple->Fill(elec_vars);
			vx_elec = vx;
			vy_elec = vy;
			vz_elec = vz;
			vt_elec = vt;
			tof_elec = time_tof;
			valid_electron = true;
		}

		// Check if the particle is not an electron.
		else if (valid_electron){
			hadron_vars[0]  = pid;
			hadron_vars[1]  = NDF;
			hadron_vars[2]  = chi2;
			hadron_vars[3]  = Q2;
			hadron_vars[4]  = nu;
			hadron_vars[5]  = vx;
			hadron_vars[6]  = vy;
			hadron_vars[7]  = vz;
			hadron_vars[8]  = vt;
			hadron_vars[9]  = p;
			hadron_vars[10] = p_T2;
			hadron_vars[11] = p_L2;
			hadron_vars[12] = E_total;
			hadron_vars[13] = E_ECIN;
			hadron_vars[14] = E_ECOU;
			hadron_vars[15] = z_h;
			hadron_vars[16] = vx_elec;
			hadron_vars[17] = vy_elec;
			hadron_vars[18] = vz_elec;
			hadron_vars[19] = vt_elec;
			hadron_vars[20] = x_bjorken;
			hadron_vars[21] = y_bjorken;
			hadron_vars[22] = W2;
			hadron_vars[23] = beta;
			hadron_vars[24] = phi*rad2deg;
			hadron_vars[25] = sector;
			hadron_vars[26] = phi_PQ;
			hadron_vars[27] = theta;
			hadron_vars[28] = targ_type;
			hadron_vars[29] = D_T = get_pion_D_T(p, path_tof, start_time, time_tof);
			hadron_vars[30] = D_Tp = get_prot_D_T(p, path_tof, start_time, time_tof);
			hadron_vars[31] = D_Te =time_tof-tof_elec;
			hadron_vars[32] = DC_R1_x;
			hadron_vars[33] = DC_R2_x;
			hadron_vars[34] = DC_R3_x;
			hadron_vars[35] = DC_R1_y;
			hadron_vars[36] = DC_R2_y;
			hadron_vars[37] = DC_R3_y;

			//all hadrons
			if (pid!=11 && pid!=-11 && status<3000 && (detector=="fd" || detector=="all") && pid!=22){hadron_tuple->Fill(hadron_vars);}

			else if (pid!=11 && pid!=-11 && status>4000 && (detector=="cd" || detector=="all") && pid!=22){hadron_tuple->Fill(hadron_vars);}

			//protons
			if (pid==2212 && status<3000 && (detector=="fd" || detector=="all")){proton_tuple->Fill(hadron_vars);}

			else if (pid==2212 && status>4000 && (detector=="cd" || detector=="all")){proton_tuple->Fill(hadron_vars);}

			////positive pions
			else if (pid==211 && status<3000  && (detector=="fd" || detector=="all") && valid_pion(p, D_T)
				&& DC_R1_fc_pi<DC_R1_edge && DC_R2_fc_pi<DC_R2_edge && DC_R3_fc_pi<DC_R3_edge){
					pion_tuple->Fill(hadron_vars);}

			else if (pid==211 && status>4000 && (detector=="cd" || detector=="all") && valid_cd_pion(p, D_T)){
					pion_tuple->Fill(hadron_vars);}

			//negative pions
			else if (pid==-211 && status<3000 && (detector=="fd" || detector=="all") && valid_pionm(p, D_T)
				&& DC_R1_fc_pi<DC_R1_edge && DC_R2_fc_pi<DC_R2_edge && DC_R3_fc_pi<DC_R3_edge){
					pion_minus_tuple->Fill(hadron_vars);}

			else if (pid==-211 && status>4000 && (detector=="cd" || detector=="all") && valid_cd_pionm(p, D_T)){
					pion_minus_tuple->Fill(hadron_vars);}

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

	//-----------------
	//----ELECTRONS----
	//-----------------

	//z vertex (total)
	draw_plot(elec_tuple, "", "vz",100,-15,6, "V_{z} [cm]", "dN/dV_{z}" , "e_vz",
				output_location, output);

	//z vertex by sector
	draw_sector_plot(elec_tuple, "", "vz",100,-15,6, "V_{z} [cm]", "dN/dV_{z}",
						"e_vz_sector", output_location, output);

	//x vertex
	draw_plot(elec_tuple, "", "vx",100,-5,5, "V_{x} [cm]", "dN/dV_{z}" , "e_vx",
				output_location, output);

	//y vertex
	draw_plot(elec_tuple, "", "vy",100,-5,5, "V_{y} [cm]", "dN/dV_{z}" , "e_vy",
				output_location, output);
	//W2
	draw_plot(elec_tuple, "", "W2",100,0,20, "W^{2}", "dN/dW^{2}" , "e_w2", output_location, output);

	//Q2
	draw_plot(elec_tuple, "", "Q2",100,0,12, "Q^{2}", "dN/dQ^{2}" , "e_q2", output_location, output);

	//Nu
	draw_plot(elec_tuple, "", "nu",100,0,12, "#nu", "dN/d#nu" , "e_nu", output_location, output);

	//Phi 
	draw_plot(elec_tuple, "", "phi",360,-180,180, "#phi [deg]", "dN/d#phi", "e_phi",
				output_location, output);

	//X_b
	draw_plot(elec_tuple, "", "x_bjorken",100, 0,1, "#x_{b}", "dN/dx_{b}", "e_x_b",
				output_location, output);

	//P vs Etot/P
	draw_plot_2D(elec_tuple, "", "E_total/p:E_total", 100,0,2,"E_{tot} [GeV]",
					100, 0.150, 0.325, "E_{tot}/P", "sf", output_location, output);

	//theta vs phi
	draw_plot_2D(elec_tuple, "", "phi:theta",100, 0, 1, "#theta",
					 180,-180,180,"#phi", "e_ThetaxPhi", output_location, output);

	//P vs theta
	draw_plot_2D(elec_tuple, "", "theta:p",100, 0, 12, "P",
					 100,0,1,"#theta", "e_PxTheta", output_location, output);

	//x_b vs Q2
	draw_plot_2D(elec_tuple, "", "Q2:x_bjorken",100, 0, 1, "X_{b}",
					 100,0,11,"Q^{2}}", "e_x_bxQ2", output_location, output);

	//Nu vs Q2
	draw_plot_2D(elec_tuple, "", "Q2:nu",100, 2, 11, "Nu",
					 100,1,11,"Q2", "e_NuxQ2", output_location, output);

	//x vs y (DC_R1)
	draw_plot_2D(elec_tuple, "", "DC_R1_y:DC_R1_x",300, -400, 400, "x [cm]",
					 300, -400, 400,"y [cm]", "e_xy_DCR1", output_location, output);

	//x vs y (DC_R1)
	draw_plot_2D(input_tuple, "pid==11", "DC_R1_y:DC_R1_x",300, -400, 400, "x [cm]",
					 300, -400, 400,"y [cm]", "e_nocut_xy_DCR1", output_location, output);

	//x vs y (DC_R2)
	draw_plot_2D(elec_tuple, "", "DC_R2_y:DC_R2_x",300, -400, 400, "x [cm]",
					 300, -400, 400,"y [cm]", "e_xy_DCR2", output_location, output);

	//x vs y (DC_R2)
	draw_plot_2D(input_tuple, "pid==11", "DC_R2_y:DC_R2_x",300, -400, 400, "x [cm]",
					 300, -400, 400,"y [cm]", "e_nocut_xy_DCR2", output_location, output);

	//x vs y (DC_R3)
	draw_plot_2D(elec_tuple, "", "DC_R3_y:DC_R3_x",300, -400, 400, "x [cm]",
					 300, -400, 400,"y [cm]", "e_xy_DCR3", output_location, output);

	//x vs y (DC_R3)
	draw_plot_2D(input_tuple, "pid==11", "DC_R3_y:DC_R3_x",300, -400, 400, "x [cm]",
					 300, -400, 400,"y [cm]", "e_nocut_xy_DCR3", output_location, output);

	//-------------------
	//----PION PLUS------
	//-------------------

	//z vertex
	draw_plot(pion_tuple, "", "vz",100,-15,6, "V_{z} [cm]", "dN/dV_{z}", "pi_vz",
				output_location, output);

	//time diff theo vs exp vertex
	draw_plot(pion_tuple, "", "D_T",100,-5,5, "Dt_{#pi} [ns]", "dN/dDT", "pi_dt",
				output_location, output);

	//z_h
	draw_plot(pion_tuple, "", "z_h",180,0,1, "Z_{h}", "dN/dZ_{h}", "pi_zh",
				output_location, output);

	//pt2
	draw_plot(pion_tuple, "", "p_T2",100,0,5, "P_{T}^{2}", "dN/dP_{T}^{2}", "pi_pt2",
				output_location, output);

	//phi
	draw_plot(pion_tuple, "", "phi",360,-180,180, "#phi [deg]", "dN/d#phi", "pi_phi",
				output_location, output);

	//phiPQ
	draw_plot(pion_tuple, "", "phi_PQ",360,-3.15,3.15, "#phi_PQ [deg]", "dN/d#phi_PQ", "pi_phi_PQ",
				output_location, output);

	//Pt2 vz Zh
	draw_plot_2D(pion_tuple, "", "z_h:p_T2",100, 0, 5, "P_{T}^{2}",
					 100, 0, 1,"Z_{h}", "pi_Pt2xZ", output_location, output);

	//P vz D_T
	draw_plot_2D(pion_tuple, "", "D_T:p",100, 0, 5, "P",
					 100, -55, 5,"#DeltaT_{#pi}", "pi_PxD_t", output_location, output);

	//P vz D_T
	draw_plot_2D(hadron_tuple, "pid==211", "D_T:p",300, 0, 5, "P",
					 100, -55, 5,"#DeltaT_{#pi}", "pi_nocut_PxD_t", output_location, output);

	//x vs y (DC_R1)
	draw_plot_2D(pion_tuple, "", "DC_R1_y:DC_R1_x",300, -400, 400, "x [cm]",
					 300, -400, 400,"y [cm]", "pi_xy_DCR1", output_location, output);

	//x vs y (DC_R1)
	draw_plot_2D(hadron_tuple, "pid==211", "DC_R1_y:DC_R1_x",300, -400, 400, "x [cm]",
					 300, -400, 400,"y [cm]", "pi_nocut_xy_DCR1", output_location, output);

	//x vs y (DC_R2)
	draw_plot_2D(pion_tuple, "", "DC_R2_y:DC_R2_x",300, -400, 400, "x [cm]",
					 300, -400, 400,"y [cm]", "pi_xy_DCR2", output_location, output);

	//x vs y (DC_R2)
	draw_plot_2D(hadron_tuple, "pid==211", "DC_R2_y:DC_R2_x",300, -400, 400, "x [cm]",
					 300, -400, 400,"y [cm]", "pi_nocut_xy_DCR2", output_location, output);

	//x vs y (DC_R3)
	draw_plot_2D(pion_tuple, "", "DC_R3_y:DC_R3_x",300, -400, 400, "x [cm]",
					 300, -400, 400,"y [cm]", "pi_xy_DCR3", output_location, output);

	//x vs y (DC_R3)
	draw_plot_2D(hadron_tuple, "pid==211", "DC_R3_y:DC_R3_x",300, -400, 400, "x [cm]",
					 300, -400, 400,"y [cm]", "pi_nocut_xy_DCR3", output_location, output);

	//EXTRAS
/*
	//x vertex
	draw_plot(pion_tuple, "", "vx",100,-1,1, "V_{x} [cm]", "dN/dV_{z}", "pi_vx",
				output_location, output);
	//y vertex
	draw_plot(pion_tuple, "", "vy",100,-1,1, "V_{y} [cm]", "dN/dV_{y}", "pi_vy",
				output_location, output);

	//time diff theo vs exp vertex (e time)
	draw_plot(pion_tuple, "", "D_Te",1000,-75,-20, "Dt_{e} [ns]", "dN/dDTe", "pi_dte",
				output_location, output);

	//time diff theo vs exp vertex (p time)
	draw_plot(pion_tuple, "", "D_Tp",1000,-55,5, "Dt_{p} [ns]", "dN/dDTp", "pi_dtp",
				output_location, output);

	//time diff theo vs exp vertex (wide range)
	draw_plot(pion_tuple, "", "D_T",600,-55,5, "Dt_{#pi} [ns]", "dN/dDT", "pi_dt_wide",
				output_location, output);

	//P vz D_Te
	draw_plot_2D(pion_tuple, "", "D_Te:p",100, 0, 5, "P",
					 100, -75, -20,"#DeltaT_{e}", "pi_PxD_te", output_location, output);

	//P vz D_Tp
	draw_plot_2D(pion_tuple, "", "D_Tp:p",100, 0, 5, "P",
					 100, -55, 5,"#DeltaT_{p}", "pi_PxD_tp", output_location, output);

	//z vertex difference (e-pi)
	draw_plot(pion_tuple, "", "vz_elec-vz",100,-10,10, "V_{z e} - V_{z #pi} [cm]", "dN/dV_{z}", "pi_vz_diff",
				output_location, output);
	//z vertex difference (e-pi)
	draw_plot(hadron_tuple, "pid==211", "vz_elec-vz",100,-10,10, "V_{z e} - V_{z #pi} [cm]", "dN/dV_{z}", "pi_nocut_vz_diff",
				output_location, output);
	//z vertex difference in D2 target (e-pi)
	draw_plot(pion_tuple, "targ_type==1", "vz_elec-vz",100,-10,10, "V_{z e} - V_{z #pi} [cm]", "dN/dV_{z}", "pi_vz_diff_liq",
				output_location, output);
	//z vertex difference in sol target (e-pi)
	draw_plot(pion_tuple, "targ_type==2", "vz_elec-vz",100,-10,10, "V_{z e} - V_{z #pi} [cm]", "dN/dV_{z}", "pi_vz_diff_sol",
				output_location, output);

	//z vertex difference vs theta (e-pi)
	draw_plot_2D(pion_tuple, "", "theta:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, 0, 2.5,"#theta", "pi_vz_diffxtheta", output_location, output);
	//z vertex difference vs theta in liq target (e-pi)
	draw_plot_2D(pion_tuple, "targ_type==1", "theta:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, 0, 2.5,"#theta", "pi_vz_diffxtheta_liq", output_location, output);
	//z vertex difference vs theta in sol target (e-pi)
	draw_plot_2D(pion_tuple, "targ_type==2", "theta:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, 0, 2.5,"#theta", "pi_vz_diffxtheta_sol", output_location, output);

	//z vertex difference vs phi (e-pi)
	draw_plot_2D(pion_tuple, "", "phi:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, -180, 180,"#phi", "pi_vz_diffxphi", output_location, output);
	//z vertex difference vs phi in liq target (e-pi)
	draw_plot_2D(pion_tuple, "targ_type==1", "phi:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, -180, 180,"#phi", "pi_vz_diffxphi_liq", output_location, output);
	//z vertex difference vs phi in sol target (e-pi)
	draw_plot_2D(pion_tuple, "targ_type==2", "phi:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, -180, 180,"#phi", "pi_vz_diffxphi_sol", output_location, output);

	//z vertex difference vs p (e-pi)
	draw_plot_2D(pion_tuple, "", "p:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, 0, 3,"p", "pi_vz_diffxp", output_location, output);
	//z vertex difference vs p e- in liq target (e-pi)
	draw_plot_2D(pion_tuple, "targ_type==1", "p:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, 0, 3,"p", "pi_vz_diffxp_liq", output_location, output);
	//z vertex difference vs p e- in sol target (e-pi)
	draw_plot_2D(pion_tuple, "targ_type==2", "p:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, 0, 3,"p", "pi_vz_diffxp_sol", output_location, output);

	//z vertex difference vs NDF (e-pi)
	draw_plot_2D(pion_tuple, "", "NDF:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 8, 0.5, 8.5,"NDF", "pi_vz_diffxndf", output_location, output);
	//z vertex difference vs NDF in liq target (e-pi)
	draw_plot_2D(pion_tuple, "targ_type==1", "NDF:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 8, 0.5, 8.5,"NDF", "pi_vz_diffxndf_liq", output_location, output);
	//z vertex difference vs NDF in sol target (e-pi)
	draw_plot_2D(pion_tuple, "targ_type==2", "NDF:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 8, 0.5, 8.5,"NDF", "pi_vz_diffxndf_sol", output_location, output);

	//z vertex difference vs chi2/NDF (e-pi)
	draw_plot_2D(pion_tuple, "", "(chi2/NDF):(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, 0, 50,"chi2/NDF", "pi_vz_diffxchi2ndf", output_location, output);
	//z vertex difference vs chi2/NDF in liq target (e-pi)
	draw_plot_2D(pion_tuple, "targ_type==1", "(chi2/NDF):(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, 0, 50,"chi2/NDF", "pi_vz_diffxchi2ndf_liq", output_location, output);
	//z vertex difference vs chi2/NDF in sol target (e-pi)
	draw_plot_2D(pion_tuple, "targ_type==2", "(chi2/NDF):(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, 0, 50,"chi2/NDF", "pi_vz_diffxchi2ndf_sol", output_location, output);

	//z vertex difference vs vt(e-pi)
	draw_plot_2D(pion_tuple, "", "vt:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, 85, 105,"vt [ns]", "pi_vz_diffxvt", output_location, output);
	//z vertex difference vs vt in liq target (e-pi)
	draw_plot_2D(pion_tuple, "targ_type==1", "vt:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, 85, 105,"vt [ns]", "pi_vz_diffxvt_liq", output_location, output);
	//z vertex difference vs vt in sol target (e-pi)
	draw_plot_2D(pion_tuple, "targ_type==2", "vt:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, 85, 105,"vt [ns]", "pi_vz_diffxvt_sol", output_location, output);

	//z vertex difference vs vt difference (e-pi)
	draw_plot_2D(pion_tuple, "", "(vt_elec-vt):(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, -0.5, 0.5,"V_{t e} - V_{t #pi} [ns]", "pi_vz_diffxvt_diff", output_location, output);
	//z vertex difference vs vt difference (e-pi)
	draw_plot_2D(hadron_tuple, "pid==211", "(vt_elec-vt):(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, -0.5, 0.5,"V_{t e} - V_{t #pi} [ns]", "pi_nocut_vz_diffxvt_diff", output_location, output);
	//z vertex difference vs vt difference in liq target (e-pi)
	draw_plot_2D(pion_tuple, "targ_type==1", "(vt_elec-vt):(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, -0.5, 0.5,"V_{t e} - V_{t #pi} [ns]", "pi_vz_diffxvt_diff_liq", output_location, output);
	//z vertex difference vs vt difference in sol target (e-pi)
	draw_plot_2D(pion_tuple, "targ_type==2", "(vt_elec-vt):(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, -0.5, 0.5,"V_{t e} - V_{t #pi} [ns]", "pi_vz_diffxvt_diff_sol", output_location, output);

	//Beta vs P
	draw_plot_2D(pion_tuple, "", "beta:p",100, 0, 3, "P",
					 100, 0, 2,"#beta", "pi_betaxP", output_location, output);

	//Beta vs P left peak
	draw_plot_2D(pion_tuple, "(vz_elec-vz)<-3", "beta:p",100, 0, 3, "P",
					 100, 0, 2,"#beta", "pi_betaxP_left", output_location, output);

	//Beta vs P central peak
	draw_plot_2D(pion_tuple, "(vz_elec-vz)>-3&&(vz_elec-vz)<3", "beta:p",100, 0, 3, "P",
					 100, 0, 2,"#beta", "pi_betaxP_center", output_location, output);

	//Beta vs P right peak
	draw_plot_2D(pion_tuple, "(vz_elec-vz)>3", "beta:p",100, 0, 3, "P",
					 100, 0, 2,"#beta", "pi_betaxP_right", output_location, output);
*/	
	//-------------------
	//----PION MINUS------
	//-------------------

	//z vertex
	draw_plot(pion_minus_tuple, "", "vz",100,-15,6, "V_{z} [cm]", "dN/dV_{z}", "pim_vz",
				output_location, output);

	//time diff theo vs exp vertex
	draw_plot(pion_minus_tuple, "", "D_T",100,-5,5, "Dt_{#pi} [ns]", "dN/dDT", "pim_dt",
				output_location, output);

	//z_h
	draw_plot(pion_minus_tuple, "", "z_h",180,0,1, "Z_{h}", "dN/dZ_{h}", "pim_zh",
				output_location, output);

	//pt2
	draw_plot(pion_minus_tuple, "", "p_T2",100,0,5, "P_{T}^{2}", "dN/dP_{T}^{2}", "pim_pt2",
				output_location, output);

	//phi
	draw_plot(pion_minus_tuple, "", "phi",360,-180,180, "#phi [deg]", "dN/d#phi", "pim_phi",
				output_location, output);

	//phiPQ
	draw_plot(pion_minus_tuple, "", "phi_PQ",360,-3.15,3.15, "#phi_PQ [deg]", "dN/d#phi_PQ", "pim_phi_PQ",
				output_location, output);

	//Pt2 vz Zh
	draw_plot_2D(pion_minus_tuple, "", "z_h:p_T2",100, 0, 5, "P_{T}^{2}",
					 100, 0, 1,"Z_{h}", "pim_Pt2xZ", output_location, output);

	//P vz D_T
	draw_plot_2D(pion_minus_tuple, "", "D_T:p",100, 0, 5, "P",
					 100, -55, 5,"#DeltaT_{#pi}", "pim_PxD_t", output_location, output);

	//P vz D_T
	draw_plot_2D(hadron_tuple, "pid==-211", "D_T:p",100, 0, 5, "P",
					 100, -55, 5,"#DeltaT_{#pi}", "pim_nocut_PxD_t", output_location, output);

	//x vs y (DC_R1)
	draw_plot_2D(pion_minus_tuple, "", "DC_R1_y:DC_R1_x",300, -400, 400, "x [cm]",
					 300, -400, 400,"y [cm]", "pim_xy_DCR1", output_location, output);

	//x vs y (DC_R1)
	draw_plot_2D(hadron_tuple, "pid==-211", "DC_R1_y:DC_R1_x",300, -400, 400, "x [cm]",
					 300, -400, 400,"y [cm]", "pim_nocut_xy_DCR1", output_location, output);

	//x vs y (DC_R2)
	draw_plot_2D(pion_minus_tuple, "", "DC_R2_y:DC_R2_x",300, -400, 400, "x [cm]",
					 300, -400, 400,"y [cm]", "pim_xy_DCR2", output_location, output);

	//x vs y (DC_R2)
	draw_plot_2D(hadron_tuple, "pid==-211", "DC_R2_y:DC_R2_x",300, -400, 400, "x [cm]",
					 300, -400, 400,"y [cm]", "pim_nocut_xy_DCR2", output_location, output);

	//x vs y (DC_R3)
	draw_plot_2D(pion_minus_tuple, "", "DC_R3_y:DC_R3_x",300, -400, 400, "x [cm]",
					 300, -400, 400,"y [cm]", "pim_xy_DCR3", output_location, output);

	//x vs y (DC_R3)
	draw_plot_2D(hadron_tuple, "pid==-211", "DC_R3_y:DC_R3_x",300, -400, 400, "x [cm]",
					 300, -400, 400,"y [cm]", "pim_nocut_xy_DCR3", output_location, output);

	//EXTRAS
/*
	//x vertex
	draw_plot(pion_minus_tuple, "", "vx",100,-1,1, "V_{x} [cm]", "dN/dV_{z}", "pim_vx",
				output_location, output);
	//y vertex
	draw_plot(pion_minus_tuple, "", "vy",100,-1,1, "V_{y} [cm]", "dN/dV_{y}", "pim_vy",
				output_location, output);

	//time diff theo vs exp vertex (e time)
	draw_plot(pion_minus_tuple, "", "D_Te",1000,-75,-20, "Dt_{e} [ns]", "dN/dDTe", "pim_dte",
				output_location, output);

	//time diff theo vs exp vertex (p time)
	draw_plot(pion_minus_tuple, "", "D_Tp",1000,-55,5, "Dt_{p} [ns]", "dN/dDTp", "pim_dtp",
				output_location, output);

	//time diff theo vs exp vertex (wide range)
	draw_plot(pion_minus_tuple, "", "D_T",600,-55,5, "Dt_{#pi} [ns]", "dN/dDT", "pim_dt_wide",
				output_location, output);

	//P vz D_Te
	draw_plot_2D(pion_minus_tuple, "", "D_Te:p",100, 0, 5, "P",
					 100, -75, -20,"#DeltaT_{e}", "pim_PxD_te", output_location, output);

	//P vz D_Tp
	draw_plot_2D(pion_minus_tuple, "", "D_Tp:p",100, 0, 5, "P",
					 100, -55, 5,"#DeltaT_{p}", "pim_PxD_tp", output_location, output);

	//z vertex difference (e-pi)
	draw_plot(pion_minus_tuple, "", "vz_elec-vz",100,-10,10, "V_{z e} - V_{z #pi} [cm]", "dN/dV_{z}", "pim_vz_diff",
				output_location, output);
	//z vertex difference (e-pi)
	draw_plot(hadron_tuple, "pid==-211", "vz_elec-vz",100,-10,10, "V_{z e} - V_{z #pi} [cm]", "dN/dV_{z}", "pim_nocut_vz_diff",
				output_location, output);
	//z vertex difference in D2 target (e-pi)
	draw_plot(pion_minus_tuple, "targ_type==1", "vz_elec-vz",100,-10,10, "V_{z e} - V_{z #pi} [cm]", "dN/dV_{z}", "pim_vz_diff_liq",
				output_location, output);
	//z vertex difference in sol target (e-pi)
	draw_plot(pion_minus_tuple, "targ_type==2", "vz_elec-vz",100,-10,10, "V_{z e} - V_{z #pi} [cm]", "dN/dV_{z}", "pim_vz_diff_sol",
				output_location, output);

	//z vertex difference vs theta (e-pi)
	draw_plot_2D(pion_minus_tuple, "", "theta:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, 0, 2.5,"#theta", "pim_vz_diffxtheta", output_location, output);
	//z vertex difference vs theta in liq target (e-pi)
	draw_plot_2D(pion_minus_tuple, "targ_type==1", "theta:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, 0, 2.5,"#theta", "pim_vz_diffxtheta_liq", output_location, output);
	//z vertex difference vs theta in sol target (e-pi)
	draw_plot_2D(pion_minus_tuple, "targ_type==2", "theta:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, 0, 2.5,"#theta", "pim_vz_diffxtheta_sol", output_location, output);

	//z vertex difference vs phi (e-pi)
	draw_plot_2D(pion_minus_tuple, "", "phi:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, -180, 180,"#phi", "pim_vz_diffxphi", output_location, output);
	//z vertex difference vs phi in liq target (e-pi)
	draw_plot_2D(pion_minus_tuple, "targ_type==1", "phi:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, -180, 180,"#phi", "pim_vz_diffxphi_liq", output_location, output);
	//z vertex difference vs phi in sol target (e-pi)
	draw_plot_2D(pion_minus_tuple, "targ_type==2", "phi:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, -180, 180,"#phi", "pim_vz_diffxphi_sol", output_location, output);

	//z vertex difference vs p (e-pi)
	draw_plot_2D(pion_minus_tuple, "", "p:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, 0, 3,"p", "pim_vz_diffxp", output_location, output);
	//z vertex difference vs p e- in liq target (e-pi)
	draw_plot_2D(pion_minus_tuple, "targ_type==1", "p:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, 0, 3,"p", "pim_vz_diffxp_liq", output_location, output);
	//z vertex difference vs p e- in sol target (e-pi)
	draw_plot_2D(pion_minus_tuple, "targ_type==2", "p:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, 0, 3,"p", "pim_vz_diffxp_sol", output_location, output);

	//z vertex difference vs NDF (e-pi)
	draw_plot_2D(pion_minus_tuple, "", "NDF:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 8, 0.5, 8.5,"NDF", "pim_vz_diffxndf", output_location, output);
	//z vertex difference vs NDF in liq target (e-pi)
	draw_plot_2D(pion_minus_tuple, "targ_type==1", "NDF:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 8, 0.5, 8.5,"NDF", "pim_vz_diffxndf_liq", output_location, output);
	//z vertex difference vs NDF in sol target (e-pi)
	draw_plot_2D(pion_minus_tuple, "targ_type==2", "NDF:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 8, 0.5, 8.5,"NDF", "pim_vz_diffxndf_sol", output_location, output);

	//z vertex difference vs chi2/NDF (e-pi)
	draw_plot_2D(pion_minus_tuple, "", "(chi2/NDF):(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, 0, 50,"chi2/NDF", "pim_vz_diffxchi2ndf", output_location, output);
	//z vertex difference vs chi2/NDF in liq target (e-pi)
	draw_plot_2D(pion_minus_tuple, "targ_type==1", "(chi2/NDF):(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, 0, 50,"chi2/NDF", "pim_vz_diffxchi2ndf_liq", output_location, output);
	//z vertex difference vs chi2/NDF in sol target (e-pi)
	draw_plot_2D(pion_minus_tuple, "targ_type==2", "(chi2/NDF):(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, 0, 50,"chi2/NDF", "pim_vz_diffxchi2ndf_sol", output_location, output);

	//z vertex difference vs vt(e-pi)
	draw_plot_2D(pion_minus_tuple, "", "vt:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, 85, 105,"vt [ns]", "pim_vz_diffxvt", output_location, output);
	//z vertex difference vs vt in liq target (e-pi)
	draw_plot_2D(pion_minus_tuple, "targ_type==1", "vt:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, 85, 105,"vt [ns]", "pim_vz_diffxvt_liq", output_location, output);
	//z vertex difference vs vt in sol target (e-pi)
	draw_plot_2D(pion_minus_tuple, "targ_type==2", "vt:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, 85, 105,"vt [ns]", "pim_vz_diffxvt_sol", output_location, output);

	//z vertex difference vs vt difference (e-pi)
	draw_plot_2D(pion_minus_tuple, "", "(vt_elec-vt):(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, -0.5, 0.5,"V_{t e} - V_{t #pi} [ns]", "pim_vz_diffxvt_diff", output_location, output);
	//z vertex difference vs vt difference (e-pi)
	draw_plot_2D(hadron_tuple, "pid==-211", "(vt_elec-vt):(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, -0.5, 0.5,"V_{t e} - V_{t #pi} [ns]", "pim_nocut_vz_diffxvt_diff", output_location, output);
	//z vertex difference vs vt difference in liq target (e-pi)
	draw_plot_2D(pion_minus_tuple, "targ_type==1", "(vt_elec-vt):(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, -0.5, 0.5,"V_{t e} - V_{t #pi} [ns]", "pim_vz_diffxvt_diff_liq", output_location, output);
	//z vertex difference vs vt difference in sol target (e-pi)
	draw_plot_2D(pion_minus_tuple, "targ_type==2", "(vt_elec-vt):(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z #pi} [cm]",
					 100, -0.5, 0.5,"V_{t e} - V_{t #pi} [ns]", "pim_vz_diffxvt_diff_sol", output_location, output);

	//Beta vs P
	draw_plot_2D(pion_minus_tuple, "", "beta:p",100, 0, 3, "P",
					 100, 0, 2,"#beta", "pim_betaxP", output_location, output);

	//Beta vs P left peak
	draw_plot_2D(pion_minus_tuple, "(vz_elec-vz)<-3", "beta:p",100, 0, 3, "P",
					 100, 0, 2,"#beta", "pim_betaxP_left", output_location, output);

	//Beta vs P central peak
	draw_plot_2D(pion_minus_tuple, "(vz_elec-vz)>-3&&(vz_elec-vz)<3", "beta:p",100, 0, 3, "P",
					 100, 0, 2,"#beta", "pim_betaxP_center", output_location, output);

	//Beta vs P right peak
	draw_plot_2D(pion_minus_tuple, "(vz_elec-vz)>3", "beta:p",100, 0, 3, "P",
					 100, 0, 2,"#beta", "pim_betaxP_right", output_location, output);
*/
/*
	//-------------------
	//----POSITIVE HADRONS------
	//-------------------

	//x vertex
	draw_plot(hadron_tuple, "", "vx",100,-1,1, "V_{x} [cm]", "dN/dV_{z}", "h_vx",
				output_location, output);
	//y vertex
	draw_plot(hadron_tuple, "", "vy",100,-1,1, "V_{y} [cm]", "dN/dV_{y}", "h_vy",
				output_location, output);
	//z vertex
	draw_plot(hadron_tuple, "", "vz",100,-15,6, "V_{z} [cm]", "dN/dV_{z}", "h_vz",
				output_location, output);

	//time diff theo vs exp vertex
	draw_plot(hadron_tuple, "", "D_T",100,-5,5, "dt_{pi} [ns]", "dN/dDT", "h_dt",
				output_location, output);

	//time diff theo vs exp vertex
	draw_plot(hadron_tuple, "", "D_T",1000,-55,75, "dt_{pi} [ns]", "dN/dDT", "h_dt_wide",
				output_location, output);

	//time diff theo vs exp vertex
	draw_plot(hadron_tuple, "", "D_Te",1000,-75,50, "Dt_{e} [ns]", "dN/dDTe", "h_dte",
				output_location, output);

		//time diff theo vs exp vertex
	draw_plot(hadron_tuple, "", "D_Tp",1000,-55,75, "Dt_{p} [ns]", "dN/dDTp", "h_dtp",
				output_location, output);

	//z_h
	draw_plot(hadron_tuple, "", "z_h",180,0,1, "Z_{h}", "dN/dZ_{h}", "h_zh",
				output_location, output);

	//pt2
	draw_plot(hadron_tuple, "", "p_T2",100,0,5, "P_{T}^{2}", "dN/dP_{T}^{2}", "h_pt2",
				output_location, output);

	//phi
	draw_plot(hadron_tuple, "", "phi",360,-180,180, "#phi [deg]", "dN/d#phi", "h_phi",
				output_location, output);

	//phiPQ
	draw_plot(hadron_tuple, "", "phi_PQ",360,-3.15,3.15, "#phi_PQ [deg]", "dN/d#phi_PQ", "h_phi_PQ",
				output_location, output);

	//Pt2 vz Zh
	draw_plot_2D(hadron_tuple, "", "z_h:p_T2",100, 0, 5, "P_{T}^{2}",
					 100, 0, 1,"Z_{h}", "h_Pt2xZ", output_location, output);

	//P vz D_T
	draw_plot_2D(hadron_tuple, "", "D_T:p",100, 0, 10, "P",
					 100, -55, 75,"#DeltaT_{#pi}", "h_PxD_t", output_location, output);

	//P vz D_T
	draw_plot_2D(hadron_tuple, "", "D_Te:p",100, 0, 10, "P",
					 100, -75, 50,"#DeltaT_{e}", "h_PxD_te", output_location, output);
	//P vz D_T
	draw_plot_2D(hadron_tuple, "", "D_Tp:p",100, 0, 10, "P",
					 100, -55, 75,"#DeltaT_{p}", "h_PxD_tp", output_location, output);
	//EXTRAS

	//z vertex difference (e-pi)
	draw_plot(hadron_tuple, "", "vz_elec-vz",100,-10,10, "V_{z e} - V_{z h} [cm]", "dN/dV_{z}", "h_vz_diff",
				output_location, output);
	//z vertex difference in D2 target (e-pi)
	draw_plot(hadron_tuple, "targ_type==1", "vz_elec-vz",100,-10,10, "V_{z e} - V_{z h} [cm]", "dN/dV_{z}", "h_vz_diff_liq",
				output_location, output);
	//z vertex difference in sol target (e-pi)
	draw_plot(hadron_tuple, "targ_type==2", "vz_elec-vz",100,-10,10, "V_{z e} - V_{z h} [cm]", "dN/dV_{z}", "h_vz_diff_sol",
				output_location, output);

	//z vertex difference vs theta (e-pi)
	draw_plot_2D(hadron_tuple, "", "theta:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, 0, 2.5,"#theta", "h_vz_diffxtheta", output_location, output);
	//z vertex difference vs theta in liq target (e-pi)
	draw_plot_2D(hadron_tuple, "targ_type==1", "theta:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, 0, 2.5,"#theta", "h_vz_diffxtheta_liq", output_location, output);
	//z vertex difference vs theta in sol target (e-pi)
	draw_plot_2D(hadron_tuple, "targ_type==2", "theta:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, 0, 2.5,"#theta", "h_vz_diffxtheta_sol", output_location, output);

	//z vertex difference vs phi (e-pi)
	draw_plot_2D(hadron_tuple, "", "phi:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, -180, 180,"#phi", "h_vz_diffxphi", output_location, output);
	//z vertex difference vs phi in liq target (e-pi)
	draw_plot_2D(hadron_tuple, "targ_type==1", "phi:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, -180, 180,"#phi", "h_vz_diffxphi_liq", output_location, output);
	//z vertex difference vs phi in sol target (e-pi)
	draw_plot_2D(hadron_tuple, "targ_type==2", "phi:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, -180, 180,"#phi", "h_vz_diffxphi_sol", output_location, output);

	//z vertex difference vs p (e-pi)
	draw_plot_2D(hadron_tuple, "", "p:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, 0, 3,"p", "h_vz_diffxp", output_location, output);
	//z vertex difference vs p e- in liq target (e-pi)
	draw_plot_2D(hadron_tuple, "targ_type==1", "p:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, 0, 3,"p", "h_vz_diffxp_liq", output_location, output);
	//z vertex difference vs p e- in sol target (e-pi)
	draw_plot_2D(hadron_tuple, "targ_type==2", "p:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, 0, 3,"p", "h_vz_diffxp_sol", output_location, output);

	//z vertex difference vs NDF (e-pi)
	draw_plot_2D(hadron_tuple, "", "NDF:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 8, 0.5, 8.5,"NDF", "h_vz_diffxndf", output_location, output);
	//z vertex difference vs NDF in liq target (e-pi)
	draw_plot_2D(hadron_tuple, "targ_type==1", "NDF:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 8, 0.5, 8.5,"NDF", "h_vz_diffxndf_liq", output_location, output);
	//z vertex difference vs NDF in sol target (e-pi)
	draw_plot_2D(hadron_tuple, "targ_type==2", "NDF:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 8, 0.5, 8.5,"NDF", "h_vz_diffxndf_sol", output_location, output);

	//z vertex difference vs chi2/NDF (e-pi)
	draw_plot_2D(hadron_tuple, "", "(chi2/NDF):(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, 0, 50,"chi2/NDF", "h_vz_diffxchi2ndf", output_location, output);
	//z vertex difference vs chi2/NDF in liq target (e-pi)
	draw_plot_2D(hadron_tuple, "targ_type==1", "(chi2/NDF):(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, 0, 50,"chi2/NDF", "h_vz_diffxchi2ndf_liq", output_location, output);
	//z vertex difference vs chi2/NDF in sol target (e-pi)
	draw_plot_2D(hadron_tuple, "targ_type==2", "(chi2/NDF):(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, 0, 50,"chi2/NDF", "h_vz_diffxchi2ndf_sol", output_location, output);

	//z vertex difference vs vt(e-pi)
	draw_plot_2D(hadron_tuple, "", "vt:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, 85, 105,"vt [ns]", "h_vz_diffxvt", output_location, output);
	//z vertex difference vs vt in liq target (e-pi)
	draw_plot_2D(hadron_tuple, "targ_type==1", "vt:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, 85, 105,"vt [ns]", "h_vz_diffxvt_liq", output_location, output);
	//z vertex difference vs vt in sol target (e-pi)
	draw_plot_2D(hadron_tuple, "targ_type==2", "vt:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, 85, 105,"vt [ns]", "h_vz_diffxvt_sol", output_location, output);

	//z vertex difference vs vt difference (e-pi)
	draw_plot_2D(hadron_tuple, "", "(vt_elec-vt):(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, -0.5, 0.5,"V_{t e} - V_{t h} [ns]", "h_vz_diffxvt_diff", output_location, output);
	//z vertex difference vs vt difference in liq target (e-pi)
	draw_plot_2D(hadron_tuple, "targ_type==1", "(vt_elec-vt):(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, -0.5, 0.5,"V_{t e} - V_{t h} [ns]", "h_vz_diffxvt_diff_liq", output_location, output);
	//z vertex difference vs vt difference in sol target (e-pi)
	draw_plot_2D(hadron_tuple, "targ_type==2", "(vt_elec-vt):(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, -0.5, 0.5,"V_{t e} - V_{t h} [ns]", "h_vz_diffxvt_diff_sol", output_location, output);

	//Beta vs P
	draw_plot_2D(hadron_tuple, "", "beta:p",100, 0, 3, "P",
					 100, 0, 1.1,"#beta", "h_betaxP", output_location, output);

	//Beta vs P left peak
	draw_plot_2D(hadron_tuple, "(vz_elec-vz)<-3", "beta:p",100, 0, 3, "P",
					 100, 0, 1.1,"#beta", "h_betaxP_left", output_location, output);

	//Beta vs P central peak
	draw_plot_2D(hadron_tuple, "(vz_elec-vz)>-3&&(vz_elec-vz)<3", "beta:p",100, 0, 3, "P",
					 100, 0, 1.1,"#beta", "h_betaxP_center", output_location, output);

	//Beta vs P right peak
	draw_plot_2D(hadron_tuple, "(vz_elec-vz)>3", "beta:p",100, 0, 3, "P",
					 100, 0, 1.1,"#beta", "h_betaxP_right", output_location, output);

	//z vertex difference (e-pi) with cuts
	draw_plot(hadron_tuple, "D_T<0.5&&D_T>-0.5", "vz_elec-vz",100,-10,10, "V_{z e} - V_{z h} [cm]", "dN/dV_{z}", "h_vz_diff_DT_cut",
				output_location, output);

	//-------------------
	//----PROTONS------
	//-------------------

	//x vertex
	draw_plot(proton_tuple, "", "vx",100,-1,1, "V_{x} [cm]", "dN/dV_{z}", "p_vx",
				output_location, output);
	//y vertex
	draw_plot(proton_tuple, "", "vy",100,-1,1, "V_{y} [cm]", "dN/dV_{y}", "p_vy",
				output_location, output);
	//z vertex
	draw_plot(proton_tuple, "", "vz",100,-15,6, "V_{z} [cm]", "dN/dV_{z}", "p_vz",
				output_location, output);

	//time diff theo vs exp vertex
	draw_plot(proton_tuple, "", "D_T",100,-15,15, "dt_{pi} [ns]", "dN/dDT", "p_dt",
				output_location, output);

	//time diff theo vs exp vertex
	draw_plot(proton_tuple, "", "D_Te",100,-30,-15, "dt_{pi} [ns]", "dN/dDT", "p_dte",
				output_location, output);

	//time diff theo vs exp vertex
	draw_plot(proton_tuple, "", "D_Tp",100,-15,15, "dt_{pi} [ns]", "dN/dDT", "p_dtp",
				output_location, output);

	//z_h
	draw_plot(proton_tuple, "", "z_h",180,0,1, "Z_{h}", "dN/dZ_{h}", "p_zh",
				output_location, output);

	//pt2
	draw_plot(proton_tuple, "", "p_T2",100,0,5, "P_{T}^{2}", "dN/dP_{T}^{2}", "p_pt2",
				output_location, output);

	//phi
	draw_plot(proton_tuple, "", "phi",360,-180,180, "#phi [deg]", "dN/d#phi", "p_phi",
				output_location, output);

	//phiPQ
	draw_plot(proton_tuple, "", "phi_PQ",360,-3.15,3.15, "#phi_PQ [deg]", "dN/d#phi_PQ", "p_phi_PQ",
				output_location, output);

	//Pt2 vz Zh
	draw_plot_2D(proton_tuple, "", "z_h:p_T2",100, 0, 5, "P_{T}^{2}",
					 100, 0, 1,"Z_{h}", "p_Pt2xZ", output_location, output);

	//P vz D_T
	draw_plot_2D(proton_tuple, "", "D_T:p",100, 0, 5, "P",
					 100, -15, 15,"#DeltaT_{h}", "p_PxD_t", output_location, output);

	//P vz D_T
	draw_plot_2D(proton_tuple, "", "D_Te:p",100, 0, 5, "P",
					 100, -30, -15,"#DeltaT_{h}", "p_PxD_te", output_location, output);

	//P vz D_T
	draw_plot_2D(proton_tuple, "", "D_Tp:p",100, 0, 5, "P",
					 100, -15, 15,"#DeltaT_{h}", "p_PxD_tp", output_location, output);

	//EXTRAS

	//z vertex difference (e-pi)
	draw_plot(proton_tuple, "", "vz_elec-vz",100,-10,10, "V_{z e} - V_{z h} [cm]", "dN/dV_{z}", "p_vz_diff",
				output_location, output);
	//z vertex difference in D2 target (e-pi)
	draw_plot(proton_tuple, "targ_type==1", "vz_elec-vz",100,-10,10, "V_{z e} - V_{z h} [cm]", "dN/dV_{z}", "p_vz_diff_liq",
				output_location, output);
	//z vertex difference in sol target (e-pi)
	draw_plot(proton_tuple, "targ_type==2", "vz_elec-vz",100,-10,10, "V_{z e} - V_{z h} [cm]", "dN/dV_{z}", "p_vz_diff_sol",
				output_location, output);

	//z vertex difference vs theta (e-pi)
	draw_plot_2D(proton_tuple, "", "theta:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, 0, 2.5,"#theta", "p_vz_diffxtheta", output_location, output);
	//z vertex difference vs theta in liq target (e-pi)
	draw_plot_2D(proton_tuple, "targ_type==1", "theta:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, 0, 2.5,"#theta", "p_vz_diffxtheta_liq", output_location, output);
	//z vertex difference vs theta in sol target (e-pi)
	draw_plot_2D(proton_tuple, "targ_type==2", "theta:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, 0, 2.5,"#theta", "p_vz_diffxtheta_sol", output_location, output);

	//z vertex difference vs phi (e-pi)
	draw_plot_2D(proton_tuple, "", "phi:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, -180, 180,"#phi", "p_vz_diffxphi", output_location, output);
	//z vertex difference vs phi in liq target (e-pi)
	draw_plot_2D(proton_tuple, "targ_type==1", "phi:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, -180, 180,"#phi", "p_vz_diffxphi_liq", output_location, output);
	//z vertex difference vs phi in sol target (e-pi)
	draw_plot_2D(proton_tuple, "targ_type==2", "phi:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, -180, 180,"#phi", "p_vz_diffxphi_sol", output_location, output);

	//z vertex difference vs p (e-pi)
	draw_plot_2D(proton_tuple, "", "p:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, 0, 3,"p", "p_vz_diffxp", output_location, output);
	//z vertex difference vs p e- in liq target (e-pi)
	draw_plot_2D(proton_tuple, "targ_type==1", "p:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, 0, 3,"p", "p_vz_diffxp_liq", output_location, output);
	//z vertex difference vs p e- in sol target (e-pi)
	draw_plot_2D(proton_tuple, "targ_type==2", "p:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, 0, 3,"p", "p_vz_diffxp_sol", output_location, output);

	//z vertex difference vs NDF (e-pi)
	draw_plot_2D(proton_tuple, "", "NDF:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 8, 0.5, 8.5,"NDF", "p_vz_diffxndf", output_location, output);
	//z vertex difference vs NDF in liq target (e-pi)
	draw_plot_2D(proton_tuple, "targ_type==1", "NDF:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 8, 0.5, 8.5,"NDF", "p_vz_diffxndf_liq", output_location, output);
	//z vertex difference vs NDF in sol target (e-pi)
	draw_plot_2D(proton_tuple, "targ_type==2", "NDF:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 8, 0.5, 8.5,"NDF", "p_vz_diffxndf_sol", output_location, output);

	//z vertex difference vs chi2/NDF (e-pi)
	draw_plot_2D(proton_tuple, "", "(chi2/NDF):(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, 0, 50,"chi2/NDF", "p_vz_diffxchi2ndf", output_location, output);
	//z vertex difference vs chi2/NDF in liq target (e-pi)
	draw_plot_2D(proton_tuple, "targ_type==1", "(chi2/NDF):(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, 0, 50,"chi2/NDF", "p_vz_diffxchi2ndf_liq", output_location, output);
	//z vertex difference vs chi2/NDF in sol target (e-pi)
	draw_plot_2D(proton_tuple, "targ_type==2", "(chi2/NDF):(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, 0, 50,"chi2/NDF", "p_vz_diffxchi2ndf_sol", output_location, output);

	//z vertex difference vs vt(e-pi)
	draw_plot_2D(proton_tuple, "", "vt:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, 85, 105,"vt [ns]", "p_vz_diffxvt", output_location, output);
	//z vertex difference vs vt in liq target (e-pi)
	draw_plot_2D(proton_tuple, "targ_type==1", "vt:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, 85, 105,"vt [ns]", "p_vz_diffxvt_liq", output_location, output);
	//z vertex difference vs vt in sol target (e-pi)
	draw_plot_2D(proton_tuple, "targ_type==2", "vt:(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, 85, 105,"vt [ns]", "p_vz_diffxvt_sol", output_location, output);

	//z vertex difference vs vt difference (e-pi)
	draw_plot_2D(proton_tuple, "", "(vt_elec-vt):(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, -0.5, 0.5,"V_{t e} - V_{t h} [ns]", "p_vz_diffxvt_diff", output_location, output);
	//z vertex difference vs vt difference in liq target (e-pi)
	draw_plot_2D(proton_tuple, "targ_type==1", "(vt_elec-vt):(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, -0.5, 0.5,"V_{t e} - V_{t h} [ns]", "p_vz_diffxvt_diff_liq", output_location, output);
	//z vertex difference vs vt difference in sol target (e-pi)
	draw_plot_2D(proton_tuple, "targ_type==2", "(vt_elec-vt):(vz_elec-vz)",100, -10, 10, "V_{z e} - V_{z h} [cm]",
					 100, -0.5, 0.5,"V_{t e} - V_{t h} [ns]", "p_vz_diffxvt_diff_sol", output_location, output);

	//Beta vs P
	draw_plot_2D(proton_tuple, "", "beta:p",100, 0, 3, "P",
					 100, 0, 1.1,"#beta", "p_betaxP", output_location, output);

	//Beta vs P left peak
	draw_plot_2D(proton_tuple, "(vz_elec-vz)<-3", "beta:p",100, 0, 3, "P",
					 100, 0, 1.1,"#beta", "p_betaxP_left", output_location, output);

	//Beta vs P central peak
	draw_plot_2D(proton_tuple, "(vz_elec-vz)>-3&&(vz_elec-vz)<3", "beta:p",100, 0, 3, "P",
					 100, 0, 1.1,"#beta", "p_betaxP_center", output_location, output);

	//Beta vs P right peak
	draw_plot_2D(proton_tuple, "(vz_elec-vz)>3", "beta:p",100, 0, 3, "P",
					 100, 0, 1.1,"#beta", "p_betaxP_right", output_location, output);

	//z vertex difference (e-pi) with cuts
	draw_plot(proton_tuple, "D_Tp<0.5&&D_Tp>-0.5", "vz_elec-vz",100,-10,10, "V_{z e} - V_{z h} [cm]", "dN/dV_{z}", "p_vz_diff_DT_cut",
				output_location, output);
*/
	//Plots for MR
	//pi+
	//d2
	draw_plot(pion_tuple, "targ_type==1", "z_h",10,Zh_bins[0],Zh_bins[N_Zh], "Z_{h}", "dN/dZ_{h}",
				"pion_z_h_d2", output_location, output);
	draw_plot(pion_tuple, "targ_type==1", "p_T2",10,Pt2_bins[0],Pt2_bins[N_Pt2], "P_{T}^{2}", "dN/dP_{T}^{2}",
				"pion_p_T2_d2", output_location, output);
	draw_plot(pion_tuple, "targ_type==1", "Q2",10,Q2_bins[0],Q2_bins[N_Q2], "Q^{2}", "dN/dQ^{2}" ,
				"pion_Q2_d2", output_location, output);
	draw_plot(pion_tuple, "targ_type==1", "nu",10,Nu_bins[0],Nu_bins[N_Nu], "#nu", "dN/d#nu" ,
				"pion_nu_d2", output_location, output);
	draw_plot(pion_tuple, "targ_type==1", "phi_PQ",10,Phi_bins[0],Phi_bins[N_Phi], "#phi_PQ", "dN/d#phi_PQ" ,
				"pion_phi_PQ_d2", output_location, output);
	//solid
	draw_plot(pion_tuple, "targ_type==2", "z_h",10,Zh_bins[0],Zh_bins[N_Zh], "Z_{h}", "dN/dZ_{h}",
				"pion_z_h_solid", output_location, output);
	draw_plot(pion_tuple, "targ_type==2", "p_T2",10,Pt2_bins[0],Pt2_bins[N_Pt2], "P_{T}^{2}", "dN/dP_{T}^{2}",
				"pion_p_T2_solid", output_location, output);
	draw_plot(pion_tuple, "targ_type==2", "Q2",10,Q2_bins[0],Q2_bins[N_Q2], "Q^{2}", "dN/dQ^{2}" ,
				"pion_Q2_solid", output_location, output);
	draw_plot(pion_tuple, "targ_type==2", "nu",10,Nu_bins[0],Nu_bins[N_Nu], "#nu", "dN/d#nu" ,
				"pion_nu_solid", output_location, output);
	draw_plot(pion_tuple, "targ_type==2", "phi_PQ",10,Phi_bins[0],Phi_bins[N_Phi], "#phi_PQ", "dN/d#phi_PQ" ,
				"pion_phi_PQ_solid", output_location, output);

	//pi-
	//d2
	draw_plot(pion_minus_tuple, "targ_type==1", "z_h",10,Zh_bins[0],Zh_bins[N_Zh], "Z_{h}", "dN/dZ_{h}",
				"pion_minus_z_h_d2", output_location, output);
	draw_plot(pion_minus_tuple, "targ_type==1", "p_T2",10,Pt2_bins[0],Pt2_bins[N_Pt2], "P_{T}^{2}", "dN/dP_{T}^{2}",
				"pion_minus_p_T2_d2", output_location, output);
	draw_plot(pion_minus_tuple, "targ_type==1", "Q2",10,Q2_bins[0],Q2_bins[N_Q2], "Q^{2}", "dN/dQ^{2}" ,
				"pion_minus_Q2_d2", output_location, output);
	draw_plot(pion_minus_tuple, "targ_type==1", "nu",10,Nu_bins[0],Nu_bins[N_Nu], "#nu", "dN/d#nu" ,
				"pion_minus_nu_d2", output_location, output);
	draw_plot(pion_minus_tuple, "targ_type==1", "phi_PQ",10,Phi_bins[0],Phi_bins[N_Phi], "#phi_PQ", "dN/d#phi_PQ" ,
				"pion_minus_phi_PQ_d2", output_location, output);
	//solid
	draw_plot(pion_minus_tuple, "targ_type==2", "z_h",10,Zh_bins[0],Zh_bins[N_Zh], "Z_{h}", "dN/dZ_{h}",
				"pion_minus_z_h_solid", output_location, output);
	draw_plot(pion_minus_tuple, "targ_type==2", "p_T2",10,Pt2_bins[0],Pt2_bins[N_Pt2], "P_{T}^{2}", "dN/dP_{T}^{2}",
				"pion_minus_p_T2_solid", output_location, output);
	draw_plot(pion_minus_tuple, "targ_type==2", "Q2",10,Q2_bins[0],Q2_bins[N_Q2], "Q^{2}", "dN/dQ^{2}" ,
				"pion_minus_Q2_solid", output_location, output);
	draw_plot(pion_minus_tuple, "targ_type==2", "nu",10,Nu_bins[0],Nu_bins[N_Nu], "#nu", "dN/d#nu" ,
				"pion_minus_nu_solid", output_location, output);
	draw_plot(pion_minus_tuple, "targ_type==2", "phi_PQ",10,Phi_bins[0],Phi_bins[N_Phi], "#phi_PQ", "dN/d#phi_PQ" ,
				"pion_minus_phi_PQ_solid", output_location, output);

	//elec
	//d2
	draw_plot(elec_tuple, "targ_type==1", "Q2",10,Q2_bins[0],Q2_bins[N_Q2], "Q^{2}", "dN/dQ^{2}" ,
				"e_Q2_d2", output_location, output);
	draw_plot(elec_tuple, "targ_type==1", "nu",10,Nu_bins[0],Nu_bins[N_Nu], "#nu", "dN/d#nu" ,
				"e_nu_d2", output_location, output);
	//solid
	draw_plot(elec_tuple, "targ_type==2", "Q2",10,Q2_bins[0],Q2_bins[N_Q2], "Q^{2}", "dN/dQ^{2}" ,
				"e_Q2_solid", output_location, output);
	draw_plot(elec_tuple, "targ_type==2", "nu",10,Nu_bins[0],Nu_bins[N_Nu], "#nu", "dN/d#nu" ,
				"e_nu_solid", output_location, output);

	cout<<"Finished plotting. Cleaning "<<endl;
	delete pion_tuple;
	delete hadron_tuple;
	delete pion_minus_tuple;
	delete proton_tuple;
	delete elec_tuple;
	delete output;
}

//Main function that recieves a txt with a list of run number asn the name of the output file
//void simple_plots(const char* inputFileName, TString Target, TString type="data"){
//	ROOT::EnableImplicitMT();
//	cout<<"Running macro based on file name"<<endl;
//
//	if 		(Target=="C"){tar_n=0;}
//    else if (Target=="Al"){tar_n=1;}
//    else if (Target=="Cu"){tar_n=2;}
//    else if (Target=="Sn"){tar_n=3;}
//    else if (Target=="Pb"){tar_n=4;}
//
//	// Open the input text file
//    std::ifstream inputFile(inputFileName);
//
//    // Create a TChain to combine input TNuples
//    TChain* input_tuple = new TChain("DT");
//    TChain* input_tuple_mc = new TChain("MC");
//
//    // Read each line from the text file and add the corresponding ROOT file to the TChain
//    char buffer [10];
//    std::string line;
//    while (std::getline(inputFile, line)) {
//    	int run_N = stoi(line);
//    	sprintf(buffer,"%0*d", 6, run_N);
//		TString run_N_str=TString(buffer);
//        if (type=="data"){
//        	cout<<"Adding run: "<<line<<endl;
//        	input_tuple->Add(type+"/ntuples_"+fwd_rec+"_"+run_N_str+".root");
//        }
//        if (type=="simul"){
//        	cout<<"Addind simul job: "<<line<<endl;
//        	input_tuple->Add(type+"/"+Target+"/ntuples_"+fwd_rec+"_"+run_N_str+".root");
//        	input_tuple_mc->Add(type+"/"+Target+"/ntuples_"+fwd_rec+"_"+run_N_str+".root");
//        }
//    }
//
//    // Close the input file
//    inputFile.close();
//
//    //process the Tchain to make plots and output tuples
//    TString output_location = "output_"+fwd_rec+"/"+type+"/"+Target+"/";
//    gSystem->Exec("mkdir -vp "+output_location);
//    cout<<"Output location: "<<output_location<<endl;
//
//    processChain(input_tuple, output_location);
//    cout<<"Data tuple done"<<endl;
//
//	if (type=="simul") {
//		output_location=output_location+"thrown/";
//		processChain(input_tuple_mc, output_location);
//		cout<<"MC tuple done"<<endl;
//	}
//
//    //delete all objects
//    delete input_tuple;
//    delete input_tuple_mc;
//}

//Main function that recieves a run number as the input
void simple_plots(int run_N=000000, TString Target="unkw", TString type="data"){
	ROOT::EnableImplicitMT();

	cout<<"Running macro based on run number"<<endl;

	if 		(Target=="C"){tar_n=1;}
    else if (Target=="Al"){tar_n=2;}
    else if (Target=="Cu"){tar_n=3;}
    else if (Target=="Sn"){tar_n=4;}
    else if (Target=="Pb"){tar_n=5;}

	//Transform input run number to Tstring with correct number of digits
	TString run_N_str = TString::Format("%06d", run_N);;

	//Get input file
	TFile *input_file;
	TNtuple *input_tuple, *input_tuple_mc;

	if (type=="data"){
    	if 		(run_location=="own_pc"){input_file = new TFile(type+"/ntuples_"+fwd_rec+"_"+run_N_str+".root","READ");}
    	else if (run_location=="farm")  {input_file = new TFile("/volatile/clas12/antorad/rge/data/pass1/"+torus_pol+"/"+Target+"_D2/"+fwd_rec+"/ntuple_files/ntuples_"+fwd_rec+"_"+run_N_str+".root", "READ");}
    	input_tuple = (TNtuple*)input_file->Get("DT");
 	}
    if (type=="simul"){
    	if 		(run_location=="own_pc"){input_file = new TFile(type+"/"+Target+"/ntuples_"+fwd_rec+"_"+run_N_str+".root","READ");}
    	else if (run_location=="farm")  {input_file = new TFile("/volatile/clas12/antorad/rge/data/pass1/"+torus_pol+"/"+Target+"_D2/"+fwd_rec+"/ntuple_files/ntuples_"+fwd_rec+"_"+run_N_str+".root", "READ");}
    	input_tuple = (TNtuple*)input_file->Get("DT");
    	input_tuple_mc = (TNtuple*)input_file->Get("MC");
    }

	//Output directory
	TString output_location;
	if 		(run_location=="own_pc"){output_location = "output_"+fwd_rec+"/"+torus_pol+"/"+type+"/"+detector+"/"+Target+"/"+run_N_str+"/";}
	else if (run_location=="farm")  {output_location = "/volatile/clas12/antorad/rge/MR_analysis/"+type+"/pass1/"+torus_pol+"/"+fwd_rec+"/"+detector+"/"+Target+"/"+run_N_str+"/";}
	gSystem->Exec("mkdir -vp "+output_location);
	cout<<"Output location: "<<output_location<<endl;

    //process the TNtuple to make plots and output tuples
	processTuple(input_tuple, output_location);
	cout<<"Data tuple done"<<endl;

	if (type=="simul") {
		output_location=output_location+"thrown/";
		processTuple(input_tuple_mc, output_location);
		cout<<"MC tuple done"<<endl;
	}

	//delete all objects
	delete input_tuple;
	delete input_tuple_mc;
}