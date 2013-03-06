  TGraph *grTemp;
  TGraph *grEventRate;
  TGraph *grCalPulserRate;
  TGraph *grRF0Rate;
  TGraph *grCPURate;

void plotAtriRunStatistics(char *fileName){
  TFile *inFile = TFile::Open(fileName, "UPDATE");
  if(!inFile){
    std::cerr << "Unable to open file " << fileName << std::endl;
    return;
  }
  TTree *inTree = (TTree*) inFile->Get("runStatsTree");
  if(!inTree){
    std::cerr << "Unable to open TTree " << "runStatsTree" << std::endl;
    return;
  }
  

  //Event Rate
  inTree->Draw("eventRate:thisUnixTime", "thisUnixTime-lastUnixTime>=60*30");
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
  grEventRate->SetMarkerStyle(22);

  //CalPulser
  inTree->Draw("calPulserRate:thisUnixTime", "thisUnixTime-lastUnixTime>=60*30");
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
  grCalPulserRate->SetMarkerStyle(24);

  //RF0
  inTree->Draw("RF0Rate:thisUnixTime", "thisUnixTime-lastUnixTime>=60*30");
  grTemp = (TGraph*)gPad->GetPrimitive("Graph");
  grRF0Rate = (TGraph*)grTemp->Clone();
  grRF0Rate->SetName("grRF0Rate");
  grRF0Rate->SetTitle("RF0 Event Rate vs. unixTime");
  grRF0Rate->GetXaxis()->SetTitle("unixTime");
  grRF0Rate->GetXaxis()->SetTimeDisplay(1);
  grRF0Rate->GetXaxis()->SetTimeFormat("%Hh-%d");
  grRF0Rate->GetYaxis()->SetTitle("Rate (Hz)");
  grRF0Rate->SetMarkerColor(kOrange);
  grRF0Rate->SetLineColor(kOrange);
  grRF0Rate->SetMarkerStyle(20);

  //CPU
  inTree->Draw("CPURate:thisUnixTime", "thisUnixTime-lastUnixTime>=60*30");
  grTemp = (TGraph*)gPad->GetPrimitive("Graph");
  grCPURate = (TGraph*)grTemp->Clone();
  grCPURate->SetName("grCPURate");
  grCPURate->SetTitle("CPU Event Rate vs. unixTime");
  grCPURate->GetXaxis()->SetTitle("unixTime");
  grCPURate->GetXaxis()->SetTimeDisplay(1);
  grCPURate->GetXaxis()->SetTimeFormat("%Hh-%d");
  grCPURate->GetYaxis()->SetTitle("Rate (Hz)");
  grCPURate->SetMarkerColor(kBlack);
  grCPURate->SetLineColor(kBlack);
  grCPURate->SetMarkerStyle(26);



}
