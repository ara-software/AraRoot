//////////////////////////////////////////////////////////////////////////////
/////  AraGeomTool.h       ARA Geometry tool                             /////
/////                                                                    /////
/////  Description:                                                      /////
/////     The Ara class working out what is where                        /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "AraGeomTool.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <zlib.h>

#include <cstdlib>

#include "TString.h"
#include "TObjArray.h"
#include "TObjString.h"

AraGeomTool * AraGeomTool::fgInstance=0;
Double_t AraGeomTool::nTopOfIce=1.48;

AraGeomTool::AraGeomTool() 
{
   //Default Constructor
  readChannelMap();
}

AraGeomTool::~AraGeomTool() {
   //Default Destructor
}

//______________________________________________________________________________
AraGeomTool*  AraGeomTool::Instance()
{
  //static function
  if(fgInstance)
    return fgInstance;

  fgInstance = new AraGeomTool();
  return fgInstance;
}

void AraGeomTool::readChannelMap()
{
  char calibDir[FILENAME_MAX];
  char fileName[FILENAME_MAX];
  char *calibEnv=getenv("ARA_CALIB_DIR");
  if(!calibEnv) {
     char *utilEnv=getenv("ARA_UTIL_INSTALL_DIR");
     if(!utilEnv)
        sprintf(calibDir,"calib");
     else
        sprintf(calibDir,"%s/share/araCalib",utilEnv);
  }
  else {
    strncpy(calibDir,calibEnv,FILENAME_MAX);
  }  

  sprintf(fileName,"%s/TestBed.csv",calibDir);
  std::ifstream IcrrMap(fileName);
  if(!IcrrMap) {
    std::cerr << "Couldn't open:\t" << fileName << " don't know where anything is.\n";
    return;
  }

  //Now we can try and read the file and parse it something useful
  TString line;
  line.ReadLine(IcrrMap); ///<Ths is the header line
  //  std::cout << line.Data() << "\n";
  line.ReadLine(IcrrMap); ///<Ths is the second header line
  //  std::cout << line.Data() << "\n";
  for(int ant=0;ant<ANTS_PER_ICRR;ant++) {
    //Loop over the antenna lines;
    line.ReadLine(IcrrMap);
    //    std::cout << line.Data();
    TObjArray *tokens = line.Tokenize(",");
    //    std::cout << tokens->GetEntries() << "\n"; 
    for(int j=0;j<tokens->GetEntries();j++) {
      const TString subString = ((TObjString*)tokens->At(j))->GetString();
      //      std::cout << subString << "\n";
      switch(j) {
      case 0:
	//Channel number
	fAntInfo[ant].antDir=AraAntDir::kReceiver;
	fAntInfo[ant].chanNum=subString.Atoi();
	break;
      case 1:
	//DAQ box channel
	if(subString.BeginsWith("DIS")) {
	  fAntInfo[ant].daqChanType=AraDaqChanType::kDisconeChan;
	  fAntInfo[ant].daqChanNum=atoi(&subString.Data()[3]);
	}
	else if(subString.BeginsWith("BAT")) {
	  fAntInfo[ant].daqChanType=AraDaqChanType::kBatwingChan;
	  fAntInfo[ant].daqChanNum=atoi(&subString.Data()[3]);
	}

	break;
      case 2:
	//High pass filter
	fAntInfo[ant].highPassFilterMhz=subString.Atof();
	break;
      case 3:
	//Low pass filter
	fAntInfo[ant].lowPassFilterMhz=subString.Atof();
	break;
      case 4:
	//ICRR wfm channel
	break;
      case 5:
	//ICRR trig channel
	if(subString.BeginsWith("RF_TRIG_DIS")) {
	  fAntInfo[ant].daqTrigChan=atoi(&subString.Data()[11]);
	}
	else if(subString.BeginsWith("RF_TRIG_BAT")) {
	  
	  fAntInfo[ant].daqTrigChan=atoi(&subString.Data()[11]);
	}
	//	std::cout << ant << "\t" << fAntInfo[ant].daqTrigChan << "\n";
	break;
      case 6:
	//Lab chip and chan(s)
	//	std::cout << subString.Data()   << "\n";
	if(subString.Contains("+")) {
	  //Two Lab Chans
	  fAntInfo[ant].numLabChans=2;
	  if(subString.Contains("A")) {
	    fAntInfo[ant].labChip=AraLabChip::kA;
	  }
	  else if(subString.Contains("B")) {
	    fAntInfo[ant].labChip=AraLabChip::kB;
	  }
	  else if(subString.Contains("C")) {
	    fAntInfo[ant].labChip=AraLabChip::kC;
	  }
	  char test[2]="0";
	  test[0]=subString.Data()[2];
	  fAntInfo[ant].labChans[0]=atoi(test)-1;
	  test[0]=subString.Data()[5];
	  fAntInfo[ant].labChans[1]=atoi(test)-1;  
	  //	  std::cout << ant << "\t" << fAntInfo[ant].labChans[0] << "\t" << fAntInfo[ant].labChans[1] << "\n";
	}
	else {
	  //One lab chan
	  fAntInfo[ant].numLabChans=1;
	  if(subString.Contains("A")) {
	    fAntInfo[ant].labChip=AraLabChip::kA;
	  }
	  else if(subString.Contains("B")) {
	    fAntInfo[ant].labChip=AraLabChip::kB;
	  }
	  else if(subString.Contains("C")) {
	    fAntInfo[ant].labChip=AraLabChip::kC;
	  }
	  char test[2]="0";
	  test[0]=subString.Data()[1];
	  fAntInfo[ant].labChans[0]=atoi(test)-1;
	}	
	break;
      case 7:
	//	std::cout << subString.Data()   << "\n";
	//Pre amp number
	fAntInfo[ant].preAmpNum=subString.Atoi();
	break;
      case 8:
	//	std::cout << subString.Data()   << "\n";
	//avg noise figure
	fAntInfo[ant].avgNoiseFigure=subString.Atof();
	break;
      case 9:
	//	std::cout << subString.Data()   << "\n";
	//RCVR number
	fAntInfo[ant].rcvrNum=subString.Atoi();
	break;
      case 10:
	//	std::cout << subString.Data()   << "\n";
	//Designator
	strncpy(fAntInfo[ant].designator,subString.Data(),3);
	{
	  char test[2]="0";
	  test[0]=subString.Data()[1];
	  fAntInfo[ant].antPolNum=atoi(test)-1;
	  //	  std::cout << fAntInfo[ant].designator << "\t" << fAntInfo[ant].antPolNum << "\n";
	}
	break;
      case 11:
	//Channel Type -- got already
	break;
      case 12:
	//Antenna Type
	//	std::cout << subString.Data() << "\n";
	if(subString.BeginsWith("Bicone"))
	  fAntInfo[ant].antType=AraAntType::kBicone;
	else if(subString.BeginsWith("Bowtie-slotted"))
	  fAntInfo[ant].antType=AraAntType::kBowtieSlot;
	else if(subString.BeginsWith("Discone"))
	  fAntInfo[ant].antType=AraAntType::kDiscone;
	else if(subString.BeginsWith("Batwing"))
	  fAntInfo[ant].antType=AraAntType::kBatwing;
	else if(subString.BeginsWith("Fat"))
	  fAntInfo[ant].antType=AraAntType::kFatDipole;
	else if(subString.BeginsWith("Quad"))
	  fAntInfo[ant].antType=AraAntType::kQuadSlot;
	
	break;
      case 13:
	//Polarisation
	//	std::cout << subString.Data() << "\n";
	if(subString.BeginsWith("V"))
	  fAntInfo[ant].polType=AraAntPol::kVertical;
	if(subString.BeginsWith("H"))
	  fAntInfo[ant].polType=AraAntPol::kHorizontal;
	break;
      case 14:
	//Location name
	//	std::cout << subString.Data() << "\n";
	strncpy(fAntInfo[ant].locationName,subString.Data(),4);
	break;
      case 15:
	//xPos
	fAntInfo[ant].antLocation[0]=subString.Atof();
	break;
      case 16:
	//xPos
	fAntInfo[ant].antLocation[1]=subString.Atof();
	break;
      case 17:
	//xPos
	fAntInfo[ant].antLocation[2]=subString.Atof();
	break;
      case 18:
	//cable delay
	fAntInfo[ant].cableDelay=subString.Atof();
	break;
      case 19:
	//Stuff added by Brendan in rev10
	fAntInfo[ant].debugHolePosition[0]=subString.Atof(); ////< x,y,z in m
	break;
      case 20:
	//Stuff added by Brendan in rev10
	fAntInfo[ant].debugHolePosition[1]=subString.Atof(); ////< x,y,z in m
	break;
      case 21:
	//Stuff added by Brendan in rev10
	fAntInfo[ant].debugHolePosition[2]=subString.Atof(); ////< x,y,z in m
	break;
      case 22:
	//Stuff added by Brendan in rev10
	fAntInfo[ant].debugPreAmpDz=subString.Atof(); ///< in m
	break;
      case 23:
	//Stuff added by Brendan in rev10	
	fAntInfo[ant].debugHolePositionZft=subString.Atof(); ///< in ft
	break;
      case 24:
	//Stuff added by Brendan in rev10
	fAntInfo[ant].debugHolePositionZm=subString.Atof(); ///< in m
	break;
      case 25:
	//Stuff added by Brendan in rev10
	fAntInfo[ant].debugTrueAsBuiltPositon[0]=subString.Atof(); ///< x,y,z in m
	break;
      case 26:
	//Stuff added by Brendan in rev10
	fAntInfo[ant].debugTrueAsBuiltPositon[1]=subString.Atof(); ///< x,y,z in m
	break;
      case 27:
	//Stuff added by Brendan in rev10
	fAntInfo[ant].debugTrueAsBuiltPositon[2]=subString.Atof(); ///< x,y,z in m
	break;
      case 28:
	//Stuff added by Brendan in rev10
	fAntInfo[ant].debugCableDelay2=subString.Atof(); //in ns
	break;
      case 29:
	//Stuff added by Brendan in rev10
	fAntInfo[ant].debugFeedPointDelay=subString.Atof(); //in ns
	break;
      case 30:
	fAntInfo[ant].debugTotalCableDelay=subString.Atof(); //in ns
	break;
      case 31:
	//Comments including ant orientation
	//	std::cout << subString.Data() << "\n";
	if(subString.BeginsWith("oriented EW")) {
	  fAntInfo[ant].polType=AraAntPol::kSurface;
	  fAntInfo[ant].antOrient=AraSurfaceOrientation::kEastWest;
	}
      default:
	break;
      }
      
    }
    //    fAntInfo[ant].printAntennaInfo();
    switch(fAntInfo[ant].polType) {
    case AraAntPol::kVertical:
      fAntLookupTable[0][fAntInfo[ant].antPolNum]=fAntInfo[ant].chanNum-1;
      break;
    case AraAntPol::kHorizontal:
      fAntLookupTable[1][fAntInfo[ant].antPolNum]=fAntInfo[ant].chanNum-1;
      break;
    case AraAntPol::kSurface:
      fAntLookupTable[2][fAntInfo[ant].antPolNum]=fAntInfo[ant].chanNum-1;
      break;
    default:
      std::cerr << "Unknown AraPolType\n";
    }
  }
}

