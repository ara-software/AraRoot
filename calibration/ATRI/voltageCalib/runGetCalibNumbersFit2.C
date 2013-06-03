void runGetCalibNumbersFit2(){

  gSystem->AddIncludePath("-I${ARA_UTIL_INSTALL_DIR}/include");
  gSystem->Load("libAraEvent.so");
  gSystem->CompileMacro("$ARA_ROOT_DIR/calibration/ATRI/voltageCalib/getCalibNumbersFit2.cxx");


  getCalibNumbers("/unix/ara/data/calibration/ARA02/root/voltageCalib2/", "/home/jdavies/repositories/ara/AraRoot/trunk/AraEvent/calib/ATRI/araAtriStation2ADCVoltConversion.txt");
 

 
}
