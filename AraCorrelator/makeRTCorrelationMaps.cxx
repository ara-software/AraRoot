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

double getUnixTime(void){
    struct timespec tv;
    if(clock_gettime(CLOCK_REALTIME, &tv) != 0) return 0;
    return (tv.tv_sec + (tv.tv_nsec/1000000000.0));
}

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

    // setup the paths to our ray tracing tables
    double radius = 300.;
    double angular_size = 1.;
    int iceModel = 40;
    char dirPath[500];
    char refPath[500];
    std::string topDir = "/cvmfs/icecube.osgstorage.org/icecube/PUBLIC/groups/arasoft/raytrace_timing_tables/";
    sprintf(dirPath, "%s/arrivaltimes_station_%d_icemodel_%d_radius_%.2f_angle_%.2f_solution_0.root",
        topDir.c_str(), station, iceModel, radius, angular_size
    );
    sprintf(refPath, "%s/arrivaltimes_station_%d_icemodel_%d_radius_%.2f_angle_%.2f_solution_1.root",
        topDir.c_str(), station, iceModel, radius, angular_size
    );

    int numAntennas = 16;
    // initialize a correlator
    RayTraceCorrelator *theCorrelator = new RayTraceCorrelator(station, numAntennas,
        radius, angular_size, dirPath, refPath
    );

    // and tell it to load up the arrival times tables
    theCorrelator->LoadTables();

    // How you set up the pairs is up to you!
    // There are a few helper functions;
    // for example, here we can load all of the VPol pairs.

    AraGeomTool *geomTool = AraGeomTool::Instance();
    std::vector<int> excludedChannels = {15};
    std::map< int, std::vector<int> > pairs = theCorrelator->SetupPairs(station, geomTool, AraAntPol::kVertical, excludedChannels);
    std::cout<<"Number of pairs "<<pairs.size()<<std::endl;
    for(int i=0; i<pairs.size(); i++){
        printf("Pair %d: %d, %d\n",i,pairs.find(i)->second[0], pairs.find(i)->second[1]);
    }

    printf("------------------\n");

    auto arrivalDelays = theCorrelator->GetArrivalDelays(pairs);

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

    numEntries=500;
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

        auto corrFunctions = theCorrelator->GetCorrFunctions(pairs, interpolatedWaveforms); // apply Hilbert envelope is default

        // get the map
        double start_time = getUnixTime();
        auto dirMap = theCorrelator->GetInterferometricMap(pairs, corrFunctions, arrivalDelays, 0); // direct solution
        double stop_time = getUnixTime();
        double difference = stop_time - start_time;
        std::cout<<"Time to make single map is "<<difference<<std::endl;  
    
        
        // draw and save the map
        gStyle->SetOptStat(0);
        TCanvas *c = new TCanvas("", "", 2200, 850);
        c->Divide(2,1);
            c->cd(1);
            dirMap.Draw("colz"); // standard colz projection
            dirMap.GetXaxis()->SetTitle("Phi [deg]");
            dirMap.GetYaxis()->SetTitle("Theta [deg]");
            dirMap.GetZaxis()->SetTitle("Summed Correlation");
            dirMap.SetTitle("Standard colz Projection");
            gPad->SetRightMargin(0.15);
        auto dirMap_copy = (TH2D*) dirMap.Clone();
            c->cd(2);
            dirMap_copy->Draw("z aitoff"); // aitoff projection
            dirMap_copy->GetXaxis()->SetTitle("Phi [deg]");
            dirMap_copy->GetYaxis()->SetTitle("Theta [deg]");
            dirMap_copy->SetTitle("Aitoff Projection");
            gPad->SetRightMargin(0.15);
        char title[500];
        sprintf(title,"maps_ev%d.png", event);
        c->SaveAs(title);
        delete c;
        delete dirMap_copy;

        // cleanup
        for(int i=0; i<16; i++){
            delete interpolatedWaveforms[i];
        }
        delete realAtriEvPtr;

    }
}
