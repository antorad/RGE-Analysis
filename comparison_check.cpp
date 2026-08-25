#include "include.h"

void comparison_check(){

    //output root file
    //TFile *output = new TFile("comparison_check.root","UPDATE");

    //Get TNtuple from make_ntuples
    TFile *input = new TFile("data/ntuples_"+fwd_rec+"_000000.root","READ");
    TNtuple* input_tuple = (TNtuple*)input->Get("data");

    Float_t pid, v_z, Q2, E_PCAL, p, P_sf, status, DC_R1_edge;
    input_tuple->SetBranchAddress("pid",&pid);
    input_tuple->SetBranchAddress("Q2",&Q2);
    input_tuple->SetBranchAddress("v_z",&v_z);
    input_tuple->SetBranchAddress("E_PCAL",&E_PCAL);
    input_tuple->SetBranchAddress("p",&p);
    input_tuple->SetBranchAddress("status",&status);
    input_tuple->SetBranchAddress("DC_R1_edge",&DC_R1_edge);

    Long64_t n_entries = input_tuple->GetEntries();

    //METHOD 1
    // vz bins
    int vz_bin[5]={0};
    int Q2_bin[5]={0};
    int P_sf_bin[2]={0};

    for (Long64_t i=0;i<n_entries;i++) {
        input_tuple->GetEntry(i);
        P_sf=E_PCAL/p;
        if (pid==11&&status<0){
            if      (v_z<-9){vz_bin[0]++;}
            else if (v_z>-9&&v_z<-4.5){vz_bin[1]++;}
            else if (v_z>-4.5&&v_z<-3){vz_bin[2]++;}
            else if (v_z>-3&&v_z<0){vz_bin[3]++;}
            else if (v_z>0){vz_bin[4]++;}

            if      (Q2>0&&Q2<2){Q2_bin[0]++;}
            else if (Q2>2&&Q2<4){Q2_bin[1]++;}
            else if (Q2>4&&Q2<6){Q2_bin[2]++;}
            else if (Q2>6&&Q2<8){Q2_bin[3]++;}
            else if (Q2>8&&Q2<10){Q2_bin[4]++;}

            if(DC_R1_edge>3){
               if      (P_sf>0&&P_sf<0.15){P_sf_bin[0]++;}
               else if (P_sf>0.15&&P_sf<0.3){P_sf_bin[1]++;}

            }
        }
    }
    cout<<"Method 1: vz"<<endl;
    cout<<"vz_0: "<<vz_bin[0]<<endl;
    cout<<"vz_1: "<<vz_bin[1]<<endl;
    cout<<"vz_2: "<<vz_bin[2]<<endl;
    cout<<"vz_3: "<<vz_bin[3]<<endl;
    cout<<"vz_4: "<<vz_bin[4]<<endl;
    cout<<"Method 1: Q2"<<endl;
    cout<<"Q2_0: "<<Q2_bin[0]<<endl;
    cout<<"Q2_1: "<<Q2_bin[1]<<endl;
    cout<<"Q2_2: "<<Q2_bin[2]<<endl;
    cout<<"Q2_3: "<<Q2_bin[3]<<endl;
    cout<<"Q2_4: "<<Q2_bin[4]<<endl;
    cout<<"Method 1: P_sf"<<endl;
    cout<<"P_sf_0: "<<P_sf_bin[0]<<endl;
    cout<<"P_sf_1: "<<P_sf_bin[1]<<endl;

    //METHOD2
    cout<<"Method 2: 100 electrons"<<endl;
    int counter=0;
    for (Long64_t i=0;i<n_entries;i++) {
        input_tuple->GetEntry(i);
        P_sf=E_PCAL/p;
        if (pid==11&&E_PCAL>0){
            counter++;
            cout<<std::setprecision(5)<<counter<<", "<<v_z<<", "<<Q2<<", "<<P_sf<<endl;
        }
        if(counter==100){break;}
    }
    //output->Close();
}