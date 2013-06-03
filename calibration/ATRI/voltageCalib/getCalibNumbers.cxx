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
    sprintf(fileName, "%s/ADCmVConversion_block%i.root", baseDir, block);
    chain->Add(fileName);
  }
  
  Int_t numEntries = chain->GetEntries();


  Int_t dda=0, chan=0, sample=0, block=0, posNeg=0;
  Double_t p0=0, p1=0, p2=0, p3=0;

  chain->SetBranchAddress("dda", &dda);
  chain->SetBranchAddress("chan", &chan);
  chain->SetBranchAddress("sample", &sample);
  chain->SetBranchAddress("block", &block);
  chain->SetBranchAddress("posNeg", &posNeg);

  chain->SetBranchAddress("p0", &p0);
  chain->SetBranchAddress("p1", &p1);
  chain->SetBranchAddress("p2", &p2);
  chain->SetBranchAddress("p3", &p3);

  Double_t ****negP1= new Double_t***[DDA_PER_ATRI];
  Double_t ****posP1= new Double_t***[DDA_PER_ATRI];
  Double_t ****posP2= new Double_t***[DDA_PER_ATRI];
  Double_t ****posP3= new Double_t***[DDA_PER_ATRI];

  Double_t ****negP1_in= new Double_t***[DDA_PER_ATRI];
  Double_t ****posP1_in= new Double_t***[DDA_PER_ATRI];
  Double_t ****posP2_in= new Double_t***[DDA_PER_ATRI];
  Double_t ****posP3_in= new Double_t***[DDA_PER_ATRI];
  
  Int_t firstBadSample=0;

  printf("Initialise Arrays\n");

   for(dda=0;dda<DDA_PER_ATRI;dda++){
     negP1[dda] = new Double_t**[RFCHAN_PER_DDA];
     posP1[dda] = new Double_t**[RFCHAN_PER_DDA];
     posP2[dda] = new Double_t**[RFCHAN_PER_DDA];
     posP3[dda] = new Double_t**[RFCHAN_PER_DDA];

     negP1_in[dda] = new Double_t**[RFCHAN_PER_DDA];
     posP1_in[dda] = new Double_t**[RFCHAN_PER_DDA];
     posP2_in[dda] = new Double_t**[RFCHAN_PER_DDA];
     posP3_in[dda] = new Double_t**[RFCHAN_PER_DDA];

     for(chan=0;chan<RFCHAN_PER_DDA;chan++){
       negP1[dda][chan] = new Double_t*[BLOCKS_PER_DDA];
       posP1[dda][chan] = new Double_t*[BLOCKS_PER_DDA];
       posP2[dda][chan] = new Double_t*[BLOCKS_PER_DDA];
       posP3[dda][chan] = new Double_t*[BLOCKS_PER_DDA];

       negP1_in[dda][chan] = new Double_t*[BLOCKS_PER_DDA];
       posP1_in[dda][chan] = new Double_t*[BLOCKS_PER_DDA];
       posP2_in[dda][chan] = new Double_t*[BLOCKS_PER_DDA];
       posP3_in[dda][chan] = new Double_t*[BLOCKS_PER_DDA];

       for(block=0;block<BLOCKS_PER_DDA;block++){
	 negP1[dda][chan][block] = new Double_t[SAMPLES_PER_BLOCK];
	 posP1[dda][chan][block] = new Double_t[SAMPLES_PER_BLOCK];
	 posP2[dda][chan][block] = new Double_t[SAMPLES_PER_BLOCK];
	 posP3[dda][chan][block] = new Double_t[SAMPLES_PER_BLOCK];

	 negP1_in[dda][chan][block] = new Double_t[SAMPLES_PER_BLOCK];
	 posP1_in[dda][chan][block] = new Double_t[SAMPLES_PER_BLOCK];
	 posP2_in[dda][chan][block] = new Double_t[SAMPLES_PER_BLOCK];
	 posP3_in[dda][chan][block] = new Double_t[SAMPLES_PER_BLOCK];

	 for(sample=0;sample<SAMPLES_PER_BLOCK;sample++){
	   negP1[dda][chan][block][sample]=1;
	   posP1[dda][chan][block][sample]=1;
	   posP2[dda][chan][block][sample]=0;
	   posP3[dda][chan][block][sample]=0;
	 }
       }
     }
   }
  


  Int_t starEvery = numEntries/80;
  for(int entry=0;entry<numEntries;entry++){
    if(entry%starEvery==0) std::cerr << "*";
    chain->GetEntry(entry);
    if(posNeg==+1){
      posP1[dda][chan][block][sample]=p1;
      posP2[dda][chan][block][sample]=p2;
      posP3[dda][chan][block][sample]=p3;
      if(firstBadSample==0){
	if(posP1[dda][chan][block][sample]!=posP1[dda][chan][block][sample] ||
	   posP2[dda][chan][block][sample]!=posP2[dda][chan][block][sample] ||
	   posP3[dda][chan][block][sample]!=posP3[dda][chan][block][sample] 	   
	   ){
	  
	  firstBadSample=1;
	  printf("%i %i %i %i\n", dda, chan, block, sample);
	}	 
      }
 	
      
    }
    else if(posNeg==-1){
      negP1[dda][chan][block][sample]=p1;
      if(firstBadSample==0){
	if(negP1[dda][chan][block][sample]!=negP1[dda][chan][block][sample]){
	  
	  firstBadSample=1;
	  printf("%i %i %i %i\n", dda, chan, block, sample);
	}	 
      }

    }
  }
  std::cerr << "\n";


  
  
  
  //Save values;

  std::ofstream outFile(outFileName);
  int entry=0;
  starEvery=(DDA_PER_ATRI*RFCHAN_PER_DDA*BLOCKS_PER_DDA*SAMPLES_PER_BLOCK)/80;
   for(dda=0;dda<DDA_PER_ATRI;dda++){
     for(chan=0;chan<RFCHAN_PER_DDA;chan++){
       
       for(block=0;block<BLOCKS_PER_DDA;block++){
	 outFile << dda << "\t" << chan << "\t" << block << "\t";
   	for(sample=0;sample<SAMPLES_PER_BLOCK;sample++){
	  entry++;
	  if(entry%starEvery==0) std::cerr << "*";
	  if( ((dda==0 || dda==3) && (chan < 6)) || ((dda==1 || dda==2) && (chan < 4))){
	    outFile << negP1[dda][chan][block][sample] << " ";
	    outFile << posP1[dda][chan][block][sample] << " ";
	    outFile << posP2[dda][chan][block][sample] << " ";
	    outFile << posP3[dda][chan][block][sample] << " ";
	    
	  }
	  else{
	    outFile << "1" << " ";
	    outFile << "1" << " ";
	    outFile << "0" << " ";
	    outFile << "0" << " ";
	  }

   	}
   	outFile << "\n";
       }
     }
   }
   std::cerr << "\n";

   outFile.close();

 
   if(debug){
     //Load values;
     entry=0;
     std::ifstream inFile(outFileName);
     
     while(inFile >> dda >> chan >> block){
       for(sample=0;sample<SAMPLES_PER_BLOCK;sample++){
	 entry++;
	 if(entry%starEvery==0) std::cerr << "*";
	 inFile >> negP1_in[dda][chan][block][sample];
	 inFile >> posP1_in[dda][chan][block][sample];
	 inFile >> posP2_in[dda][chan][block][sample];
	 inFile >> posP3_in[dda][chan][block][sample];
	 
	 Double_t precision=1e-5;
	 if(TMath::Abs(negP1_in[dda][chan][block][sample] - negP1[dda][chan][block][sample]) / negP1[dda][chan][block][sample] > precision  ) fprintf(stderr, "%i %i %i %i\n", dda, chan, block, sample);
	 if(TMath::Abs(posP1_in[dda][chan][block][sample] - posP1[dda][chan][block][sample]) / posP1[dda][chan][block][sample] > precision  ) fprintf(stderr, "%i %i %i %i\n", dda, chan, block, sample);
	 if(TMath::Abs(posP2_in[dda][chan][block][sample] - posP2[dda][chan][block][sample]) / posP2[dda][chan][block][sample] > precision  ) fprintf(stderr, "%i %i %i %i\n", dda, chan, block, sample);
	 if(TMath::Abs(posP3_in[dda][chan][block][sample] - posP3[dda][chan][block][sample]) / posP3[dda][chan][block][sample] > precision  ) fprintf(stderr, "%i %i %i %i\n", dda, chan, block, sample);
	 
       }
     }
     std::cerr << "\n";   
     inFile.close();
   }



   

}
