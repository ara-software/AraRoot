#include "TFile.h"
#include "RawAtriStationEvent.h"
#include "UsefulAtriStationEvent.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){

	if(argc<2){
		std::cout<<"Usage requires input in the form: " << basename(argv[0]) << " <input data file>"<<std::endl;
		exit(-1);
	}

	// Check if the data file can be opened at all
	TFile *fpIn = new TFile(argv[1], "READ");
	if(fpIn->IsZombie()){
		printf("Cannot open ARA data file (%s). Test will fail.\n",argv[1]);
		exit(-1);
	}


}

