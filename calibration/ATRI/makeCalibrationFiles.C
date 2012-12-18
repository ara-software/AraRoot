Double_t sample_times[4][8][2][64]={{{{0}}}};
Double_t sample_times_delta[4][8][2][64]={{{{0}}}};
Int_t sample_index[4][8][2][64]={{{{0}}}};
Double_t epsilon_times[4][8][2]={{{0}}};

Double_t sample_times_out[4][8][2][64]={{{{0}}}};
Double_t sample_times_delta_out[4][8][2][64]={{{{0}}}};
Int_t sample_index_out[4][8][2][64]={{{{0}}}};
Double_t epsilon_times_out[4][8][2]={{{0}}};
Int_t numSamples[4][8][2]={{{0}}};
Int_t useSample[4][8][2][64]={{{{0}}}};


void makeCalibrationFiles(char *inFileName){

  TFile *fpIn = new TFile(inFileName);
  TTree *inTree = (TTree*) fpIn->Get("timeTree");
  Double_t time=0,deltaTime=0,epsilon=0;
  Int_t dda=0,chan=0,capArray=0,sample=0, index=0;
  inTree->SetBranchAddress("dda", &dda);
  inTree->SetBranchAddress("chan", &chan);
  inTree->SetBranchAddress("capArray", &capArray);
  inTree->SetBranchAddress("sample", &sample);
  inTree->SetBranchAddress("index", &index);
  inTree->SetBranchAddress("time", &time);
  inTree->SetBranchAddress("deltaTime", &deltaTime);
  inTree->SetBranchAddress("epsilon", &epsilon);


  for(int entry=0;entry<(inTree->GetEntries());entry++){
    inTree->GetEntry(entry);
    sample_times[dda][chan][capArray][sample]=time;
    sample_times_delta[dda][chan][capArray][sample]=deltaTime;
    sample_index[dda][chan][capArray][sample]=index;
    epsilon_times[dda][chan][capArray]=epsilon;
  }
  
  for(dda=0;dda<4;dda++){
    for(chan=0;chan<8;chan++){
      for(capArray=0;capArray<2;capArray++){
	for(sample=0;sample<64;sample++){
	  if(sample_index[dda][chan][capArray][sample]%2==1){
	    useSample[dda][chan][capArray][sample]=0;
	  }
	  //if(sample_index[dda][chan][capArray][sample]>62) continue;
	  sample_index_out[dda][chan][capArray][numSamples[dda][chan][capArray]]=sample_index[dda][chan][capArray][sample];
	  sample_times_out[dda][chan][capArray][numSamples[dda][chan][capArray]]=sample_times[dda][chan][capArray][sample];
	  numSamples[dda][chan][capArray]++;
	  useSample[dda][chan][capArray][sample]=1;
	}
      }
    }
  }
  
  for(dda=0;dda<4;dda++){
    for(chan=0;chan<8;chan++){
      for(capArray=0;capArray<2;capArray++){
	epsilon_times_out[dda][chan][capArray]=epsilon_times[dda][chan][capArray]+sample_times[dda][chan][1-capArray][63]-sample_times[dda][chan][1-capArray][62];
	fprintf(stderr, "dda %i chan %i capArray %i epsilon %f epsilon_out %f\n", dda, chan, capArray, epsilon_times[dda][chan][capArray],epsilon_times_out[dda][chan][capArray]);
      }
    }
  }
  

  save_inter_sample_times("araAtriStation2SampleTiming_evenOnly.txt");
  save_epsilon_times("araAtriStation2EpsilonTiming_evenOnly.txt");


  
}

Int_t save_inter_sample_times(char* outName){

  std::ofstream OutFile(outName);
  Int_t capArray, sample;

  for(int dda=0;dda<4;dda++){
    for(int chan=0;chan<8;chan++){
      for(int capArray=0;capArray<2;capArray++) {
	OutFile << dda << "\t" << chan << "\t" << capArray << "\t" << numSamples[dda][chan][capArray] << "\t";   
	for(sample=0;sample<numSamples[dda][chan][capArray];sample++) {
	  OutFile << sample_index_out[dda][chan][capArray][sample] << " ";
	}
	OutFile << "\n";
	OutFile << dda << "\t" << chan << "\t" << capArray << "\t" << numSamples[dda][chan][capArray] << "\t";    
	for(int sample=0;sample<numSamples[dda][chan][capArray];sample++) {
	  //time values
	  OutFile << sample_times_out[dda][chan][capArray][sample] << " ";
	}
	OutFile << "\n";
      }
    }
  }
  OutFile.close();

  return 0;
}

Int_t save_epsilon_times(char* outName){

  std::ofstream OutFile(outName);
  Int_t capArray, sample;

  for(Int_t dda=0;dda<4;dda++){
    for(Int_t chan=0;chan<8;chan++){
      for(int capArray=0;capArray<2;capArray++){
	OutFile <<  dda << "\t"
		<< chan << "\t" 
		<< capArray << "\t";
	OutFile << epsilon_times_out[dda][chan][capArray] << "\n";
      }
    }
  }
  OutFile.close();
 
  return 0;
 
}
