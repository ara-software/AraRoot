gSystem->Reset();

void runAraDisplay() {
  
  char fileName[180];  

  int run=399;
  //  int run=3764; //175MHz @ -20 - stationId==0 - TestBed event
  //  int run=3755; //680MHz @ -20
  //  int run=3750; //350MHz @ -20


   //   sprintf(fileName,"~/ara/data/miniATRI/root/run191/event191.root");
     sprintf(fileName, "~/ara/data/miniATRI/root/run%i/event%i.root", run, run);
  //   sprintf(fileName, "~/ara/data/fromWisconsin/root/run012441/run012441.root");
  //  sprintf(fileName, "~/ara/data/ara_station1_ICRR_calibration/root/AraRoot/trunk/run%i/event%i.root", run, run);


   //  printf("~/ara/data/ara_station1_ICRR_calibration/root/AraRoot/trunk/run%d/event%d.root\n",run,run);
  runAraDisplay(fileName);
}

 
void runAraDisplay(char *eventFile) {
  gSystem->AddIncludePath("-I${ARA_UTIL_INSTALL_DIR}/include");
  // gSystem->AddIncludePath("-I/sw/include");
  gSystem->SetDynamicPath(std::string(std::string(gSystem->GetDynamicPath())+":${ARA_UTIL_INSTALL_DIR}/lib").c_str());
  // gSystem->Load("libfftw3.so");
  // gSystem->Load("libgsl.so");
  // gSystem->Load("libMathMore.so");
  // gSystem->Load("libGeom.so");;
  // gSystem->Load("libGraf3d.so");
  // gSystem->Load("libPhysics.so");  
  // gSystem->Load("libRootFftwWrapper.so");     	  
  // gSystem->Load("libAraEvent.so");   	      	  
  // gSystem->Load("libAraCorrelator.so");   	  
  // gSystem->Load("libAraDisplay.so");

  gSystem->Load("libfftw3");
  gSystem->Load("libgsl");
  gSystem->Load("libMathMore");
  gSystem->Load("libGeom");;
  gSystem->Load("libGraf3d");
  gSystem->Load("libPhysics");  
  gSystem->Load("libRootFftwWrapper");     	  
  gSystem->Load("libAraEvent");   	      	  
  gSystem->Load("libAraCorrelator");   	  
  gSystem->Load("libAraDisplay");
  TChain *fred=0; //Will this work?

  //This is how to set a pedestal file manually - note that you cannot use relative paths, you must use the full path for the pedestal file
  AraEventCalibrator::Instance()->setAtriPedFile("/Users/jdavies/ara/data/miniATRI/run_000402/pedTest.txt");

  //  AraDisplay *magicPtr = new AraDisplay(eventFile,AraCalType::kNoCalib);  
  AraDisplay *magicPtr = new AraDisplay(eventFile,AraCalType::kJustPed);  
  magicPtr->startEventDisplay();
}
