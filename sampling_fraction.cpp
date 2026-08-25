#include "include.h"

using namespace std;

void processChain(TChain* input_tuple, TString output_location){
    ROOT::EnableImplicitMT();

    //Output root file
    TFile *output = new TFile(output_location+"/sampling_fraction.root","RECREATE");

    //Make histograms for SF studies
    float sf_up_lim[6][4];
    float sf_lo_lim[6][4];

    for (int i=0; i<6; i++){
        for (int j=0; j<4; j++)
        {
            sf_up_lim[i][j] = mu_sf[i][j]+3.5*sigma_sf[i][j];
            sf_lo_lim[i][j] = mu_sf[i][j]-3.5*sigma_sf[i][j];
        }
    }

    //CUTS for elecrons previous to sampling fraction
    TCut pid_cut = "pid==11";
    TCut P_cut = "p>2&&p<8";
    TCut theta_cut = "theta*57.2958>5";
    TCut PCal_cut = "PCAL_V>14&&PCAL_W>14";
    TCut DC_cut = "DC_R1_edge>4.5&&DC_R2_edge>3.5&&DC_R2_edge>7.5";
    TCut partial_sf_cut = "(p<4.5)||(p>4.5&&E_PCAL/p>(-0.22/0.15)*E_ECIN/p+0.22)";
    TCut sf_cut_1u = Form("sector == 1 && E_total/p < %f + %f*E_total + %f*pow(E_total,2) + %f*pow(E_total,3)",sf_up_lim[0][0],sf_up_lim[0][1],sf_up_lim[0][2],sf_up_lim[0][3]);
    TCut sf_cut_1d = Form("sector == 1 && E_total/p > %f + %f*E_total + %f*pow(E_total,2) + %f*pow(E_total,3)",sf_lo_lim[0][0],sf_lo_lim[0][1],sf_lo_lim[0][2],sf_lo_lim[0][3]);
    TCut sf_cut_2u = Form("sector == 2 && E_total/p < %f + %f*E_total + %f*pow(E_total,2) + %f*pow(E_total,3)",sf_up_lim[1][0],sf_up_lim[1][1],sf_up_lim[1][2],sf_up_lim[1][3]);
    TCut sf_cut_2d = Form("sector == 2 && E_total/p > %f + %f*E_total + %f*pow(E_total,2) + %f*pow(E_total,3)",sf_lo_lim[1][0],sf_lo_lim[1][1],sf_lo_lim[1][2],sf_lo_lim[1][3]);
    TCut sf_cut_3u = Form("sector == 3 && E_total/p < %f + %f*E_total + %f*pow(E_total,2) + %f*pow(E_total,3)",sf_up_lim[2][0],sf_up_lim[2][1],sf_up_lim[2][2],sf_up_lim[2][3]);
    TCut sf_cut_3d = Form("sector == 3 && E_total/p > %f + %f*E_total + %f*pow(E_total,2) + %f*pow(E_total,3)",sf_lo_lim[2][0],sf_lo_lim[2][1],sf_lo_lim[2][2],sf_lo_lim[2][3]);
    TCut sf_cut_4u = Form("sector == 4 && E_total/p < %f + %f*E_total + %f*pow(E_total,2) + %f*pow(E_total,3)",sf_up_lim[3][0],sf_up_lim[3][1],sf_up_lim[3][2],sf_up_lim[3][3]);
    TCut sf_cut_4d = Form("sector == 4 && E_total/p > %f + %f*E_total + %f*pow(E_total,2) + %f*pow(E_total,3)",sf_lo_lim[3][0],sf_lo_lim[3][1],sf_lo_lim[3][2],sf_lo_lim[3][3]);
    TCut sf_cut_5u = Form("sector == 5 && E_total/p < %f + %f*E_total + %f*pow(E_total,2) + %f*pow(E_total,3)",sf_up_lim[4][0],sf_up_lim[4][1],sf_up_lim[4][2],sf_up_lim[4][3]);
    TCut sf_cut_5d = Form("sector == 5 && E_total/p > %f + %f*E_total + %f*pow(E_total,2) + %f*pow(E_total,3)",sf_lo_lim[4][0],sf_lo_lim[4][1],sf_lo_lim[4][2],sf_lo_lim[4][3]);
    TCut sf_cut_6u = Form("sector == 6 && E_total/p < %f + %f*E_total + %f*pow(E_total,2) + %f*pow(E_total,3)",sf_up_lim[5][0],sf_up_lim[5][1],sf_up_lim[5][2],sf_up_lim[5][3]);
    TCut sf_cut_6d = Form("sector == 6 && E_total/p > %f + %f*E_total + %f*pow(E_total,2) + %f*pow(E_total,3)",sf_lo_lim[5][0],sf_lo_lim[5][1],sf_lo_lim[5][2],sf_lo_lim[5][3]);
    TCut sf_cut = (sf_cut_1u&&sf_cut_1d)||(sf_cut_2u&&sf_cut_2d)||(sf_cut_3u&&sf_cut_3d)||(sf_cut_4u&&sf_cut_4d)||(sf_cut_5u&&sf_cut_5d)||(sf_cut_6u&&sf_cut_6d);
    TCut total_cut = pid_cut&&P_cut&&theta_cut&&partial_sf_cut&&sf_cut; //Add Pcal and DC later

    output->cd();
    //Sampling fraction TH2F
    cout<<input_tuple->GetEntries()<<endl;
    input_tuple->Draw("E_total/p:E_total>>h2_sf_nocut(100,0,2,100,0.150,0.350)",pid_cut&&P_cut,"COLZ");
    TH2F *h2_sf_nocut = (TH2F*)gDirectory->GetList()->FindObject("h2_sf_nocut");

    input_tuple->Draw("E_PCAL/p:E_ECIN/p>>h2_psf_nocut(100,0,0.2,100,0,0.25)",pid_cut&&P_cut,"COLZ");
    TH2F *h2_psf_nocut = (TH2F*)gDirectory->GetList()->FindObject("h2_psf_nocut");

    input_tuple->Draw("E_PCAL/p:E_ECIN/p>>h2_psf(100,0,0.2,100,0,0.25)",pid_cut&&P_cut&&partial_sf_cut,"COLZ");
    TH2F *h2_psf = (TH2F*)gDirectory->GetList()->FindObject("h2_psf");

    input_tuple->Draw("E_total/p:E_total>>h2_sf(100,0,2,100,0.150,0.350)",total_cut,"COLZ");
    TH2F *h2_sf = (TH2F*)gDirectory->GetList()->FindObject("h2_sf");

    TF1 *s3_up = new TF1("s3_up",Form("%f + %f*x + %f*pow(x,2) + %f*pow(x,3)",sf_up_lim[2][0],sf_up_lim[2][1],sf_up_lim[2][2],sf_up_lim[2][3]) ,0,2);
    s3_up->Write("s3_up");
    TF1 *s3_lo = new TF1("s3_lo",Form("%f + %f*x + %f*pow(x,2) + %f*pow(x,3)",sf_lo_lim[2][0],sf_lo_lim[2][1],sf_lo_lim[2][2],sf_lo_lim[2][3]) ,0,2);
    s3_lo->Write("s3_lo");

    h2_sf_nocut->Write("h2_sf_nocut");
    h2_psf_nocut->Write("h2_psf_nocut");
    h2_psf->Write("h2_psf");
    h2_sf->Write("h2_sf");

    output->Close();
}