//jd
int AraGeomTool::getRFChanByPolAndAnt(AraAntPol::AraAntPol_t antPol, int antNum)
{
  if(antNum<8 && antNum>=0)
    return fAntLookupTable[antPol][antNum];
  return -1;

}



Double_t AraGeomTool::calcDeltaTInfinity(Double_t ant1[3], Double_t ant2[3],Double_t phiWave, Double_t thetaWave)
{
  //Calc some cylindrical coordinates
  Double_t rho1=TMath::Sqrt(ant1[0]*ant1[0]+ant1[1]*ant1[1]);
  Double_t phi1=TMath::ATan2(ant1[1],ant1[0]);
  Double_t rho2=TMath::Sqrt(ant2[0]*ant2[0]+ant2[1]*ant2[1]);
  Double_t phi2=TMath::ATan2(ant2[1],ant2[0]);
  Double_t d1=TMath::Cos(thetaWave)*(ant1[2]*TMath::Tan(thetaWave)+rho1*TMath::Cos(phi1-phiWave));
  Double_t d2=TMath::Cos(thetaWave)*(ant2[2]*TMath::Tan(thetaWave)+rho2*TMath::Cos(phi2-phiWave));
  Double_t t1t2=(d2-d1)*nTopOfIce/TMath::C();
  t1t2*=1e9;
  return t1t2;

}

