gSystem->Reset();

void runAraDisplay() {

  //  runAraDisplay("/Users/rjn/ara/data/root/event_200MHz_DISC01.root");
  //  runAraDisplay("/Users/rjn/ara/data/root/event_frozen_200MHz.root");
  //runAraDisplay("/unix/anita1/ara/calibration/Minus54C/sine_wave_data/root/event500MHz_303mV.root");
runAraDisplay("/unix/anita1/ara/calibration/Minus54C/sine_wave_data/root/event200MHz_317mV.root");
}

 
void runAraDisplay(char *eventFile) {
  gSystem->AddIncludePath("-I${ARA_UTIL_INSTALL_DIR}/include");
		
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

  //  AraDisplay *magicPtr = new AraDisplay("/Users/rjn/ara/data/root/",time,AraCalType::kNoCalib);
  //  AraEventCalibrator::Instance()->setPedFile("/Users/rjn/ara/data/frozen_daqbox_calibration/Minus54C/pedestal_files/peds_1291239657/peds_1291239657/peds_1291239657.193855.dat");
  //  AraEventCalibrator::Instance()->setPedFile("/unix/anita1/ara/data/pole11/peds/peds_1293938343/peds_1293938343.353103.dat");
  AraEventCalibrator::Instance()->setPedFile("/unix/anita1/ara/data/frozen_daqbox_calibration/Minus54C/pedestal_files/peds_1291239657/peds_1291239657/peds_1291239657.193855.dat");
  AraDisplay *magicPtr = new AraDisplay(eventFile,AraCalType::kFirstCalib);  
  magicPtr->startEventDisplay();  
}
