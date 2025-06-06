
void runFirstCalib() {
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

  gSystem->CompileMacro("firstCalibTry.cxx");
  firstCalibTry(399,0.225);

}
