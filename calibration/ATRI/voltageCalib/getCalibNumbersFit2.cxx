#include "TFile.h"
#include "TChain.h"
#include "TMath.h"

#include "araSoft.h"

#include <iostream>
#include <fstream>

TChain *chain = new TChain("fitTree");

void getCalibNumbers(char *baseDir, char *outFileName, bool debug=false){

  char fileName[FILENAME_MAX];
  for(int block=0;block<BLOCKS_PER_DDA; block++){
    sprintf(fileName, "%s/fitBlock%i.root", baseDir, block);
    chain->Add(fileName);
  }
  
  Int_t numEntries = chain->GetEntries();


  Int_t dda=0, chan=0, sample=0, block=0;
  Double_t p0=0, p1=0, p2=0, p3=0, p4=0;

  chain->SetBranchAddress("dda", &dda);
  chain->SetBranchAddress("chan", &chan);
  chain->SetBranchAddress("sample", &sample);
  chain->SetBranchAddress("block", &block);

  chain->SetBranchAddress("p0", &p0);
  chain->SetBranchAddress("p1", &p1);
  chain->SetBranchAddress("p2", &p2);
  chain->SetBranchAddress("p3", &p3);
  chain->SetBranchAddress("p4", &p4);

  Double_t ****P0= new Double_t***[DDA_PER_ATRI];
  Double_t ****P1= new Double_t***[DDA_PER_ATRI];
  Double_t ****P2= new Double_t***[DDA_PER_ATRI];
  Double_t ****P3= new Double_t***[DDA_PER_ATRI];
  Double_t ****P4= new Double_t***[DDA_PER_ATRI];

  Int_t firstBadSample=0;

  printf("Initialise Arrays\n");

   for(dda=0;dda<DDA_PER_ATRI;dda++){
     P0[dda] = new Double_t**[RFCHAN_PER_DDA];
     P1[dda] = new Double_t**[RFCHAN_PER_DDA];
     P2[dda] = new Double_t**[RFCHAN_PER_DDA];
     P3[dda] = new Double_t**[RFCHAN_PER_DDA];
     P4[dda] = new Double_t**[RFCHAN_PER_DDA];


     for(chan=0;chan<RFCHAN_PER_DDA;chan++){
       P0[dda][chan] = new Double_t*[BLOCKS_PER_DDA];
       P1[dda][chan] = new Double_t*[BLOCKS_PER_DDA];
       P2[dda][chan] = new Double_t*[BLOCKS_PER_DDA];
       P3[dda][chan] = new Double_t*[BLOCKS_PER_DDA];
       P4[dda][chan] = new Double_t*[BLOCKS_PER_DDA];


       for(block=0;block<BLOCKS_PER_DDA;block++){
	 P0[dda][chan][block] = new Double_t[SAMPLES_PER_BLOCK];
	 P1[dda][chan][block] = new Double_t[SAMPLES_PER_BLOCK];
	 P2[dda][chan][block] = new Double_t[SAMPLES_PER_BLOCK];
	 P3[dda][chan][block] = new Double_t[SAMPLES_PER_BLOCK];
	 P4[dda][chan][block] = new Double_t[SAMPLES_PER_BLOCK];


	 for(sample=0;sample<SAMPLES_PER_BLOCK;sample++){
	   P0[dda][chan][block][sample]=0;
	   P1[dda][chan][block][sample]=1;
	   P2[dda][chan][block][sample]=0;
	   P3[dda][chan][block][sample]=0;
	   P4[dda][chan][block][sample]=0;
	 }
       }
     }
   }
  


  Int_t starEvery = numEntries/80;
  for(int entry=0;entry<numEntries;entry++){
    if(entry%starEvery==0) std::cerr << "*";
    chain->GetEntry(entry);
  }
  std::cerr << "\n";
   

}
