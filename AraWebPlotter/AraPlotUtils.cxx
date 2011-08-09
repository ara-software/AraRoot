////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////// Simple utiltities for making the web plots                  /////////
//////                                                             /////////
////// rjn@hep.ucl.ac.uk --- September 2006                        /////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#include "AraPlotUtils.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TColor.h"
#include "TAxis.h"
#include "TH1.h"
#include "TMultiGraph.h"
#include "TPaveText.h"
#include "TLatex.h"
#include <fstream>
#include <utime.h>      
#include <sys/stat.h>

TLatex *gLatex=0;

const char * AraPlotTime::getTimeString(AraPlotTime_t plotTime)
{
    const char *myString;
    switch(plotTime) {
	case kFullTime:
	    myString="All";
	    break;
	case kOneHour:
	    myString="OneHour";
	    break;
	case kSixHours:
	    myString="SixHours";
	    break;
	case kTwelveHours:
	    myString="TwelveHours";
	    break;
	case kOneDay:
	    myString="OneDay";
	    break;
	default:
	    myString="Unknown";
	    break;
    }
    return myString;
}


const char * AraPlotTime::getTimeTitleString(AraPlotTime_t plotTime)
{
    const char *myString;
    switch(plotTime) {
	case kFullTime:
	    myString="All Time";
	    break;
	case kOneHour:
	    myString="Last Hour";
	    break;
	case kSixHours:
	    myString="Last Six Hours";
	    break;
	case kTwelveHours:
	    myString="Last Twelve Hours";
	    break;
	case kOneDay:
	    myString="Last Day";
	    break;
	default:
	    myString="Unknown";
	    break;
    }
    return myString;
}


//AraPlotTime_t operator++ (const AraPlotTime_t& x, int)
//{
//    int ix=(int)x;
//    ++x;
//    return (AraPlotTime_t)x;
//
//}




void AraPlotUtils::setDefaultStyle() {
//    gROOT->SetStyle("Plain");
//    gStyle->SetCanvasBorderMode(0);
//    gStyle->SetFrameBorderMode(0);
//    gStyle->SetPadBorderMode(0);
//    gStyle->SetFrameFillStyle(4000);
//    gStyle->SetFillStyle(4000);
//    gStyle->SetDrawBorder(0);
//    gStyle->SetCanvasBorderSize(0);
//    gStyle->SetFrameBorderSize(0);
//    gStyle->SetPadBorderSize(0);
    gStyle->SetTitleBorderSize(0);

    gStyle->SetStatColor(0);
    gStyle->SetCanvasColor(0);
    gStyle->SetPadColor(0);

//    // Set the size of the default canvas
    gStyle->SetCanvasDefH(400);
    gStyle->SetCanvasDefW(500);
//    gStyle->SetCanvasDefX(10);
//    gStyle->SetCanvasDefY(10);

   // Set Line Widths
//    gStyle->SetFrameLineWidth(1);
    gStyle->SetFuncWidth(1);
//    gStyle->SetHistLineWidth(1);
    gStyle->SetFuncColor(kRed);
    gStyle->SetFuncStyle(2);

//    //Set Marker things for graphs
    gStyle->SetMarkerStyle(29);
    gStyle->SetMarkerColor(9);
    gStyle->SetMarkerSize(0.75);

//    // Set margins -- I like to shift the plot a little up and to the
//    // right to make more room for axis labels
    gStyle->SetPadTopMargin(0.1);
    gStyle->SetPadBottomMargin(0.15);
    gStyle->SetPadLeftMargin(0.12);
    gStyle->SetPadRightMargin(0.05);


//    // Set tick marks and turn off grids
//    gStyle->SetNdivisions(505,"xyz");
   
//    // Set Data/Stat/... and other options
//    gStyle->SetOptDate(0);
//    //gStyle->SetDateX(0.1);
//    //gStyle->SetDateY(0.1);
//    gStyle->SetOptFile(0);
    gStyle->SetOptStat(1110);
    gStyle->SetStatFormat("6.3f");
    gStyle->SetFitFormat("6.3f");
    gStyle->SetOptFit(1);
//    gStyle->SetStatH(0.14);
    gStyle->SetStatStyle(0000);
    gStyle->SetStatW(0.2);
    gStyle->SetStatX(0.95);
    gStyle->SetStatY(0.9);  
    gStyle->SetStatBorderSize(1);
    gStyle->SetStatColor(0);
    gStyle->SetOptTitle(1);
   
//    // Adjust size and placement of axis labels
//    gStyle->SetLabelSize(0.04,"xyz");
//    gStyle->SetLabelOffset(0.01,"x");
//    gStyle->SetLabelOffset(0.01,"y");
//    gStyle->SetLabelOffset(0.005,"z");
//    gStyle->SetTitleSize(0.05,"xyz");
//    gStyle->SetTitleOffset(0.5,"z");
    gStyle->SetTitleOffset(1.3,"y");
   
   
    //    gStyle->SetPalette(1);
   const Int_t NRGBs = 5;
   const Int_t NCont = 255;
   
   Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
   Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
   Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
   Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
   TColor color;
   color.InitializeColors();
   color.CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
   gStyle->SetNumberContours(NCont);

//    gStyle->SetCanvasColor(0);
    gStyle->SetFrameFillColor(0);
//    //   gStyle->SetFillColor(0);

   
//   //   gStyle->SetPalette(51,0);
    gStyle->SetTitleH(0.06);
    gStyle->SetTitleW(0.6);
    gStyle->SetTitleBorderSize(0);
    gStyle->SetTitleStyle(0);
// //   gStyle->SetTitleFontSize(0.1);
//    gStyle->SetTextAlign(12);
// //     gStyle->SetTitleStyle(0); 
// //     gStyle->SetStatX(0.9);
// //     gStyle->SetStatY(0.9);
// //     gStyle->SetStatH(0.2);
// //     gStyle->SetStatW(0.2);
// //     gStyle->SetStatFormat("6.3g");  
// //     gStyle->SetStatBorderSize(1);
// //     gStyle->SetOptStat(1110);
// //     gStyle->SetOptFit(1);
//    //gStyle->SetLabelSize(0.6);
//    //gStyle->SetLabelOffset(0.7);
    gROOT->ForceStyle();
   
}


