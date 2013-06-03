TChain *chain = new TChain("maxPosNegTree");
TGraph *grTemp;
TGraph* grPos;
TGraph* grNeg;
TMultiGraph *grPosNeg ;
TCanvas *tempCan;

void plotADCVoltage(int dda, int chan, int block, int sample){


  
  for(int run=442;run<460;run++){
    if(tempCan) delete tempCan;
    char runName[100];
    sprintf(runName, "/unix/ara/data/calibration/ARA02/root/run%i/adcSampleBlock2.root",run);
    chain->Add(runName);
  }

  char cuts[100];
  char exp[100];
  sprintf(cuts, "dda==%i&&chan==%i&&block==%i&&sample==%i", dda, chan, block, sample);
  sprintf(exp, "voltage/2:maxPosValue");
  tempCan = new TCanvas();
  chain->Draw(exp, cuts);
  grTemp = (TGraph*)gPad->GetPrimitive("Graph");
  grPos = (TGraph*) grTemp->Clone();
  grPos->SetName("grPos");
  grPos->SetTitle("grPos");
  grPos->Fit("pol2");

  delete   tempCan;
  tempCan = new TCanvas();

  char cuts[100];
  char exp[100];
  sprintf(cuts, "dda==%i&&chan==%i&&block==%i&&sample==%i", dda, chan, block, sample);
  sprintf(exp, "-1*voltage/2:maxNegValue");
  tempCan = new TCanvas();
  chain->Draw(exp, cuts);
  grTemp = (TGraph*)gPad->GetPrimitive("Graph");
  grNeg = (TGraph*) grTemp->Clone();
  grNeg->SetName("grNeg");
  grNeg->SetTitle("grNeg");
  grNeg->Fit("pol2");

  delete   tempCan;
  tempCan = new TCanvas();


  char grTitle[100];
  sprintf(grTitle, "ADC - mV dda %i chan %i sample %i", dda, chan, sample);
  printf("grTitle %s\n", grTitle);
  
  grPosNeg = new TMultiGraph("grPosNeg", grTitle);
  grPosNeg->Add(grPos, "p");
  grPosNeg->Add(grNeg, "p");
  grPosNeg->Draw("AP");
  grPosNeg->SetTitle(grTitle);
  char xAxisTitle[100];
  sprintf(xAxisTitle, "ADC");
  grPosNeg->GetXaxis()->SetTitle(xAxisTitle);
  char yAxisTitle[100];
  sprintf(yAxisTitle, "mV");
  grPosNeg->GetYaxis()->SetTitle(yAxisTitle);



  delete   tempCan;
  tempCan = new TCanvas();
  
  grPosNeg->Draw("AP");

  

}
