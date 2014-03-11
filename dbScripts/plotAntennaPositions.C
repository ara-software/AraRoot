void plotAntennaPositions(int stationId,char *outName){
  gSystem->Load("libAraEvent.so");
  AraStationInfo *stationInfo = new AraStationInfo(stationId);

  TFile *fp = new TFile(outName, "RECREATE");
  TTree *posTree = new TTree("posTree", "Tree containing position information for deep and surface antennas");
  Double_t antLocX=0,antLocY=0,antLocZ=0;
  Int_t chanNum=0,daqChanNum=0, polType=0,polNum=0, isCalPulser=0;
  posTree->Branch("antLocX", &antLocX, "antLocX/D");
  posTree->Branch("antLocY", &antLocY, "antLocY/D");
  posTree->Branch("antLocZ", &antLocZ, "antLocZ/D");

  posTree->Branch("chanNum", &chanNum, "chanNum/I");
  posTree->Branch("polType",&polType, "polType/I");
  posTree->Branch("polNum",&polNum, "polNum/I");
  posTree->Branch("daqChanNum", &daqChanNum, "daqChanNum/I");
  posTree->Branch("isCalPulser", &isCalPulser, "isCalPulser/I");

  fprintf(stderr, "Receive Antennas\n\n");


  for(int i=0;i<20;i++){
    Double_t *antLoc = stationInfo->getAntennaInfo(i)->getLocationXYZ();
    antLocX=antLoc[0];
    antLocY=antLoc[1];
    antLocZ=antLoc[2];
    chanNum=stationInfo->getAntennaInfo(i)->chanNum;
    daqChanNum=stationInfo->getAntennaInfo(i)->daqChanNum;
    polType=(int)stationInfo->getAntennaInfo(i)->polType;
    polNum=(int)stationInfo->getAntennaInfo(i)->antPolNum;
    isCalPulser=0;

    if(polType <2)
      fprintf(stderr, "%s ant %i %s X %f Y %f Z %f\n", polType ? "HPol" : "VPol",
	      polNum, stationInfo->getAntennaInfo(i)->designator, antLocX, antLocY, antLocZ);
    posTree->Fill();
  }

  fprintf(stderr, "\n\nCalibration Antennas\n\n");
  
  for(int i=0;i<4;i++){
    Double_t *antLoc = stationInfo->getCalAntennaInfo(i)->getLocationXYZ();
    antLocX=antLoc[0];
    antLocY=antLoc[1];
    antLocZ=antLoc[2];
    chanNum=-1;
    daqChanNum=-1;
    polType=(int)stationInfo->getCalAntennaInfo(i)->polType;
    polNum=-1;
    fprintf(stderr, "%s ant %i X %f Y %f Z %f\n", polType ? "HPol" : "VPol",
	    polNum, antLocX, antLocY, antLocZ);
    isCalPulser=1;
    posTree->Fill();
  }

  //  fillDeltaArray(stationId, antLocations, calLocations, deltaT, deltaR);

  TH1* histVPolAntPos[3];
  TH1* histHPolAntPos[3];
  TH1* histSurfaceAntPos[3];
  TCanvas *canAntPos3D = new TCanvas("canAntPos3D", "Canvas showing positions of antennas in 3D");
  TCanvas *canAntPosXY = new TCanvas("canAntPosXY", "Canvas showing positions of antennas in XY");
  TCanvas *canAntPosXZ = new TCanvas("canAntPosXZ", "Canvas showing positions of antennas in XZ");
  TCanvas *canAntPosYZ = new TCanvas("canAntPosYZ", "Canvas showing positions of antennas in YZ");
  TCanvas *canTemp = new TCanvas("canTemp", "canTemp");
  TGraph *grTemp;
  TGraph *grVpol[2][3];
  TGraph *grHpol[2][3];
  TMultiGraph *grXY;
  TMultiGraph *grXZ;
  TMultiGraph *grYZ;

  //Set up Legend with dummy histograms for the plots
  TLegend *leg = new TLegend(0.86,0.8,0.99,0.98);
  TH1D* legHistVPol = new TH1D("legHistVPol", "legHistVPol", 0,0,0);
  legHistVPol->SetMarkerStyle(34);
  legHistVPol->SetMarkerSize(2);
  legHistVPol->SetMarkerColor(kBlue);
  TH1D* legHistHPol = new TH1D("legHistHPol", "legHistHPol", 0,0,0);
  legHistHPol->SetMarkerStyle(33);
  legHistHPol->SetMarkerColor(kRed);
  legHistHPol->SetMarkerSize(2);
  leg->AddEntry(legHistVPol, "VPol Antennas", "p");
  leg->AddEntry(legHistHPol, "HPol Antennas", "p");
  leg->SetFillColor(0);

			     
  canAntPos3D->cd();
  posTree->SetMarkerColor(kBlue);
  posTree->SetMarkerStyle(34);
  posTree->SetMarkerSize(2);
  posTree->Draw("antLocZ:antLocY:antLocX", "polType==0");
  posTree->SetMarkerColor(kRed);
  posTree->SetMarkerStyle(33);
  posTree->SetMarkerSize(2);
  posTree->Draw("antLocZ:antLocY:antLocX", "polType==1", "SAME");
  leg->Draw("SAME");
  canAntPos3D->Write();

  canTemp->cd();
  posTree->SetMarkerColor(kBlue);
  posTree->SetMarkerStyle(34);
  posTree->SetMarkerSize(2);
  posTree->Draw("antLocY:antLocX", "polType==0");
  grTemp = (TGraph*)gPad->GetPrimitive("Graph");
  grVpol[0][0] = (TGraph*)grTemp->Clone();
  grVpol[0][0]->SetMarkerStyle(34);
  grVpol[0][0]->SetMarkerColor(kBlue);
  grVpol[0][0]->SetMarkerSize(2);


  posTree->SetMarkerColor(kRed);
  posTree->SetMarkerStyle(33);
  posTree->SetMarkerSize(2);
  posTree->Draw("antLocY:antLocX", "polType==1");
  grTemp = (TGraph*)gPad->GetPrimitive("Graph");
  grHpol[0][0] = (TGraph*)grTemp->Clone();
  grHpol[0][0]->SetMarkerStyle(33);
  grHpol[0][0]->SetMarkerColor(kRed);
  grHpol[0][0]->SetMarkerSize(2);
  grXY = makeMultiGraph("grXY", "XY Projection", grVpol[0][0], grHpol[0][0], "antLocX (m)", "antLocY (m)", AraGeomTool::getStationName(stationId));
  canAntPosXY->cd();
  grXY->Draw("A");
  leg->Draw("SAME");
  canAntPosXY->Write();

  canTemp->cd();
  posTree->SetMarkerColor(kBlue);
  posTree->SetMarkerStyle(34);
  posTree->SetMarkerSize(2);
  posTree->Draw("antLocZ:antLocX", "polType==0");
  grTemp = (TGraph*)gPad->GetPrimitive("Graph");
  grVpol[0][1] = (TGraph*)grTemp->Clone();
  grVpol[0][1]->SetMarkerStyle(34);
  grVpol[0][1]->SetMarkerColor(kBlue);
  grVpol[0][1]->SetMarkerSize(2);
  posTree->SetMarkerColor(kRed);
  posTree->SetMarkerStyle(33);
  posTree->SetMarkerSize(2);
  posTree->Draw("antLocZ:antLocX", "polType==1");
  grTemp = (TGraph*)gPad->GetPrimitive("Graph");
  grHpol[0][1] = (TGraph*)grTemp->Clone();
  grHpol[0][1]->SetMarkerStyle(33);
  grHpol[0][1]->SetMarkerColor(kRed);
  grHpol[0][1]->SetMarkerSize(2);
  grXZ = makeMultiGraph("grXZ", "XZ Projection", grVpol[0][1], grHpol[0][1], "antLocX (m)", "antLocZ (m)",AraGeomTool::getStationName(stationId));
  canAntPosXZ->cd();
  grXZ->Draw("A");

  leg->Draw("SAME");
  canAntPosXZ->Write();

  canTemp->cd();
  posTree->SetMarkerColor(kBlue);
  posTree->SetMarkerStyle(34);
  posTree->SetMarkerSize(2);
  posTree->Draw("antLocZ:antLocY", "polType==0");
  grTemp = (TGraph*)gPad->GetPrimitive("Graph");
  grVpol[0][2] = (TGraph*)grTemp->Clone();
  grVpol[0][2]->SetMarkerStyle(34);
  grVpol[0][2]->SetMarkerSize(2);
  grVpol[0][2]->SetMarkerColor(kBlue);

  posTree->SetMarkerColor(kRed);
  posTree->SetMarkerStyle(33);
  posTree->SetMarkerSize(2);
  posTree->Draw("antLocZ:antLocY", "polType==1");
  grTemp = (TGraph*)gPad->GetPrimitive("Graph");
  grHpol[0][2] = (TGraph*)grTemp->Clone();
  grHpol[0][2]->SetMarkerStyle(33);
  grHpol[0][2]->SetMarkerColor(kRed);
  grHpol[0][2]->SetMarkerSize(2);
  grYZ = makeMultiGraph("grYZ", "YZ Projection", grVpol[0][2], grHpol[0][2], "antLocY (m)", "antLocZ (m)",AraGeomTool::getStationName(stationId));
  canAntPosYZ->cd();
  grYZ->Draw("A");

  leg->Draw("SAME");
  canAntPosYZ->Write();


  posTree->Write();
  fp->Write();

  delete canTemp;
  delete stationInfo;

}


