#include "TChain.h"
#include <iostream>
#include <fstream>

TChain *chain = new TChain("fitTree");

int plotADCmVConversion(){

  char runName[FILENAME_MAX];
  for(int block=0;block<512;block++){
    sprintf(runName, "/unix/ara/data/calibration/ARA02/root/voltageCalibFridayPM/ADCmVConversion_block%i.root", block);
    printf("%s\n", runName);    
  }
  
  
  
  
  
  
  

  return 0;
}
