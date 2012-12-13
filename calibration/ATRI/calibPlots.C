void calibPlots(char *calibFileName, char *outFileName){
  TFile *fpOut = new TFile(outFileName, "RECREATE");
  TTree *outTree = new TTree("timeTree", "timeTree");
  Double_t time=0,deltaTime=0,epsilon=0;
  Int_t dda=0,chan=0,capArray=0,sample=0, index=0;
  outTree->Branch("dda", &dda, "dda/I");
  outTree->Branch("chan", &chan, "chan/I");
  outTree->Branch("capArray", &capArray, "capArray/I");
  outTree->Branch("sample", &sample, "sample/I");
  outTree->Branch("index", &index, "index/I");
  outTree->Branch("time", &time, "time/D");
  outTree->Branch("deltaTime", &deltaTime, "deltaTime/D");
  outTree->Branch("epsilon", &epsilon, "epsilon/D");
  

  TFile *fpIn = new TFile(calibFileName);
  TTree *inTree = (TTree*) fpIn->Get("binWidthsTree");
  inTree->SetBranchAddress("dda", &dda);
  inTree->SetBranchAddress("chan", &chan);
  inTree->SetBranchAddress("capArray", &capArray);
  inTree->SetBranchAddress("sample", &sample);
  inTree->SetBranchAddress("index", &index);
  inTree->SetBranchAddress("time", &time);
  inTree->SetBranchAddress("epsilon", &epsilon);
  
  Double_t sample_times[4][8][2][64]={{{{0}}}};
  Int_t sample_index[4][8][2][64]={{{{0}}}};
  Double_t epsilon_times[4][8][2]={{{0}}};
  
  for(int entry=0;entry<(inTree->GetEntries());entry++){
    inTree->GetEntry(entry);
    sample_index[dda][chan][capArray][sample]=index;
    sample_times[dda][chan][capArray][sample]=time;
    epsilon_times[dda][chan][capArray]=epsilon;
    //    printf("dda %i chan %i capArray %i sample %i index %i time %f epsilon %f\n", dda, chan, capArray, sample, index, time, epsilon);
  }
  
  for(dda=0;dda<4;dda++){
    for(chan=0;chan<8;chan++){
      for(capArray=0;capArray<2;capArray++){
	for(sample=0;sample<64;sample++){
	  if(sample==0) deltaTime=1;
	  else deltaTime=sample_times[dda][chan][capArray][sample]-sample_times[dda][chan][capArray][sample-1];
	  time=sample_times[dda][chan][capArray][sample];
	  index=sample_index[dda][chan][capArray][sample];
	  epsilon=epsilon_times[dda][chan][capArray];
	  outTree->Fill();
	}
      }
    }
  }
  fpIn->Close();
  fpOut->cd();
  outTree->Write();
  fpOut->Write();
  fpOut->Close();
  
}
