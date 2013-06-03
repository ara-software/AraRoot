void runPlotDeltaT(){
  gSystem->AddIncludePath("-I${ARA_UTIL_INSTALL_DIR}/include");
  gSystem->Load("libAraEvent.so");
  gSystem->CompileMacro("dbScripts/plotDeltaT.C");
  
  plotDeltaT(2);
  
  
  
  
  
  
}