TMultiGraph *makeMultiGraph(char *name, char *title, TGraph *gr1, TGraph *gr2, char* xTitle, char *yTitle, char *stationName){
  TMultiGraph *grOut = new TMultiGraph();
  char fullTitle[100];
  sprintf(fullTitle, "%s %s", stationName, title);
  grOut->SetTitle(fullTitle);
  grOut->SetName(name);
  grOut->Add(gr1, "p");
  grOut->Add(gr2, "p");
  TCanvas *can = new TCanvas();
  can->cd();
  grOut->Draw("a");
  grOut->GetXaxis()->SetTitle(xTitle);
  grOut->GetXaxis()->CenterTitle();
  grOut->GetYaxis()->SetTitle(yTitle);
  grOut->GetYaxis()->CenterTitle();
  delete can;
  return grOut;

}

// void fillDeltaArray(Int_t stationId, Double_t *antLocations, Double_t *calLocations, Double_t *deltaTArray, Double_t *deltaRArray){
//   gSystem->Load("libAraEvent.so");
//   AraStationInfo *stationInfo = new AraStationInfo(stationId);
//   for(int i=0;i<20;i++){
//     Double_t *antLoc = stationInfo->getAntennaInfo(i)->getLocationXYZ();
//     antLocations[i][0] = antLoc[0];
//     antLocations[i][1] = antLoc[1];    
//     antLocations[i][2] = antLoc[2];
//   }
//   for(int i=0;i<4;i++){
//     Double_t *antLoc = stationInfo->getCalAntennaInfo(i)->getLocationXYZ();
//     calLocations[i][0] = antLoc[0];
//     calLocations[i][1] = antLoc[1];    
//     calLocations[i][2] = antLoc[2];
//   }
//   for(int ant=0;ant<20;ant++){
//     for(int cal=0;cal<4;cal++){
//       deltaTArray[ant][cal]=getDeltaT(antLocations[ant][0],antLocations[ant][1],antLocations[ant][2],calLocations[cal][0],calLocations[cal][1],calLocations[cal][2]);
//       deltaRArray[ant][cal]=getDeltaR(antLocations[ant][0],antLocations[ant][1],antLocations[ant][2],calLocations[cal][0],calLocations[cal][1],calLocations[cal][2]);
//       printf("ant %i cal %i antLoc %f %f %f calLoc %f %f %f deltaR %f deltaT %f\n", ant, cal, 
// 	     antLocations[ant][0],antLocations[ant][1],antLocations[ant][2],
// 	     calLocations[ant][0],calLocations[ant][1],calLocations[ant][2],
// 	     deltaR, deltaT);
      
//     }
//   }

// }


// Double_t getDeltaR(Double_t x1, Double_t y1, Double_t z1, Double_t x2, Double_t y2, Double_t z2){
//   Double_t deltaR = TMath::Sqrt( TMath::Power(x1-x2,2) +TMath::Power(y1-y2,2) +TMath::Power(z1-z2,2) );
//   return deltaR;
// }

// Double_t getDeltaT(Double_t x1, Double_t y1, Double_t z1, Double_t x2, Double_t y2, Double_t z2){
//   Double_t deltaR = getDeltaR(x1,y1,z1,x2,y2,z2);
//   Double_t n_ice = 1.48;
//   Double_t deltaT = deltaR/TMath::C()*n_ice;
//   return deltaR;
// }
