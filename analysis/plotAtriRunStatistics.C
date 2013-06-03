TGraph *grTemp;
TGraph *grEventRate;
TGraph *grCalPulserRate;
TGraph *grRF0Rate;
TGraph *grCPURate;
TChain *chain;

//Plots
TCanvas *canCalForceRate;
TCanvas *canTotalRate;
TCanvas *canAll;
TLegend *legAll;
TLegend *legTotalRate;
TLegend *legCalForceRate;
TLegend *legAllRate;
TMultiGraph *grCalForceRate;
TMultiGraph *grTotalRate;
TMultiGraph *grAllRate;
Int_t stationId;

void plotAtriRunStatistics(char *baseName, Int_t runLow, Int_t runHigh, Int_t numWeeks){
  chain = new TChain("runStatsTree");
  char fileName[256];
  for(Int_t runNo=runLow;runNo<=runHigh;runNo++){
    sprintf(fileName, "%s_run%i.root", baseName, runNo);
    printf("Adding %s\n", fileName);
    chain->Add(fileName);
  }
  
  chain->SetBranchAddress("stationId", &stationId);
  chain->GetEntry(0);
  getGraphs();
  TTimeStamp *timeThen = new TTimeStamp();
  timeThen->Add(-7*24*60*60*numWeeks);
  makePlots(timeThen->GetSec());
}





void plotAtriRunStatistics(char *baseName, Int_t runLow, Int_t runHigh){
  chain = new TChain("runStatsTree");
  char fileName[256];
  for(Int_t runNo=runLow;runNo<=runHigh;runNo++){
    sprintf(fileName, "%s_run%i.root", baseName, runNo);
    printf("Adding %s\n", fileName);
    chain->Add(fileName);
  }
  chain->SetBranchAddress("stationId", &stationId);
  chain->GetEntry(0);
  getGraphs();
  makePlots();
  

}
void plotAtriRunStatistics(char *fileName){
  chain = new TChain("runStatsTree");
  chain->Add(fileName);
  chain->SetBranchAddress("stationId", &stationId);
  chain->GetEntry(0);
  getGraphs();
  makePlots();

}

void getGraphs(){

  TCanvas *can = new TCanvas();

  //Event Rate
  //  chain->Draw("eventRate:thisUnixTime", "thisUnixTime-lastUnixTime>=60*30");
  chain->Draw("eventRate:thisUnixTime", "firstTime==0&&lastTime==0");
  grTemp = (TGraph*)gPad->GetPrimitive("Graph");
  grEventRate = (TGraph*)grTemp->Clone();
  grEventRate->SetName("grEventRate");
  grEventRate->SetTitle("Total Event Rate vs. unixTime");
  grEventRate->GetXaxis()->SetTitle("unixTime");
  grEventRate->GetXaxis()->SetTimeDisplay(1);
  grEventRate->GetXaxis()->SetTimeFormat("%Hh-%d");
  grEventRate->GetYaxis()->SetTitle("Rate (Hz)");
  grEventRate->SetMarkerColor(kBlue);
  grEventRate->SetLineColor(kBlue);
  grEventRate->SetLineWidth(4);
  grEventRate->SetMarkerStyle(26);

  //CalPulser
  //  chain->Draw("calPulserRate:thisUnixTime", "thisUnixTime-lastUnixTime>=60*30");
  chain->Draw("calPulserRate:thisUnixTime", "firstTime==0&&lastTime==0");
  grTemp = (TGraph*)gPad->GetPrimitive("Graph");
  grCalPulserRate = (TGraph*)grTemp->Clone();
  grCalPulserRate->SetName("grCalPulserRate");
  grCalPulserRate->SetTitle("CalPulser Event Rate vs. unixTime");
  grCalPulserRate->GetXaxis()->SetTitle("unixTime");
  grCalPulserRate->GetXaxis()->SetTimeDisplay(1);
  grCalPulserRate->GetXaxis()->SetTimeFormat("%Hh-%d");
  grCalPulserRate->GetYaxis()->SetTitle("Rate (Hz)");
  grCalPulserRate->SetMarkerColor(kRed);
  grCalPulserRate->SetLineColor(kRed);
  grCalPulserRate->SetLineWidth(4);
  grCalPulserRate->SetMarkerStyle(26);

  //RF0
  //  chain->Draw("RF0Rate:thisUnixTime", "thisUnixTime-lastUnixTime>=60*30");
  chain->Draw("RF0Rate:thisUnixTime", "firstTime==0&&lastTime==0");
  grTemp = (TGraph*)gPad->GetPrimitive("Graph");
  grRF0Rate = (TGraph*)grTemp->Clone();
  grRF0Rate->SetName("grRF0Rate");
  grRF0Rate->SetTitle("Deep Trigger Event Rate vs. unixTime");
  grRF0Rate->GetXaxis()->SetTitle("unixTime");
  grRF0Rate->GetXaxis()->SetTimeDisplay(1);
  grRF0Rate->GetXaxis()->SetTimeFormat("%Hh-%d");
  grRF0Rate->GetYaxis()->SetTitle("Rate (Hz)");
  grRF0Rate->SetMarkerColor(kGreen);
  grRF0Rate->SetLineColor(kGreen);
  grRF0Rate->SetLineWidth(4);
  grRF0Rate->SetMarkerStyle(24);

  //CPU
  //  chain->Draw("CPURate:thisUnixTime", "thisUnixTime-lastUnixTime>=60*30");
  chain->Draw("CPURate:thisUnixTime", "firstTime==0&&lastTime==0");
  grTemp = (TGraph*)gPad->GetPrimitive("Graph");
  grCPURate = (TGraph*)grTemp->Clone();
  grCPURate->SetName("grCPURate");
  grCPURate->SetTitle("Min-Bias Event Rate vs. unixTime");
  grCPURate->GetXaxis()->SetTitle("unixTime");
  grCPURate->GetXaxis()->SetTimeDisplay(1);
  grCPURate->GetXaxis()->SetTimeFormat("%Hh-%d");
  grCPURate->GetYaxis()->SetTitle("Rate (Hz)");
  grCPURate->SetMarkerColor(kBlack);
  grCPURate->SetLineColor(kBlack);
  grCPURate->SetLineWidth(4);
  grCPURate->SetMarkerStyle(24);

  delete can;


}

