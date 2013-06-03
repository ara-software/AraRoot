void runGetCalibNumbers(){

  gSystem->AddIncludePath("-I${ARA_UTIL_INSTALL_DIR}/include");
  gSystem->Load("libAraEvent.so");
  gSystem->CompileMacro("$ARA_ROOT_DIR/calibration/ATRI/voltageCalib/getCalibNumbers.cxx");


  getCalibNumbers("/unix/ara/data/calibration/ARA02/root/voltageCalibThursdayPM", "/home/jdavies/repositories/ara/AraRoot/trunk/AraEvent/calib/ATRI/araAtriStation2ADCVoltConversion.txt");
 

 
}
