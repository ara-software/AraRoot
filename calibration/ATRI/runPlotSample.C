
void runPlotSample() {
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

  //  gSystem->CompileMacro("quickLoopAndPlot.C");
  gSystem->CompileMacro("plotSampleAdc.C","k");
  plotSampleAdc("/unix/ara/data/hawaii2012/StationOne/root/run277/event277.root",10,0,0);
  //  plotSampleAdc("/Users/rjn/ara/data/ohio2011/root/run413/event413.root",1000,2,1);
}
