// This code calculate and plot the Pt broadening using the histograms generated
// by Integration.cpp It can be compile with g++ -Wall -fPIC -I../include
// `root-config --cflags` Broadening.cpp -o ../bin/Broadening  `root-config
// --glibs` ../include/Broad.h

#include "Broad.h"

int main() {

    TStopwatch t;

    std::cout << "Full Integrated Broad" << std::endl;
    PtBroadeningFullIntegrated(inputDirectory, outputDirectory, "meanPt2.root",
                               "Pt_broad_FullIntegrated.root");
    // PtBroadeningFullIntegrated(inputDirectory, outputDirectory,
    // "meanPt2ZhCut.root", "Pt_broad_FullIntegratedZhCut.root");

    std::cout << "Zh depended Broad" << std::endl;
    PtBroadeningZh(inputDirectory, outputDirectory);

    std::cout << "Q2 depended Broad" << std::endl;
    PtBroadeningQ2(inputDirectory, outputDirectory);

    std::cout << "Nu depended Broad" << std::endl;
    PtBroadeningNu(inputDirectory, outputDirectory);

    t.Print();
}
