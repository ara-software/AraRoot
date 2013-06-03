void runGetADCmVConversion2(){

  gSystem->AddIncludePath("-I${ARA_UTIL_INSTALL_DIR}/include");
  gSystem->Load("libAraEvent.so");
  gSystem->CompileMacro("$ARA_ROOT_DIR/calibration/ATRI/voltageCalib/getADCmVConversion2.cxx");
  
  char baseDir[256];
  sprintf(baseDir, " /unix/ara/data/calibration/ARA02/");
  Int_t runLow=440;
  Int_t runHigh=462;
  Int_t block=0;
  getADCmVConversion(baseDir, runLow, runHigh, block);

 








}
