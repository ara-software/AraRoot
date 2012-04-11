#ifndef ARAWAVEFORMGRAPH_H
#define ARAWAVEFORMGRAPH_H
#include "TGraph.h"
#include "TH1.h"


class TObject;
class TGaxis;

//!  The graph class that inherits from ROOT's TGraph.
/*!
  The AraWaveformGraph is one of the things that makes AraDisplay purr. It's really quite simple but allows for the fucntionality to click on a graph and have it draw in a new window, and also allows the right click DrawFFT option.
*/
class AraWaveformGraph : public TGraph
{

 public:
   AraWaveformGraph(); ///< Constructor.
   AraWaveformGraph(int n, const Int_t *x, const Int_t *y); ///< Int_t constructor.
   AraWaveformGraph(int n, const Float_t *x, const Float_t *y); ///< Float_t constructor.
   AraWaveformGraph(int n, const Double_t *x, const Double_t *y); ///< Double_t constructor.
   virtual ~AraWaveformGraph(); ///< Destructor.
  
   //!Draws the PSD of the waveform in a new window (NB: It is the *MENU* desigination that ROOT uses to add it to the right click list).   
  void DrawFFT(); // *MENU*   
  void DrawHilbert(); // *MENU* 
  void AddToCorrelation(); // *MENU*
  TGraph *getFFT(); ///< Returns a pointer to a TGraph containing the PSD of the waveform.
  TH1D *getFFTHisto(); ///< Returns a pointer to a TH1D containing the PSD of the waveform
  TGraph *getHilbert(); ///< Returns a pointer to a TGraph containing the hilbert envelope of the waveform.

  void setElecChan(int elecChan);
  Int_t fElecChan; ///< Used in labelling.
  void setRFChan(int rfChan, int stationId);
  Int_t fRFChan; ///< Used in labelling.

 private:


  void ExecuteEvent(Int_t event, Int_t px, Int_t py); ///< ROOT function that is called when a user interacts with the graph using the mouse.
  void ClearSelection(void); ///< ROOT thingy.
  void ApplySelectionToButtons(); ///< ROOT thingy.
  void drawInNewCanvas(); ///< Draws a copy waveform in a new canvas.

  Int_t fNewCanvas; ///< Flag to dictate whether the waveform is a copy drawn in a new canvas (if it is then the title is shown).
  
  
  ClassDef(AraWaveformGraph,2) ///< ROOT macro for persistence. 
};                              // end of class AraWaveformGraph


#endif  // ARAWAVEFORMGRAPH_H
