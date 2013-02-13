TFile *fpIn;
TTree *hkTree;

TCanvas *canL1[4];
TCanvas *canL2[4];
TCanvas *canL3;
TCanvas *canL4;
TCanvas *canT1;
TH1* histL1[4][4];
TH1* histL2[4][4];
TH1* histL3[2];
TH1* histL4[4];
TH1* histT1[4];


void threshold_plots(char *fileName){

  fpIn = new TFile(fileName);
  hkTree = (TTree*) fpIn->Get("eventHkTree");

}

TCanvas *getL1Canvas(Int_t tda){
  if(canL1[tda]) delete canL1[tda];
  for(int i=0;i++;i<4){
    if(histL1[tda][i]) delete histL1[tda][i];
  }

  char canName[100];
  char histExp[100];
  char histCut[100];
  char histName[100];
  sprintf(canName, "canL1Tda%i", tda);

  canL1[tda] = new TCanvas(canName, canName);
  canL1[tda]->Divide(2,2);
  for(int chan=0;chan<4;chan++){
    canL1[tda]->cd(chan+1);
    sprintf(histName, "histL1ScalerTda%iChan%i", tda, chan);
    sprintf(histExp, "eventHk.getSingleChannelRateHz(%i, %i):thresholdDac[%i]>>%s", tda, chan, tda*4+chan, histName);
    hkTree->Draw(histExp);
    histL1[tda][chan] = (TH1*) gDirectory->Get(histName);
    histL1[tda][chan]->SetName(histName);
    histL1[tda][chan]->SetTitle(histName);
  }

    
}

TCanvas *getL2Canvas(Int_t tda){
  if(canL2[tda]) delete canL2[tda];
  for(int i=0;i++;i<4){
    if(histL2[tda][i]) delete histL2[tda][i];
  }

  char canName[100];
  char histExp[100];
  char histCut[100];
  char histName[100];
  sprintf(canName, "canL2Tda%i", tda);

  canL2[tda] = new TCanvas(canName, canName);
  canL2[tda]->Divide(2,2);
  for(int chan=0;chan<4;chan++){
    canL2[tda]->cd(chan+1);
    sprintf(histName, "histL2ScalerTda%iChan%i", tda, chan);
    sprintf(histExp, "l2Scaler[%i]:thresholdDac[%i]>>%s", tda*4+chan, tda*4+chan, histName);
    hkTree->Draw(histExp);
    histL2[tda][chan] = (TH1*) gDirectory->Get(histName);
    histL2[tda][chan]->SetName(histName);
    histL2[tda][chan]->SetTitle(histName);
  }

    
}
TCanvas *getL3Canvas(){
  if(canL3) delete canL3;
  for(int i=0;i++;i<2){
    if(histL3[i]) delete histL3[i];
  }

  char canName[100];
  char histExp[100];
  char histCut[100];
  char histName[100];
  sprintf(canName, "canL3");

  canL3 = new TCanvas(canName, canName);
  canL3->Divide(2);
  for(int chan=0;chan<2;chan++){
    canL3->cd(chan+1);
    sprintf(histName, "histL3Scaler%i", chan);
    sprintf(histExp, "l3Scaler[%i]:thresholdDac[%i]>>%s", chan, chan, histName);
    hkTree->Draw(histExp);
    histL3[chan] = (TH1*) gDirectory->Get(histName);
    histL3[chan]->SetName(histName);
    histL3[chan]->SetTitle(histName);
  }

    
}
TCanvas *getL4Canvas(){
  if(canL4) delete canL4;
  for(int i=0;i++;i<4){
    if(histL4[i]) delete histL4[i];
  }

  char canName[100];
  char histExp[100];
  char histCut[100];
  char histName[100];
  sprintf(canName, "canL4");

  canL4 = new TCanvas(canName, canName);
  canL4->Divide(2,2);
  for(int chan=0;chan<4;chan++){
    canL4->cd(chan+1);
    sprintf(histName, "histL4Scaler%i", chan);
    sprintf(histExp, "l4Scaler[%i]:thresholdDac[%i]>>%s", chan, chan, histName);
    hkTree->Draw(histExp);
    histL4[chan] = (TH1*) gDirectory->Get(histName);
    histL4[chan]->SetName(histName);
    histL4[chan]->SetTitle(histName);
  }

    
}
TCanvas *getT1Canvas(){
  if(canT1) delete canT1;
  for(int i=0;i++;i<4){
    if(histT1[i]) delete histT1[i];
  }

  char canName[100];
  char histExp[100];
  char histCut[100];
  char histName[100];
  sprintf(canName, "canT1");

  canT1 = new TCanvas(canName, canName);
  canT1->Divide(2,2);
  for(int chan=0;chan<4;chan++){
    canT1->cd(chan+1);
    sprintf(histName, "histT1Scaler%i", chan);
    sprintf(histExp, "t1Scaler[%i]:thresholdDac[%i]>>%s", chan, chan, histName);
    hkTree->Draw(histExp);
    histT1[chan] = (TH1*) gDirectory->Get(histName);
    histT1[chan]->SetName(histName);
    histT1[chan]->SetTitle(histName);
  }

    
}
