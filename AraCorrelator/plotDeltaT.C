

void plotDeltaT()
{
  Double_t nTopOfIce=1.4;
  Double_t ant1[3]={8,-5,-25};
  Double_t ant2[3]={8,5,-30};
  Double_t rho1=TMath::Sqrt(ant1[0]*ant1[0]+ant1[1]*ant1[1]);
  Double_t rho2=TMath::Sqrt(ant2[0]*ant2[0]+ant2[1]*ant2[1]);
  Double_t phi1=TMath::ATan2(ant1[1],ant1[0]);
  Double_t phi2=TMath::ATan2(ant2[1],ant2[0]);
  std::cout << phi1*TMath::RadToDeg() << "\t" << phi2*TMath::RadToDeg() << "\n";
  TH2F *histDt = new TH2F("histDt","histDt",360,-180,180,180,-90,90);
  for(int binx=1;binx<=histDt->GetNbinsX();binx++) {
    Double_t phiWaveDeg=histDt->GetXaxis()->GetBinCenter(binx);
    for(int biny=1;biny<=histDt->GetNbinsY();biny++) {
      Double_t thetaWaveDeg=histDt->GetYaxis()->GetBinCenter(biny);
      Double_t phiWave=phiWaveDeg*TMath::DegToRad();
      Double_t thetaWave=thetaWaveDeg*TMath::DegToRad();
      
      Double_t d1=TMath::Cos(thetaWave)*(ant1[2]*TMath::Tan(thetaWave)+rho1*TMath::Cos(phi1-phiWave));
      Double_t d2=TMath::Cos(thetaWave)*(ant2[2]*TMath::Tan(thetaWave)+rho2*TMath::Cos(phi2-phiWave));
      Double_t t2t1=(d1-d2)*nTopOfIce/TMath::C();
      t2t1*=1e9;
      histDt->Fill(phiWaveDeg,thetaWaveDeg,t2t1);
    }
  }
  histDt->Draw("colz");

}
