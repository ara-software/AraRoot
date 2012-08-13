#ifndef ARAFFTGRAPH_H
#define ARAFFTGRAPH_H
#include "TGraph.h"


class TObject;
class TGaxis;

//!  Part of AraDisplay library. The PSD display graph class that inherits from ROOT's TGraph.
/*!
  The AraFFTGraph is one of the things that makes AraDisplay purr. It's really quite simple but allows for the fucntionality to click on a graph and have it draw in a new window.
*/
class AraFFTGraph : public TGraph
{

 public:
   AraFFTGraph();  ///< Constructor
   AraFFTGraph(int n, const Int_t *x, const Int_t *y); ///< Int_t constructor
   AraFFTGraph(int n, const Float_t *x, const Float_t *y); ///< Float_t constructor
   AraFFTGraph(int n, const Double_t *x, const Double_t *y); ///< Double_t constructor
   int AddFFT(AraFFTGraph *otherGraph); ///<Adds an FFT to an average
   virtual ~AraFFTGraph(); ///< Destructor
  


 private:
  Int_t fSurf; ///< Used in labelling.
  Int_t fChan; ///< Used in labelling.
  Int_t fPhi; ///< Used in labelling.
  Int_t fAnt; ///< Used in labelling.


  void ExecuteEvent(Int_t event, Int_t px, Int_t py); ///< ROOT function that is called when a user interacts with the graph using the mouse.
  void ClearSelection(void); ///< ROOT thingy.
  void ApplySelectionToButtons(); ///< ROOT thingy.
  void drawInNewCanvas(); ///< Draws a copy waveform in a new canvas.

  Int_t fNewCanvas; ///< Flag to dictate whether the waveform is a copy drawn in a new canvas (if it is then the title is shown).
  Int_t fNumInAverage; ///<Counts number of FFTs in current rolling average
  
  
  ClassDef(AraFFTGraph,2)
};                              // end of class AraFFTGraph


#endif  // ARAFFTGRAPH_H
