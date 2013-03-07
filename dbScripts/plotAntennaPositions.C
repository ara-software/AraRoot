void plotAntennaPositions(char *outName){
  gSystem->Load("libAraEvent.so");


  Int_t stationId=100;//ARA1
  AraStationInfo *stationInfo = new AraStationInfo(stationId);

  TFile *fp = new TFile(outName, "RECREATE");
  TTree *posTree = new TTree("posTree", "Tree containing position information for deep and surface antennas");
  Double_t antLocX=0,antLocY=0,antLocZ=0;
  Int_t chanNum=0,daqChanNum=0;
  posTree->Branch("antLocX", &antLocX, "antLocX/D");
  posTree->Branch("antLocY", &antLocY, "antLocY/D");
  posTree->Branch("antLocZ", &antLocZ, "antLocZ/D");

  posTree->Branch("chanNum", &chanNum, "chanNum/I");
  posTree->Branch("daqChanNum", &daqChanNum, "daqChanNum/I");




  for(int i=0;i<20;i++){
    Double_t *antLoc = stationInfo->getAntennaInfo(i)->getLocationXYZ();
    antLocX=antLoc[0];
    antLocY=antLoc[1];
    antLocZ=antLoc[2];
    chanNum=stationInfo->getAntennaInfo(i)->chanNum;
    daqChanNum=stationInfo->getAntennaInfo(i)->daqChanNum;
    posTree->Fill();
  }

  TH1* histAntPos[3];
  TCanvas *canAntPos = new TCanvas("canAntPos", "canvas showing positions of antennas");
  posTree->Draw("antLocZ:antLocY:antLocX>>histVPolPos", "chanNum<8");
  posTree->Draw("antLocZ:antLocY:antLocX>>histHPolPos", "chanNum<8");
  posTree->Draw("antLocZ:antLocY:antLocX>>histSurfacePos", "chanNum<8");
  
  histAntPos[0] = (TH1*) fp->Get("histVPolPos");
  histAntPos[1] = (TH1*) fp->Get("histHPolPos");
  histAntPos[2] = (TH1*) fp->Get("histSurfPos");

  histAntPos[0]->SetMarkerStyle(34);
  histAntPos[0]->SetMarkerColor(kBlue);
  histAntPos[1]->SetMarkerStyle(33);
  histAntPos[1]->SetMarkerColor(kRed);

  canAntPos->Clear();
  canAntPos->cd();
  histAntPos[0]->Draw();
  histAntPos[1]->Draw("SAME");

  TLegend *leg = new TLegend(0.75,0.75,0.85,0.850);
  leg->AddEntry(histAntPos[0], "VPol Antennas", "p");
  leg->AddEntry(histAntPos[1], "HPol Antennas", "p");
  leg->SetFillColor(0);
  leg->Draw("SAME");
			     




  posTree->Write();
  fp->Write();


}





