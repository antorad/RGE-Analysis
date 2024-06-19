//output directory
TString output_directory="output/";

// Define global variables
TString var = "z_h";
TString hadron_pid = "211";
const int nbins = 100;
const double shift = 0.0; // shift in the x axes for better readability

//target list and color graph options
vector<string> run_number_list = {"020144", "020145", "020148"};
vector<string> tarList = {"C", "Al", "Cu", "Sn", "Pb"};
vector<string> colorList = {"red", "purple", "blue", "green", "black"};
vector<string> markerList = {"o", "o", "o", "o", "o"};

void plot_mr() {
cout<<var<<endl;
    //create canvas
    TCanvas *canvas = new TCanvas("canvas", "Multiplicity Ratio", 800, 600);
    canvas->SetGrid();
    TMultiGraph *mg = new TMultiGraph();
/*
     for (size_t i = 0; i < run_number_list.size(); ++i) {
        TString fileName = output_directory + TString(run_number_list[i]) + "/out_clas12_" + hadron_pid + "_" + TString(run_number_list[i]) + ".root";
        TFile *file = new TFile(fileName, "READ");

        if (!file || file->IsZombie()) {
            cerr << "Could not open file " << fileName << endl;
            continue;
        }

        TString graphName = "multiplicity_ratio_" + var;
        TGraphErrors *graph = (TGraphErrors*)file->Get(graphName);

        if (!graph) {
            cerr << "Could not get graph " << graphName << " from file " << fileName << endl;
            file->Close();
            continue;
        }

        int nPoints = graph->GetN();
        cout<<nPoints;
        vector<double> x(nPoints), y(nPoints), ey(nPoints);

        for (int j = 0; j < nPoints; ++j) {
            graph->GetPoint(j, x[j], y[j]);
            x[j] += (-shift + shift * 2 * i);
            ey[j] = graph->GetErrorY(j);
        }

        TGraphErrors *graphShifted = new TGraphErrors(nPoints, x.data(), y.data(), nullptr, ey.data());
        graphShifted->SetMarkerStyle(20 + i);
        graphShifted->SetMarkerColor(i + 1);
        graphShifted->SetLineColor(i + 1);
        mg->Add(graphShifted, "P");

        file->Close();
    }

    mg->Draw("A");
    mg->GetXaxis()->SetTitle(TString(var));
    mg->GetYaxis()->SetTitle("R");

    // Adding legend
    //TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    //for (size_t i = 0; i < tarList.size(); ++i) {
    //    legend->AddEntry(mg->GetListOfGraphs()->At(i), tarList[i], "P");
    //}
    //legend->Draw();
*/
    canvas->SaveAs(output_directory + "multiplicity_Ratio_" + hadron_pid + "_" + var + ".png");

}