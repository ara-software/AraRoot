//////////////////////////////////////////////////////////////////////////////
/////  AraDisplayConventions.h        Ara Display Conventions        /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A handy file full of enumerations and the like identifying     /////
/////     the conventions we use in Ara Display                        /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////


#ifndef ARADISPLAYCONVENTIONS_H
#define ARADISPLAYCONVENTIONS_H

#ifndef ROOT_Rtypes
#if !defined(__CINT__) || defined(__MAKECINT__)
#include "Rtypes.h"
#endif
#endif


//!  Waveform display format option
/*!
  A simple enumeration to enumerate the different waveform display options for AraDisplay (eg. waveform, PSD (FFT), Hilbert envelope, etc.)
*/
namespace AraDisplayFormatOption {
  typedef enum EAraDisplayFormatOption {
    kWaveform = 0, ///< View of waveforms
    kPowerSpectralDensity = 1, ///< View of FFTs
    kFFT = 1, ///< View of FFTs
    kHilbertEnvelope, ///< View of hilbert envelopes
    kAveragedFFT ///< View of avergaed FFTs
  } AraDisplayFormatOption_t; ///< Enumeration for plot style.
};

    

namespace AraDisplayCanvasLayoutOption {
  typedef enum EAraDisplayCanvasLayoutOption{
    kElectronicsView = 0,
    kAntennaView = 1,
    kRFChanView = 2,
    kIntMapView = 3
  } AraDisplayCanvasLayoutOption_t;
}

int getNiceColour(int ind); ///< Simple fucntion to return some of my favourite colours.
int getMarker(int ind);///< Returns a maker number

#endif //ARADISPLAYCONVENTIONS_H
