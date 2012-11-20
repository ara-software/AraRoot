gSystem->Reset();

void runAraDisplay(int run, int ped) {
  
  char fileName[180];  
  sprintf(fileName, "/Users/jdavies/ara/data/ntu2012/StationTwo/root/run%i/event%i.root", run, run);
  char pedName[180];  
  sprintf(pedName, "/Users/jdavies/ara/data/ntu2012/StationTwo/raw_data/run_%06d/pedestalValues.run%06d.dat", ped, ped);

  runAraDisplay(fileName, pedName);

}

 
void runAraDisplay(char *eventFile, char *pedFile) {
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

  AraEventCalibrator::Instance()->setAtriPedFile(pedFile,2);

  AraDisplay *magicPtr = new AraDisplay(eventFile,AraCalType::kJustPed);  
  //  AraDisplay *magicPtr = new AraDisplay(eventFile,AraCalType::kNoCalib);  

  magicPtr->startEventDisplay();
}
