gSystem->Reset();

void runAraDisplay() {
  char fileName[180];
  int run=3763; //175MHz @ -20
  //  int run=3755; //680MHz @ -20
  //  int run=3750; //350MHz @ -20


  sprintf(fileName,"~/ara/data/root/run%d/event%d.root",run,run);
  printf("~/ara/data/root/run%d/event%d.root\n",run,run);



  //  runAraDisplay("/Users/rjn/ara/data/root/event_200MHz_DISC01.root");
  //  runAraDisplay("/Users/rjn/ara/data/root/event_frozen_200MHz.root");
  //runAraDisplay("/unix/anita1/ara/calibration/Minus54C/sine_wave_data/root/event500MHz_303mV.root");
  //  runAraDisplay("/unix/anita1/ara/calibration/Minus54C/sine_wave_data/root/event200MHz_317mV.root");
  runAraDisplay(fileName);
  //runAraDisplay("/Users/rjn/ara/data/ohio2011/root/run184/event184.root");
}

 
void runAraDisplay(char *eventFile) {
  gSystem->AddIncludePath("-I${ARA_UTIL_INSTALL_DIR}/include");
  // gSystem->AddIncludePath("-I/sw/include");
  // gSystem->SetDynamicPath(std::string(std::string(gSystem->GetDynamicPath())+":${ARA_UTIL_INSTALL_DIR}/lib").c_str());
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
  AraEventCalibrator::Instance()->setPedFile("/unix/ara/data/ara_station1_ICRR_calibration/data/peds/run_003747/peds_1326108401/peds_1326108401.602169.run003747.dat", 1);

  //RoomTemp pedestal
  //  AraEventCalibrator::Instance()->setPedFile("/unix/ara/data/ara_station1_ICRR_calibration/data/peds/run_002442/peds_1324990418/peds_1324990418.988115.run002442.dat", 1);

  AraDisplay *magicPtr = new AraDisplay(eventFile,AraCalType::kFirstCalib);  
  magicPtr->startEventDisplay();
}
