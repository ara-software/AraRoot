gSystem->Reset();

void runAraDisplay() {
  
  char fileName[180];  

  int run=394;
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


  //-20 pedestal
  //    AraEventCalibrator::Instance()->setAtriPedFile("~/ara/data/miniATRI/run_000302/pedestalValues.run000302.dat");

  //RoomTemp pedestal
  //  AraEventCalibrator::Instance()->setPedFile("/unix/ara/data/ara_station1_ICRR_calibration/data/peds/run_002442/peds_1324990418/peds_1324990418.988115.run002442.dat", 1);

  AraDisplay *magicPtr = new AraDisplay(eventFile,AraCalType::kNoCalib);  
	//    AraDisplay *magicPtr = new AraDisplay(eventFile,AraCalType::kJustPed);  
  magicPtr->startEventDisplay();
}
