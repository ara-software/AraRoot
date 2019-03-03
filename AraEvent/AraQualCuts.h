//////////////////////////////////////////////////////////////////////////////
/////  AraQualCuts.h       ARA Quality cuts tool                         /////
/////                                                                    /////
/////  Description:                                                      /////
/////     The Ara class imposing some event qualtiy cuts                 /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ARAQUALCUTS_H
#define ARAQUALCUTS_H

//Includes
#include "RawAtriStationEvent.h"
#include "UsefulAtriStationEvent.h"

//! Part of AraEvent library. Can report on if there is a quality cut problem with an event
/*!
  The Ara event quality cuts tool
  \ingroup rootclasses
*/
class AraQualCuts
{
  public:
    AraQualCuts(); ///< Default constructor
    ~AraQualCuts(); ///< Destructor

    //Instance generator
    static AraQualCuts*  Instance();

    bool isGoodEvent(UsefulAtriStationEvent *realEvent);

    bool hasBlockGap(RawAtriStationEvent *rawEvent); ///< Detects block gaps
    bool hasTimingError(UsefulAtriStationEvent *realEvent); ///< Detects timing errors
    bool hasTooFewBlocks(UsefulAtriStationEvent *realEvent); ///< Detects too few block cases
    bool hasA2FirstEventCorruption(RawAtriStationEvent *rawEvent); ///<Checks for first four event corruption in A2

    /*
      the cut variables for the offset blocks cut

      these cut variables are very specifically *not* made static
      (so they are initialized in the constructor for c++<11 compatibility)
      so that a user can go in and tune them
      e.g. AraQualCuts *qual->_VdeltaT=0.5; or qual->_NumOffsetBlocksCut=10;
    */
    double _VdeltaT; ///< the recommended interpolation factors for vpol waveforms (at the time of writing)
    double _HdeltaT; ///< the recommended interpolation factors for hpol waveforms (at the time of writing)
    double _VOffsetThresh; ///< the offset which will trigger a "bad block" in vpol
    double _HOffsetThresh; ///< the offset which will trigger a "bad block" in hpol
    int _NumOffsetBlocksCut; ///< numer of channels which must have an offset block to qualify the event as bad
    int _OffsetBlocksTimeWindowCut; ///< the coincidence window for offset blocks
    bool hasOffsetBlocks(UsefulAtriStationEvent *realEvent); ///< Detects offset blocks
  
  protected:
    static AraQualCuts *fgInstance; // protect against multiple instances
    
  private:
    double getMax(TGraph *gr, double *maxTime); ///< gets the max of waveform
    double getMean(TGraph *gr); ///< gets the mean of a waveform
    TGraph* getRollingMean(TGraph *grInt, int samplePerBlock); ///< gets the rolling average of a waveform

};

#endif //ARAQUALCUTS_H
