gSystem->Reset();

void runAraDisplay(int run) {
  
  char fileName[180];  
  sprintf(fileName, "/unix/ara/data/miniATRI_ucl/eventFormat2/root/run%d/event%d.root", run, run);
  runAraDisplay(fileName);

}

 
void runAraDisplay(char *eventFile) {
  gSystem->AddIncludePath("-I${ARA_UTIL_INSTALL_DIR}/include");
  //  gSystem->AddIncludePath("-I/sw/include");
  gSystem->SetDynamicPath(std::string(std::string(gSystem->GetDynamicPath())+":${ARA_UTIL_INSTALL_DIR}/lib").c_str());
  //  gSystem->Load("~/repositories/InstallDir/utilities/lib/libsqlite3.so");
  //  gSystem->Load("libsqlite3.so");
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

  TChain *fred=0; //Will this work?

  AraEventCalibrator::Instance()->setAtriPedFile("/unix/ara/data/miniATRI_ucl/eventFormat2/run_000283/pedestalValues.run000283.dat", 2); //BRENDAN -- this is how to set an ATRI pedestal file (second argument is the stationId)

  AraDisplay *magicPtr = new AraDisplay(eventFile,AraCalType::kJustPed);  
  //  AraDisplay *magicPtr = new AraDisplay(eventFile,AraCalType::kNoCalib);  

  magicPtr->startEventDisplay();
}
