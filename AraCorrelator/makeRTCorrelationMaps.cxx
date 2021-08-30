#include <iostream>

// ROOT Includes
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TStyle.h"

// ARA Includes
#include "AraGeomTool.h"
#include "RayTraceCorrelator.h"
#include "RawAtriStationEvent.h"
#include "UsefulAtriStationEvent.h"
#include "FFTtools.h"
RawAtriStationEvent *rawAtriEvPtr;

int main(int argc, char **argv)
{
    double interpV = 0.4;
    double interpH = 0.625;

    if(argc<3) {
        std::cout << "Usage\n" << argv[0] << " <station> <input file>\n";
        std::cout << "e.g.\n" << argv[0] << " 2 http://www.hep.ucl.ac.uk/uhen/ara/monitor/root/run1841/event1841.root\n";
        return 0;
    }

    int station = atoi(argv[1]);

    /////////////////////////////////////////////////
    /////////////////////////////////////////////////
    //// Initialize the correlator
    /////////////////////////////////////////////////
    /////////////////////////////////////////////////

    // initialize a correlator
    double radius = 300.;
    double angular_size = 1.;
    int iceModel = 0;
    int unixTime = 0;
    RayTraceCorrelator *theCorrelator = new RayTraceCorrelator(station, 
        radius, angular_size, iceModel, unixTime
    );

    // and tell it to load up the arrival times tables
    theCorrelator->LoadTables("/cvmfs/ara.opensciencegrid.org/data/raytrace_tables/");

    // How you set up the pairs is up to you!
    // There are a few helper functions;
    // for example, here we can load all of the VPol pairs.

    std::vector<int> excludedChannels = {15};
    std::map< int, std::vector<int> > pairs = theCorrelator->SetupPairs(AraAntPol::kVertical, excludedChannels);
    std::cout<<"Number of pairs "<<pairs.size()<<std::endl;
    for(int i=0; i<pairs.size(); i++){
        printf("Pair %d: %d, %d\n",i,pairs.find(i)->second[0], pairs.find(i)->second[1]);
    }

    printf("------------------\n");


    /////////////////////////////////////////////////
    /////////////////////////////////////////////////
    //// Actually use it on some data
    /////////////////////////////////////////////////
    /////////////////////////////////////////////////

    TFile *fp = TFile::Open(argv[2]);
    if(!fp) { std::cerr << "Can't open file\n"; return -1; }
    TTree *eventTree = (TTree*) fp->Get("eventTree");
    if(!eventTree) { std::cerr << "Can't find eventTree\n"; return -1; }
    eventTree->SetBranchAddress("event", &rawAtriEvPtr);
    Long64_t numEntries=eventTree->GetEntries();

    numEntries=10;
    for(Long64_t event=0;event<numEntries;event++) {
        eventTree->GetEntry(event);

        bool isCalpulser = rawAtriEvPtr->isCalpulserEvent();
        if(!isCalpulser) continue;

        std::cout<<"Looking at event number "<<event<<std::endl;

        UsefulAraStationEvent * realAtriEvPtr = new UsefulAtriStationEvent(rawAtriEvPtr, AraCalType::kLatestCalib);

        std::map<int, TGraph*> interpolatedWaveforms;
        for(int i=0; i<16; i++){
            TGraph *gr = realAtriEvPtr->getGraphFromRFChan(i);
            TGraph *grInt = FFTtools::getInterpolatedGraph(gr,i<8?interpV:interpH);
            interpolatedWaveforms[i] = grInt;
            delete gr;
        }

        // get the map
        TH2D *dirMap = theCorrelator->GetInterferometricMap(interpolatedWaveforms, pairs, 0); // direct solution

        // draw and save the map
        gStyle->SetOptStat(0);
        TCanvas *c = new TCanvas("", "", 1100, 850);
        dirMap->Draw("colz");
        dirMap->GetXaxis()->SetTitle("Phi [deg]");
        dirMap->GetYaxis()->SetTitle("Theta [deg]");
        dirMap->GetZaxis()->SetTitle("Summed Correlation");
        c->SetRightMargin(0.15);
        char title[500];
        sprintf(title,"maps_ev%d.png", event);
        c->SaveAs(title);

        // cleanup
        delete c;
        delete dirMap;

    }
}
