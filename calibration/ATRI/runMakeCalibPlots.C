void runMakeCalibPlots(){
  gSystem->AddIncludePath("-I${ARA_UTIL_INSTALL_DIR}/include");
  gSystem->Load("libAraEvent.so");
  gSystem->CompileMacro("$ARA_ROOT_DIR/calibration/ATRI/makeCalibPlots.cxx");

  char calibFileName[200];
  sprintf(calibFileName, "~/ara/data/calibration/ATRI/ARA02/root/run466/calibFourthTry.root");

  makeCalibPlots(calibFileName);


}
