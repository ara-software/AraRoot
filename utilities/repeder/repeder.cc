
#include <iostream> 
#include <fstream> 
#include "TH2.h" 
#include "TFile.h" 
#include "TTree.h" 
#include "RawAtriStationEvent.h" 
#include "TGraphErrors.h" 
#include "araSoft.h" 

/** program to recalculate pedestals for ATRI  from data*/ 


const int nchan = CHANNELS_PER_ATRI; 
const int nblk = BLOCKS_PER_DDA; 
const int nsamp = BLOCKS_PER_DDA * SAMPLES_PER_BLOCK; 
int min_adu = 2048-512; 
int max_adu = 2048+512; 
int adu_bin = 1; 

const int samp_per_block = SAMPLES_PER_BLOCK; 
const int chan_per_dda = RFCHAN_PER_DDA; 
const int dda_per_atri = DDA_PER_ATRI; 

void usage() 
{
  std::cerr << "Usage: repeder input_file.root output_pedestal_file.dat [output_file.root] [hist_channel_mask = 0x1] [min_adu=1536] [max_adu=2560] [adu_bin=1]" << std::endl; 
}


int main (int nargs, char ** args) 
{

  if (nargs < 3) 
  {
    usage(); 
    return 1; 
  }

  TFile *  infile = TFile::Open(args[1]); 

  if (!infile) 
  {
    std::cerr << "Problem opening " << args[1] << std::endl; 
    return 1; 

  }
  TTree *t = (TTree*) infile->Get("eventTree"); 

  if (!t) 
  {
    std::cerr << "Could not find eventTree in " << args[1] << std::endl; 
    return 1; 
  }

  bool do_full_hists = nargs > 3; 
  TH2 * full_hists[nchan] ;  
  TFile * full_hists_file = 0; 

  if (do_full_hists) 
  {
    int hist_mask = 1; 
    if (nargs > 4) hist_mask = strtol(args[4], 0, 0); 
    if (nargs > 5) min_adu = strtol(args[5],0,0); 
    if (nargs > 6) max_adu = strtol(args[6],0,0); 
    if (nargs > 7) adu_bin = strtol(args[7],0,0); 

    full_hists_file = new TFile(args[3],"RECREATE"); 
    for (int i = 0; i < nchan; i++) 
    {
      if ( hist_mask  & (1 << i) ) 
      {
        full_hists[i] = new TH2S(Form("samp_hist_ch%d", i), 
                                 Form("Sample Histogram Channel %d", i), nsamp, 0, nsamp, 
                                 (max_adu-min_adu+1)/adu_bin, min_adu, max_adu); 
                               
      }
      else full_hists[i] = 0; 
    }
  }

  std::vector<std::vector<double> > sum(nchan, std::vector<double> ( nsamp,0)); 
  std::vector<std::vector<double> > sum2(nchan, std::vector<double> ( nsamp,0)) ; 
  std::vector<std::vector<int > >num(nchan, std::vector<int> ( nsamp,0));

  RawAtriStationEvent * ev = 0; 
  t->SetBranchAddress("event",&ev); 
  t->SetCacheSize(100*1000*1000); 
  t->AddBranchToCache("event",true); 

  int nev = t->GetEntries(); 
  int nhundred = 0; 

  for (int iev = 0; iev < t->GetEntries(); iev++) 
  {
    t->GetEntry(iev); 
    if (iev >= nhundred*100)
    {
      std::cout << iev << "/"  <<  nev << "\r"; 
      std::cout << std::flush; 
      nhundred++; 
    }

    for (unsigned iblk = 0; iblk < ev->blockVec.size(); iblk++) 
    {

      int chan_idx = 0; 
      for (int ich= 0; ich< ev->blockVec[iblk].getNumChannels(); ich++) 
      {
        if (ev->blockVec[iblk].channelMask && (1 << ich) == 0) continue; 
        int chan= chan_per_dda *ev->blockVec[iblk].getDda() + ich; 
        int offset = ev->blockVec[iblk].getBlock() * samp_per_block; 
        for (unsigned isamp = 0; isamp < ev->blockVec[iblk].data[chan_idx].size(); isamp++) 
        {
          UShort_t val = ev->blockVec[iblk].data[chan_idx][isamp]; 
          unsigned i = (offset+isamp) % nsamp; 
          sum2[chan][i] += val*val; 
          sum[chan][i] += val; 
          num[chan][i] ++; 

          if (full_hists[chan]) 
          {
            full_hists[chan]->Fill(i,val); 
          }
        }

        chan_idx++; 
      }
    }
  }

  std::cout << std::endl; 

  //write out pedestal file

  std::ofstream pf(args[2]); 

  for (int ich = 0; ich < nchan; ich++) 
  {
    for (int blk = 0; blk < nblk; blk++) 

    {
      pf <<  ich / dda_per_atri <<  " " << blk << " " <<  ich % chan_per_dda ; 

      for (int isamp = 0; isamp < samp_per_block; isamp++) 
      {
        pf << " " <<  round( sum[ich][isamp+blk*samp_per_block] / num[ich][isamp+blk*samp_per_block]); 
      }

      pf << std::endl; 
    }
  }



  if (do_full_hists) 
  {
    full_hists_file->cd(); 
    for (int ih = 0; ih < nchan; ih++) 
    {
      if (full_hists[ih]) full_hists[ih]->Write(); 
    }

    for (int ich; ich < nchan; ich++) 
    {
      TGraphErrors * g = new TGraphErrors(nsamp); 

      for (int isamp = 0; isamp < nsamp; isamp++) 
      {
        g->SetPoint(isamp,isamp, sum[ich][isamp]/ num[ich][isamp]); 
        g->SetPointError(isamp, 0, sqrt(sum2[ich][isamp]/ num[ich][isamp] - g->GetY()[isamp]*g->GetY()[isamp] )); 
      }
      g->SetTitle(Form("Channel %d",ich)); 
      g->Write(Form("g_ch%d",ich)); 
      delete g; 
    }
  }

  delete full_hists_file; 
}
