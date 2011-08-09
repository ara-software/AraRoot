//////////////////////////////////////////////////////////////////////////////
/////  AraControlPanel.h        Magic Display Control Panel            /////
/////                                                                    /////
/////  Description:                                                      /////
/////                                                                    ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef MAGICCONTROLPANEL_H
#define MAGICCONTROLPANEL_H


#include <TGResourcePool.h>
#include <TGListBox.h>
#include <TGListTree.h>
#include <TGFSContainer.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TGIcon.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGNumberEntry.h>
#include <TGMsgBox.h>
#include <TGMenu.h>
#include <TGCanvas.h>
#include <TGComboBox.h>
#include <TGTab.h>
#include <TGSlider.h>
#include <TGDoubleSlider.h>
#include <TGFileDialog.h>
#include <TGTextEdit.h>
#include <TGShutter.h>
#include <TGProgressBar.h>
#include <TGColorSelect.h>
#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TH1.h>
#include <TH2.h>
#include <TRandom.h>
#include <TSystem.h>
#include <TSystemDirectory.h>
#include <TEnv.h>
#include <TFile.h>
#include <TKey.h>
#include <TGDockableFrame.h>
#include <TGFontDialog.h>



//!  The Magic Display control panel
/*!
  This is the horribly buggy control panel that was meant to enable easy control of magic Display, without resorting to command line power user mode. Unfortunately, it doesn't yet really work in a non segmentation violation frenzy way. I blame ROOT, others may have different opinions.
*/
class AraControlPanel 
{
 
 public:


   AraControlPanel(); ///< The default constructor
   virtual ~AraControlPanel(); ///< The destructor

   //   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t); ///< ROOT underlying code fragment

  

  //Instance generator
   static AraControlPanel*  Instance(); ///< The instance generator (there should only be one instance of the AraControlPanel at a time
   
   void goToEvent();
   void closeControlPanel();
  

 protected:
   static AraControlPanel *fgInstance;   ///< Protect against multiple instances

   TGMainFrame     *fMainFrame; ///< Main frame
   TGTextButton    *fGotoBut; ///< Text button.

   TGCheckButton   *fEventBut; ///< Check button
   TGCheckButton   *fSurfBut; ///< Check button
   TGCheckButton   *fTurfBut; ///< Check button
   TGCheckButton   *fAvgSurfBut; ///< Check button
   TGCheckButton   *fSumTurfBut; ///< Check button
   TGLayoutHints   *fLayout; ///< Layout nonsense
   TGLayoutHints   *fLeftLayout; ///< Layout nonsense
   TGLayoutHints   *fRightLayout; ///< Layout nonsense
   TGLayoutHints   *fButtonLayout; ///< Layout nonsense
   TGLayoutHints   *fCenterLayout; ///< Layout nonsense
   TGNumberEntry   *fEventEntry; ///< Number entry form
   TGLabel         *fEventLabel; ///< It's a label
   TGLabel         *fUpdateLabel; ///< It's a label

   TGVerticalFrame *fEntryPanel; ///< A vertical frame
   TGHorizontalFrame *fEventPanel; ///< A horizontal frame
   TGVerticalFrame *fButtonPanel; ///< A vertical frame

   ClassDef(AraControlPanel,1); ///< One of the magic ROOT macros.
};


#endif //MAGICCONTROLPANEL_H