//Main function that recieves a txt with a list of run number asn the name of the output file
void sampling_fraction(const char* inputFileName, TString Target, TString type="data"){
    ROOT::EnableImplicitMT();

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
            input_tuple->Add(type+"/ntuples_"+fwd_rec+"_"+run_N_str+".root");
        }
        if (type=="simul"){
            cout<<"Addind simul job: "<<line<<endl;
            input_tuple->Add(type+"/"+Target+"/ntuples_"+fwd_rec+"_"+run_N_str+".root");
            input_tuple_mc->Add(type+"/"+Target+"/ntuples_"+fwd_rec+"_"+run_N_str+".root");
        }
    }

    // Close the input file
    inputFile.close();

    //process the Tchain to make plots and output tuples
    TString output_location = "output_"+fwd_rec+"/"+type+"/"+Target+"/";
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
void sampling_fraction(int run_N=000000, TString Target="unkw", TString type="data"){
    ROOT::EnableImplicitMT();

    // Create a TChain to load input TNuples
    TChain* input_tuple = new TChain("data");
    TChain* input_tuple_mc = new TChain("MC");

    //Transform input run number to Tstring with correct number of digits
    TString run_N_str = TString::Format("%06d", run_N);

    //Output directory
    TString output_location;
    output_location = "output_"+fwd_rec+"/"+type+"/"+Target+"/"+run_N_str+"/";
    cout<<"Output location: "<<output_location<<endl;

    if (type=="data"){
        input_tuple->Add(type+"/ntuples_"+fwd_rec+"_"+run_N_str+".root");
    }
    if (type=="simul"){
        input_tuple->Add(type+"/"+Target+"/ntuples_"+fwd_rec+"_"+run_N_str+".root");
        input_tuple_mc->Add(type+"/"+Target+"/ntuples_"+fwd_rec+"_"+run_N_str+".root");
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