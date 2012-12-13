Double_t sample_times_out[4][8][2][64]={{{{0}}}};
Double_t sample_times_delta_out[4][8][2][64]={{{{0}}}};
Int_t sample_index_out[4][8][2][64]={{{{0}}}};
Double_t epsilon_times_out[4][8][2]={{{0}}};
Int_t numSamples[4][8][2]={{{0}}};

void makeFakeCalib(){
  Int_t dda=0,chan=0,capArray=0,sample=0;

  for(dda=0;dda<4;dda++){
    for(chan=0;chan<8;chan++){
      for(capArray=0;capArray<2;capArray++){
	for(sample=0;sample<64;sample++){
	  //	  if(sample%2==1) continue;
	  sample_index_out[dda][chan][capArray][numSamples[dda][chan][capArray]]=sample;
	  sample_times_out[dda][chan][capArray][numSamples[dda][chan][capArray]]=sample;
	  numSamples[dda][chan][capArray]++;
	}
      }
    }
  }

  for(dda=0;dda<4;dda++){
    for(chan=0;chan<8;chan++){
      for(capArray=0;capArray<2;capArray++){
	epsilon_times_out[dda][chan][capArray]=1./3.2;
      }
    }
  }
  


  save_inter_sample_times("araAtriStation2SampleTiming_fake.txt");
  save_epsilon_times("araAtriStation2Epsilon_fake.txt");

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
	for(sample=0;sample<numSamples[dda][chan][capArray];sample++) {
	  OutFile << sample_times_out[dda][chan][capArray][sample] << " ";
	}
	OutFile << "\n";
	// OutFile << dda << "\t" << chan << "\t" << capArray << "\t" << numSamples[dda][chan][capArray] << "\t"
	// for(sample=0;sample<numSamples[dda][chan][capArray];sample++) {
	//   //time values
	//   OutFile << sample_times_out[dda][chan][capArray][sample] << " ";
	// }
	// OutFile << "\n";
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
