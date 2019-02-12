/*
runAraDisplay.C
By: Jonathan Davies
Updated: Brian Clark (Feb 2019)

This is meant to be used as a ROOT macro. For example:

1) Open ROOT: `root` (with x-forwarding enabled)
2) Load the macro: `.L runAraDisplay.C`
3) Run the function: `runAraDisplay("/path/to/data/event1407.root", "/path/to/pedestal/pedestalValues.txt", 2);

*/


gSystem->Reset();
void runAraDisplay(char *eventFile, char *pedFile, int stationID) {
    gSystem->AddIncludePath("-I${ARA_UTIL_INSTALL_DIR}/include");
    gSystem->SetDynamicPath(std::string(std::string(gSystem->GetDynamicPath())+":${ARA_UTIL_INSTALL_DIR}/lib").c_str());
    gSystem->Load("libfftw3.so");
    gSystem->Load("libgsl.so");
    gSystem->Load("libMathMore.so");
    gSystem->Load("libGeom.so");;
    gSystem->Load("libGraf3d.so");
    gSystem->Load("libPhysics.so");  
    gSystem->Load("libRootFftwWrapper.so");
    gSystem->Load("libAraEvent.so");
    gSystem->Load("libAraCorrelator.so");
    gSystem->Load("libAraDisplay.so");

    AraEventCalibrator::Instance()->setAtriPedFile(pedFile,2);
    AraDisplay *magicPtr = new AraDisplay(eventFile,AraCalType::kJustPed);
    magicPtr->startEventDisplay();
}
