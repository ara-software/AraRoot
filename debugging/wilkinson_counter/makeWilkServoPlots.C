//fileName should be an eventHk file outName should be output file
/*
e.g.

root
.x makeWilkServoPlots.C("/unix/ara/data/osu2012/root/run43/eventHk43.root", "output.root")

*/


void makeWilkServoPlots(char *fileName, char *outName){

  gStyle->SetOptStat(0);

  TFile *fpIn = new TFile(fileName);
  TTree *hkTree = (TTree*) fpIn->Get("eventHkTree");

  char plotName[400];
  char exp[100];
  char xAxisName[100];
  char yAxisName[100];
  char histName[100];

  char outFileName[200];
  sprintf(outFileName, "%s.root", outName);
  TFile *fpOut = new TFile(outFileName, "RECREATE");

  TH1* histEvent[4];
  TH1* histVdly[4];
  TCanvas can;

  for(int dda=0;dda<4;dda++){
    
    sprintf(plotName, "wilkCounter_vs_Entry_DDA_%i", dda);
    sprintf(exp, "wilkinsonCounterNs(%i).:Entry$>>tempHist", dda);
    sprintf(xAxisName, "Entry");
    sprintf(yAxisName, "WilkinsonCounter (ns)");
    sprintf(histName, "histEventDda%i", dda);
    hkTree->Draw(exp);
    histEvent[dda] = (TH1*) gDirectory->Get("tempHist");
    histEvent[dda]->GetXaxis()->SetTitle(xAxisName);
    histEvent[dda]->GetYaxis()->SetTitle(yAxisName);
    histEvent[dda]->SetTitle(plotName);
    histEvent[dda]->SetName(histName);    
    histEvent[dda]->Write();
    sprintf(plotName, "%s_%s.pdf", outName, plotName);
    histEvent[dda]->Draw();
    can.SaveAs(plotName);
    
    //    delete hista;
    
    sprintf(plotName, "wilkCounter_vs_vdlyDac_DDA_%i", dda);
    sprintf(exp, "wilkinsonCounterNs(%i):vdlyDac[%i]>>tempHist", dda, dda);
    sprintf(xAxisName, "vdlyDac");
    sprintf(yAxisName, "WilkinsonCounter (ns)");
    sprintf(histName, "histVdlyDda%i", dda);

    hkTree->Draw(exp);
    histVdly[dda] = (TH1*) gDirectory->Get("tempHist");
    histVdly[dda]->GetXaxis()->SetTitle(xAxisName);
    histVdly[dda]->GetYaxis()->SetTitle(yAxisName);
    histVdly[dda]->SetTitle(plotName);
    histVdly[dda]->SetName(histName);
    histVdly[dda]->Write();
    sprintf(plotName, "%s_%s.pdf", outName, plotName);
    histVdly[dda]->Draw();
    can.SaveAs(plotName);
    
    
  }//dda





}
