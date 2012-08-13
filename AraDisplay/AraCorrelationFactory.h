//////////////////////////////////////////////////////////////////////////////
/////  AraCorrelationFactory.h       The correlation factory                /////
/////                                                                    /////
/////  Description:                                                      /////
/////                                                                    ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ARACORRELATIONFACTORY_H
#define ARACORRELATIONFACTORY_H

//Includes
#include <iostream>
#include <TObject.h>
#include <TMath.h>
#include <TVector3.h>

class AraWaveformGraph;


//!  Part of AraDisplay library. The correlation factory which takes two waveforms and correlates them
/*!
  It's a factory the produces correlations.
*/
class AraCorrelationFactory 
{
 public:
   AraCorrelationFactory(); ///<Constructor
   ~AraCorrelationFactory(); ///<Destructor

  //Instance generator
   static AraCorrelationFactory*  Instance(); ///<Instance generator
 
   void addWaveformToCorrelation(AraWaveformGraph *inputWave); ///< Adds a a waveform to the correlation

   void setCorrelationDeltaT(Double_t deltaT) {fDeltaT=deltaT;} ///< Sets the deltaT for the correlation.
   Double_t getCorrelationDeltaT() { return fDeltaT;} ///<Returns the deltaT used in correlation.


   AraWaveformGraph *fGrWave[2]; ///<The input WaveformGraph's
   Double_t fDeltaT; ///< The deltaT used in the correlation.

 protected:
   static AraCorrelationFactory *fgInstance; ///< Protect against multiple instances.
 
};


#endif //CORRELATIONFACTORY_H
