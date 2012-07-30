gSystem->Reset();

void runAraDisplay() {
  
  char fileName[180];  

  int run=399;

  //  sprintf(fileName, "~/ara/data/miniATRI/root/run%i/event%i.root", run, run);
  //JPD Testing TestBed
  //  sprintf(fileName, "~/ara/data/testing_for_trunk/root/TestBed/run12813/event12813.root");
  //JPD Testing Station1
  sprintf(fileName, "~/ara/data/testing_for_trunk/root/Station1/run10160/event10160.root");
  //JPD Testing AraRootBranches3.2
  //  sprintf(fileName, "~/ara/data/AraRoot_3-2/root/0808_2011/TestBed_2011_run005546.L0.root");
  //JPD Testing TestBed vs branches/TestBed1
  //  sprintf(fileName,"~/ara/data/testing_for_trunk/root/TestBed/run5546/event5546.root");

  runAraDisplay(fileName);
}

 
void runAraDisplay(char *eventFile) {
  gSystem->AddIncludePath("-I${ARA_UTIL_INSTALL_DIR}/include");
  gSystem->AddIncludePath("-I/sw/include");
  gSystem->SetDynamicPath(std::string(std::string(gSystem->GetDynamicPath())+":${ARA_UTIL_INSTALL_DIR}/lib").c_str());
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

  //This is how to set a pedestal file manually - note that you cannot use relative paths, you must use the full path for the pedestal file
  //  AraEventCalibrator::Instance()->setAtriPedFile("/Users/jdavies/ara/data/miniATRI/run_000402/pedTest.txt");

  AraDisplay *magicPtr = new AraDisplay(eventFile,AraCalType::kLatestCalib);  
  magicPtr->startEventDisplay();
}
