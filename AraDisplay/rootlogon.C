{
    gSystem->AddIncludePath("-I${ARA_UTIL_INSTALL_DIR}/include");
    gSystem->SetDynamicPath(std::string(std::string(gSystem->GetDynamicPath())+":${ARA_UTIL_INSTALL_DIR}/lib").c_str());
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

}

