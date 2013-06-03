void runCalibrationFitter(){

  gSystem->AddIncludePath("-I${ARA_UTIL_INSTALL_DIR}/include");
  gSystem->Load("libAraEvent.so");
  gSystem->CompileMacro("$ARA_ROOT_DIR/calibration/ATRI/voltageCalib/calibrationFitterTest.cxx");

  calibrationFitter("/unix/ara/data/calibration/ARA02/root/voltageCalib2/ADCmVConversion.root", "fitter.root");









}