void makePlots(time_t then){
  

  TTimeStamp *timeThen = new TTimeStamp(then);
  TTimeStamp *timeNow = new TTimeStamp();

  //Calibration Pulser and Forced trigger rate

  canCalForceRate = new TCanvas("canCalForceRate");
  canCalForceRate->cd();
  grCalForceRate = new TMultiGraph();
  grCalForceRate->SetName("grCalForceRate");
  char title[100];
  sprintf(title, "ARA0%i - Event Rate", stationId);
  grCalForceRate->SetTitle(title);
  grCalForceRate->Add(grCPURate, "l");
  grCalForceRate->Add(grCalPulserRate, "l");
  grCalForceRate->Draw("a");
  grCalForceRate->GetXaxis()->SetTitle("Time");
  grCalForceRate->GetXaxis()->SetRangeUser(timeThen->GetSec(), timeNow->GetSec());
  grCalForceRate->GetXaxis()->SetTimeDisplay(1);
  grCalForceRate->GetXaxis()->SetTimeFormat("%d %B %F1970-01-01");

  grCalForceRate->GetYaxis()->SetTitle("Rate (Hz)");

  legCalForceRate = new TLegend(0.85,0.75,1,0.9);
  legCalForceRate->AddEntry(grCalPulserRate, "CalPulser Rate", "l");
  legCalForceRate->AddEntry(grCPURate, "Min-Bias Rate", "l");
  legCalForceRate->SetFillColor(0);
  legCalForceRate->Draw();
  

  //Total and RF0 Rate

  canTotalRate = new TCanvas("canTotalRate");
  canTotalRate->cd();
  grTotalRate = new TMultiGraph();
  grTotalRate->SetName("grTotalRate");
  char title[100];
  sprintf(title, "ARA0%i - Event Rate", stationId);
  grTotalRate->SetTitle(title);
  grTotalRate->Add(grEventRate, "l");
  grTotalRate->Add(grRF0Rate, "l");
  grTotalRate->Draw("a");
  grTotalRate->GetXaxis()->SetTitle("Time");
  grTotalRate->GetXaxis()->SetRangeUser(timeThen->GetSec(), timeNow->GetSec());
  grTotalRate->GetXaxis()->SetTimeDisplay(1);
  grTotalRate->GetXaxis()->SetTimeFormat("%d %B %F1970-01-01");
  grTotalRate->GetYaxis()->SetTitle("Rate (Hz)");

  legTotalRate = new TLegend(0.85,0.75,1,0.9);
  legTotalRate->AddEntry(grRF0Rate,  "Deep Trigger Rate", "l");
  legTotalRate->AddEntry(grEventRate, "Total Rate", "l");
  legTotalRate->SetFillColor(0);
  legTotalRate->Draw();


  //All - Total, RF0 Calpulser and RF0 rates

  canAll = new TCanvas("canAllRate");
  canAll->cd();
  grAllRate = new TMultiGraph();
  grAllRate->SetName("grAllRate");
  char title[100];
  sprintf(title, "ARA0%i - Event Rate", stationId);
  grAllRate->SetTitle(title);
  grAllRate->Add(grEventRate, "l");
  grAllRate->Add(grRF0Rate, "l");
  grAllRate->Add(grCalPulserRate, "l");
  //  grAllRate->Add(grCPURate, "l");
  grAllRate->Draw("a");
  grAllRate->GetXaxis()->SetTitle("Time");
  grAllRate->GetXaxis()->SetRangeUser(timeThen->GetSec(), timeNow->GetSec());
  grAllRate->GetXaxis()->SetTimeDisplay(1);
  grAllRate->GetXaxis()->SetTimeFormat("%d %B %F1970-01-01");
  grAllRate->GetYaxis()->SetTitle("Rate (Hz)");

  legAllRate = new TLegend(0.85,0.75,1,0.9);
  legAllRate->AddEntry(grRF0Rate,  "Deep Trigger Rate", "l");
  legAllRate->AddEntry(grEventRate, "Total Rate", "l");
  legAllRate->AddEntry(grCalPulserRate, "CalPulser Rate", "l");
  //  legAllRate->AddEntry(grCPURate, "Min-Bias Rate", "l");
  legAllRate->SetFillColor(0);
  legAllRate->Draw();


  
}




