#include <TTimeStamp.h>
 



class TIMESTAMP {
 public:
  TIMESTAMP(){}
  TIMESTAMP(unsigned int unixTime) {TTimeStamp* ts=new TTimeStamp(unixTime); epoch=unixTime; DOY=ts->GetDayOfYear(); int dat=ts->GetTime(); hour=(int) dat/10000; minute=(int) dat/100 - hour*100; second=dat-hour*10000-minute*100; DOW=ts->GetDayOfWeek();
    printf ("dat=%i %i \n",dat,unixTime);

};
  ~TIMESTAMP(){};
  unsigned int epoch;
  Int_t DOY;
  Int_t hour;
  Int_t minute;
  Int_t second;
  Int_t DOW;

};

typedef struct {
  Double_t FitPoint[4];
  Double_t FitError[4];
  Double_t vertexfindervtx[3];
  Int_t nhits;
  Double_t chisqvtx;
  Float_t Rxtresid[16];
  Float_t deltaTime[16];
  Float_t convergence[2];



} RECO;
 
typedef struct {
  Int_t stationId;
  Int_t channelId;
  Double_t Location[3];
  Double_t HighPassFilter;
  Double_t LowPassFilter;
  double cableDelay;
  Int_t antPol;
  Int_t antType;
  Double_t antOrient[3];
  Double_t averageNoiseFigure;
} ANTENNA;

typedef struct {
  Double_t mean[ANTS_PER_ICRR];
  Double_t rms[ANTS_PER_ICRR];
  Int_t isInTrigPattern[ANTS_PER_ICRR];
} WF;


 typedef struct {
   Int_t RunNumber;
   TIMESTAMP RunStartTime;
   TIMESTAMP RunEndTime;
   Int_t RunDuration;
   Int_t ErrorFlag;
   Int_t RunType;
   Int_t NumberOfEvents;
   Int_t NumberOfRFEvents;
   Int_t NumberOfPulserEvents;
   Int_t NumberOfForcedEvents;
   Int_t NumberOfUnknownEvents;
   Int_t Nall;
 } RUNHEADER;


typedef struct {
  Double_t RbClock;  // Rubidium time in seconds. (divided by 280M)
  Double_t DeadTime; // Dead time as a fraction
  Int_t TriggerType;//
  Int_t EventType; //
  Int_t TriggerPattern; //
} TRIGGER;

typedef struct {
TIMESTAMP unixTime;   //-i
unsigned int unixTimeusec;//-i
unsigned int eventNumber;
int gpsSubTime;
UShort_t calibStatus;
UChar_t priority;
UChar_t errorFlag;
} HEADER;

typedef struct {
  Double_t temperature[8];
  Double_t RFPower[16];
  UShort_t sclGlobal; 
  UShort_t sclL1[12];  
  UShort_t scl[24];
} HK;

  typedef struct {
    Int_t hour,minute,sec,usec,dayoy;
    UInt_t time; 
    Float_t dt,dtEvent;
    Double_t rbtime;    
  } TIME;