//jd
Double_t AraGeomTool::calcDeltaTInfinity(Int_t chan1, Int_t chan2,Double_t phiWave, Double_t thetaWave)
{
  if(chan1<0 || chan1>=TOTAL_ANTS_PER_ICRR)
    return 0;
  if(chan2<0 || chan2>=TOTAL_ANTS_PER_ICRR)
    return 0;
  return calcDeltaTInfinity(fAntInfo[chan1].antLocation,fAntInfo[chan2].antLocation,phiWave,thetaWave);            
}

Double_t AraGeomTool::calcDeltaTR(Double_t ant1[3], Double_t ant2[3], Double_t phiWave, Double_t thetaWave,Double_t R)
{

  Double_t xs=R*TMath::Cos(thetaWave)*TMath::Cos(phiWave);
  Double_t ys=R*TMath::Cos(thetaWave)*TMath::Sin(phiWave);
  Double_t zs=R*TMath::Sin(thetaWave);

  
  Double_t d1=TMath::Sqrt((xs-ant1[0])*(xs-ant1[0])+(ys-ant1[1])*(ys-ant1[1])+(zs-ant1[2])*(zs-ant1[2]));
  Double_t d2=TMath::Sqrt((xs-ant2[0])*(xs-ant2[0])+(ys-ant2[1])*(ys-ant2[1])+(zs-ant2[2])*(zs-ant2[2]));
         
  Double_t t1t2=(d1-d2)*nTopOfIce/TMath::C();
  t1t2*=1e9;
  return t1t2;

}

//jd
Double_t AraGeomTool::calcDeltaTR(Int_t chan1, Int_t chan2, Double_t phiWave, Double_t thetaWave,Double_t R)
{
  if(chan1<0 || chan1>=TOTAL_ANTS_PER_ICRR)
    return 0;
  if(chan2<0 || chan2>=TOTAL_ANTS_PER_ICRR)
    return 0;
  return calcDeltaTR(fAntInfo[chan1].antLocation,fAntInfo[chan2].antLocation,phiWave,thetaWave,R);   

}

