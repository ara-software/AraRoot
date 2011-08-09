gSystem->Reset();

void runAraDisplay() {
  gSystem->AddIncludePath("-I${ARA_UTIL_INSTALL_DIR}/include");
		
  gSystem->Load("libfftw3.so");
  gSystem->Load("libgsl.so");
  gSystem->Load("libMathMore.so");
  gSystem->Load("libGeom.so");;
  gSystem->Load("libGraf3d.so");
  gSystem->Load("libPhysics.so");  
  gSystem->Load("libRootFftwWrapper.so");     	  
  gSystem->Load("libAraEvent.so");   	  
  gSystem->Load("libAraDisplay.so");

  TChain *fred=0; //Will this work?
  runAraDisplayTime(1290399409);
}


void runAraDisplayTime(UInt_t time) {
  //  AraDisplay *magicPtr = new AraDisplay("/Users/rjn/ara/data/root/",time,AraCalType::kNoCalib);
  AraDisplay *magicPtr = new AraDisplay("/Users/rjn/ara/data/root/",time,AraCalType::kVoltageTime);
  
  magicPtr->startEventDisplay();  
}