int AraPlotUtils::getNiceColour(int index)
{
   Int_t baseColours[12]={kRed,kBlue,kGreen,kMagenta,kCyan,kYellow,kPink,kAzure,kSpring,kOrange,kViolet,kTeal};
   Int_t offsetNums[4]={+2,-2,+1,-1};
   Int_t baseNum=index%12;
   Int_t offsetNum=(index/12)%4;
   return (baseColours[baseNum]+offsetNums[offsetNum]);			   
}

int AraPlotUtils::getNiceMarker(int index)
{
    int value=index%6;
    Int_t niceMarkers[6]={22,26,23,28,21,3};
    return niceMarkers[value];
}

UInt_t AraPlotTime::getStartTime(UInt_t currentTime,AraPlotTime_t plotTime)
{
  switch(plotTime) {
  case kFullTime: return 0;
  case kOneHour: return currentTime-3600;
  case kSixHours: return currentTime-(6*3600);
  case kTwelveHours: return currentTime-(12*3600);
  case kOneDay: return currentTime-(24*3600);
  default: return 0; //Who knows what they wanted
  }
}

TMultiGraph* AraPlotUtils::plotMultigraph(TCanvas *can, TGraph *gr[], 
					     Int_t numGraphs,
					     const char *plotTitle, const char *xTitle, const char *yTitle,
					     int timeDisplay)
{
  TMultiGraph *mg=0;
  int gotMg=0;
  for(int i=0;i<numGraphs;i++) {
    if(gr[i]) {

      if(!mg) mg = new TMultiGraph();
      gr[i]->SetLineColor(getNiceColour(i));
      gr[i]->SetMarkerColor(getNiceColour(i));
      gr[i]->SetMarkerStyle(getNiceMarker(i));
      mg->Add(gr[i],"lp");
      gotMg=1;
    }
  }
  
  if(!gotMg) {
    if(mg) delete mg; //Shouldn't ever reach here
    return NULL;  
  }
  //mg->SetTitle(plotTitle);

  mg->Draw("ap");
  if(mg->GetXaxis()) {
    mg->GetXaxis()->SetTimeDisplay(timeDisplay);
    if(xTitle) mg->GetXaxis()->SetTitle(xTitle);
  }
  if(mg->GetYaxis()) {
    if(yTitle)mg->GetYaxis()->SetTitle(yTitle);
  }    
 // if(mg->GetHistogram())
   //  mg->GetHistogram()->SetTitle(plotTitle);  
  if(!gLatex) gLatex = new TLatex(0,0.9,"dummy");
  gLatex->DrawTextNDC(0.05,0.95,plotTitle);
  
  return mg;
  
}

int AraPlotUtils::updateTouchFile(char *touchFile, UInt_t unixTime)
{
   //Touch File  
   struct utimbuf ut;
   ut.actime=unixTime;  
   ut.modtime=unixTime;   
   struct stat buf;  
   int retVal2=stat(touchFile,&buf);  
   if(retVal2==0) {    
      if(buf.st_mtime<ut.modtime) {      
	 utime(touchFile,&ut);      
	 return 1;
      }  
   }  
   else {
      //Maybe file doesn't exist    
      std::ofstream Touch(touchFile);    
      Touch.close();    
      utime(touchFile,&ut);    
      return 1;
   }      
   return 0;
}
