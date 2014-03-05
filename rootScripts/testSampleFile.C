

void testSampleFile(char *fileName) {

  std::ifstream Calib(fileName);
  if(!Calib.is_open()) {
    std::cerr << "Couldn't open: " << fileName << "\n";
    return;   
  }

  std::ofstream OutFile("fixedFile.txt");
  if(!OutFile.is_open()) {
    std::cerr << "Couldn't open: fixedFile.txt\n";
    return;   
  }

  Int_t dda,chan,cap,numValid;
  Int_t dda2,chan2,cap2,numValid2;
  Int_t sampArray[64];
  Double_t timeArray[64];
  Int_t indexArray[64];
  
  
  while(Calib >> dda >> chan >> cap >> numValid) {
    for(int i=0;i<numValid;i++) {
      Calib >> sampArray[i];
    }
    Calib >> dda2 >> chan2 >> cap2 >> numValid2;
    for(int i=0;i<numValid;i++) {
      Calib >> timeArray[i];
    }
    std::cout << "Checking: " << dda << "\t" << chan << "\t" << cap << "\n";
    
    
    if(dda!=dda2) std::cerr << "DDA Mismatch\n";
    if(chan!=chan2) std::cerr << "Channel Mismatch\n";
    if(cap!=cap2) std::cerr << "Cap. Array Mismatch\n";
    if(numValid!=numValid2) std::cerr << "Number of Samples Mismatch\n";

    int sampsGood=1;
    int timesGood=1;
    for(int i=0;i<numValid;i++) {
      if(sampArray[i]<0 || sampArray[i]>63) sampsGood=0;
      if(i>0) {
	if(timeArray[i]<timeArray[i-1]) timesGood=0;
      }
    }    
    if(!sampsGood) {
      std::cerr << "Sample Index Error\n";
    }
    if(!timesGood) {
      std::cerr << "Back in time\n";
    }

    TMath::Sort(numValid,timeArray,indexArray,kFALSE);
    OutFile << dda << "\t" << chan << "\t" << cap << "\t" << numValid << "\t";
    for(int i=0;i<numValid;i++) {
      OutFile << sampArray[indexArray[i]] << " ";
    }
    OutFile << "\n";
    OutFile << dda << "\t" << chan << "\t" << cap << "\t" << numValid << "\t";
    for(int i=0;i<numValid;i++) {
      OutFile << timeArray[indexArray[i]] << " ";
    }
    OutFile << "\n";


     
  }
  Calib.close();
  OutFile.close();



}
