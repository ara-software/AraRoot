
#include <iostream>
#include <fstream>
#include "TH2.h"
#include "TFile.h"
#include "RawAtriStationEvent.h"
#include "TGraphErrors.h"
#include "araSoft.h"
#include "TChain.h"
#include "TROOT.h"

/** program to recalculate pedestals for ATRI  from data*/


const int nchan = CHANNELS_PER_ATRI;
const int nblk = BLOCKS_PER_DDA;
const int nsamp = BLOCKS_PER_DDA * SAMPLES_PER_BLOCK;
const int samp_per_block = SAMPLES_PER_BLOCK;
const int chan_per_dda = RFCHAN_PER_DDA;
const int dda_per_atri = DDA_PER_ATRI;


int min_adu = 1750-512;
int max_adu = 1750+512;
int adu_bin = 1;
int n_adu_bins = 0;
unsigned hist_mask = 0x0f0f0f0f;

Long64_t max = -1;

const char * input_file = 0;
const char * pedestal_file = 0;
bool use_median = false;
const char * root_output = 0;
const char * qual_file = 0;

bool use_calpulsers = false;
int cache_size = 100;

TChain chain("eventTree");

void usage()
{
  std::cout << "Usage: repeder input_file.root [intput_file2.root ...]  output_pedestal_file.dat " << std::endl
            << "      [-d] [-h] [-o output_file.root] [-x hist_channel_mask=0x0f0f0f0f] " << std::endl
            << "      [-p] [-N num_events] [-C cache_size=100] [-t num_threads] " << std::endl
            <<"       [-m min_hist_adu="<< min_adu << "] [-M max_hist_adu=" << max_adu <<" ] [-b hist_adu_bin="<< adu_bin <<"]" << std::endl;
  std::cout << "-h :  Display this message" << std::endl;
  std::cout << "-d :  Use median instead of mean (for channels defined in hist mask only)" << std::endl;
  std::cout << "-o :  Auxilliary ROOT output. Will contain histograms for channels in hist mask and also mean/rms graphs. " << std::endl;
  std::cout << "-x :  Histogram mask. Has no effect if neither -o nor -d are defined. " << std::endl;
  std::cout << "-p :  Include events marked as calpulsers. Default is to exclude. " << std::endl;
  std::cout << "-N :  Only process up to event N" << std::endl;
  std::cout << "-t :  Enable multithreading (in TTree reading). Specify number of threads (or 0 to choose automatically) " << std::endl;
  std::cout << "-C :  Size in megabytes of TTreeCache" << std::endl;
  std::cout << "-m,-M,-b :   Set histogram bounds /binning.  " << std::endl;
  std::cout << "-q :  Input clean event list (txt file) by quality cut results" << std::endl; ///< -MK added 11-02-2022
}

int get_median_slice(const TH2S * h, int bin)
{
  double sum = 0; ///< Need to be double to get half of cumulative frequency
  const short * array = h->GetArray();
  for (int i = 1; i <=n_adu_bins; i++)
  {
    sum += array[ bin * (n_adu_bins+2) + i];
  }

  int partial_sum = 0;
  int i = 1;
  while (partial_sum < sum/2)
  {
    partial_sum += array[ bin * (n_adu_bins+2) + i++];
  }

  //! return 0 if bin_center is nagative. -MK added 11-02-2022
  double bin_center = h->GetXaxis()->GetBinCenter(i-1);
  int median = 0;
  if (bin_center > 0) median = floor(bin_center);
  return median;
  //return floor(h->GetXaxis()->GetBinCenter(i-1)); //this makes sense if anything is binned..
}


int parse(int nargs, char ** args)
{

  int iarg = 0;
  std::vector<char *> positional_args;
  while (++iarg < nargs)
  {

//    std::cout << iarg << " " << args[iarg] << std::endl;
    if (!strcmp(args[iarg],"-o"))
    {
      root_output = args[++iarg];
      continue;
    }

    if (!strcmp(args[iarg],"-q"))
    {
      qual_file = args[++iarg];
      continue;
    }

    if (!strcmp(args[iarg],"-M"))
    {
      max_adu = atoi(args[++iarg]);
      continue;
    }

    if (!strcmp(args[iarg],"-m"))
    {
      min_adu = atoi(args[++iarg]);
      continue;
    }

    if (!strcmp(args[iarg],"-d"))
    {
      use_median = true;
      continue;
    }

    if (!strcmp(args[iarg],"-p"))
    {
      use_calpulsers = true;
      continue;
    }
    if (!strcmp(args[iarg],"-C"))
    {
      cache_size = atoi(args[++iarg]);
      continue;
    }

    if (!strcmp(args[iarg],"-t"))
    {
      ROOT::EnableImplicitMT(atoi(args[++iarg]));
      chain.SetParallelUnzip();
      continue;
    }


    if (!strcmp(args[iarg],"-b"))
    {
      adu_bin = atoi(args[++iarg]);
      continue;
    }
    if (!strcmp(args[iarg],"-N"))
    {
      max = atoi(args[++iarg]);
      continue;
    }


    if (!strcmp(args[iarg],"-x"))
    {
      hist_mask = strtol(args[++iarg],0,0);
      continue;
    }

    if (!strcmp(args[iarg],"-h"))
    {
      usage();
      return 1;
    }

    if (args[iarg][0]=='-')
    {
      std::cerr << "unknown argument: " << args[iarg] << std::endl;
      usage();
      return -1;
    }

    positional_args.push_back(args[iarg]);
  }

  if (positional_args.size() < 2)
  {
    usage();
    return -1;
  }

  pedestal_file= positional_args[positional_args.size()-1];

  for (unsigned ipositional = 0; ipositional < positional_args.size()-1; ipositional++)
  {
    chain.Add(positional_args[ipositional]);
  }

  return 0;
}




int main (int nargs, char ** args)
{

  int parse_return = parse(nargs,args);
  if (parse_return < 0) return 1;
  if (parse_return) return 0;




  if (!chain.GetEntries())
  {
    std::cerr << "No entries found :(" << std::endl;
    return 1;
  }

  bool do_full_hists = (use_median || root_output) && hist_mask;

  TH2S * full_hists[nchan]  = {0};
  TH1I * median_difference_hists[nchan] = {0};
  short * arrays[nchan];
  Long64_t entries[nchan];
  TFile * full_hists_file = 0;

  if (do_full_hists)
  {
    n_adu_bins= (max_adu-min_adu+1)/adu_bin;
    if (root_output) full_hists_file = new TFile(root_output,"RECREATE");
    for (int i = 0; i < nchan; i++)
    {
      if ( hist_mask  & (1 << i) )
      {
        full_hists[i] = new TH2S(Form("samp_hist_ch%d", i),
                                 Form("Sample Histogram Channel %d;ADU;Sample", i),
                                 n_adu_bins, min_adu, max_adu,
                                 nsamp, 0, nsamp
                                 );

        median_difference_hists[i] = new TH1I(Form("median_diff_hist_ch%d",i),
                                              Form("Median Difference, Channel %d; Median-Mean", i),
                                              201, -100,100);


        arrays[i] = full_hists[i]->GetArray();
      }
      else
      {
        full_hists[i] = 0;
        arrays[i] = 0;
      }
    }
  }

  std::vector<std::vector<double> > sum(nchan, std::vector<double> ( nsamp,0));
  std::vector<std::vector<double> > sum2(nchan, std::vector<double> ( nsamp,0)) ;
  std::vector<std::vector<int > >num(nchan, std::vector<int> ( nsamp,0));

  RawAtriStationEvent * ev = 0;
  chain.SetBranchAddress("event",&ev);
  chain.SetCacheSize(cache_size*1024*1024);
  chain.AddBranchToCache("event",true);
  chain.StopCacheLearningPhase();

  //! Input quality cut results. -MK added 11-02-2022
  std::ifstream qualFile(qual_file);
  if (qual_file) {
    if(!qualFile.is_open()) {
        std::cout << "Can not open: " << qual_file << "\n";
        abort();
    }
    std::cout<<"Applied quality cut file: "<<qual_file<<std::endl;
  }

  int nhundred = 0;

  int nev = max < 0 ? chain.GetEntries()+1+max : TMath::Min(max, chain.GetEntries());

  for (int iev = 0; iev < nev; iev++)
  {
    chain.GetEntry(iev);
    if (iev >= nhundred*100)
    {
      std::cout << iev << "/"  <<  nev << "\r";
      std::cout << std::flush;
      nhundred++;
    }

    //! pass the event by quality cut results. -MK added 11-02-2022
    if (qual_file) {
      int passed_evt;
      qualFile >> passed_evt;
      if (passed_evt != 1) continue;
    }

    if (ev->isCalpulserEvent() && !use_calpulsers)  continue;

    //skip the first block from all 4 DDA board!
    for (unsigned iblk = 4; iblk < ev->blockVec.size(); iblk++)
    {

      int chan_idx = 0;
      unsigned nchannels = ev->blockVec[iblk].getNumChannels();
      for (unsigned ich= 0; ich< nchannels; ich++)
      {

        if (ev->blockVec[iblk].channelMask && (1 << ich) == 0) continue;
        int chan= chan_per_dda * ev->blockVec[iblk].getDda() + ich;
        int offset = ev->blockVec[iblk].getBlock() * samp_per_block;
        unsigned size = ev->blockVec[iblk].data[chan_idx].size();
        for (unsigned isamp = 0; isamp < size; isamp++)
        {
          UShort_t val = ev->blockVec[iblk].data[chan_idx][isamp];
          unsigned i = (offset+isamp) % nsamp;
          sum2[chan][i] += val*val;
          sum[chan][i] += val;
          num[chan][i] ++;

          if (full_hists[chan])
          {
            int bin = 1+(val-min_adu)/adu_bin;
            if (bin < 0) bin = 0;
            if (bin > n_adu_bins) bin = n_adu_bins+1;
            arrays[chan][bin + (2 + n_adu_bins) * (i+1)]++;

            /** Try to prefetch next row, assuming the next sample will be similar bin*/
            __builtin_prefetch( arrays[chan] + bin-32 + (2 + n_adu_bins) * (i+2),1,0);
          }
        }

        entries[chan]+=ev->blockVec[iblk].data[chan_idx].size();
        chan_idx++;
      }
    }
  }

  qualFile.close();

  std::cout << std::endl;

  //write out pedestal file. This probably isn't in the normal order but the way it's read in, it doesn't matter.

  std::ofstream pf(pedestal_file);

  for (int blk = 0; blk < nblk; blk++)

  {
    for (int ich = 0; ich < nchan; ich++)

    {
      pf <<  ich / chan_per_dda <<  " " << blk << " " <<  ich % chan_per_dda;

      for (int isamp = 0; isamp < samp_per_block; isamp++)
      {
        int idx= isamp+blk*samp_per_block;
        //! return mean = 0 if it is zero division. -MK added 11-02-2022
        int mean = 0;
        if (sum[ich][idx] != 0 || num[ich][idx] != 0) mean = int(round( sum[ich][idx] / num[ich][idx]));
        if (full_hists[ich])
        {
          int median = get_median_slice(full_hists[ich], idx+1);
          median_difference_hists[ich]->Fill(median-mean);
          if (use_median)
          {
            pf << " " <<  median;
          }
        }

        if (!full_hists[ich] || !use_median)
        {
          pf << " " << mean;
        }
      }

      pf << std::endl;
    }
  }



  if (root_output)
  {
    full_hists_file->cd();
    for (int ih = 0; ih < nchan; ih++)
    {
      if (full_hists[ih])
      {
        full_hists[ih]->SetEntries(entries[ih]);
        full_hists[ih]->Write();
        median_difference_hists[ih]->Write();
      }
    }

    for (int ich=0; ich < nchan; ich++)
    {
      TGraphErrors * g = new TGraphErrors(nsamp);

      for (int isamp = 0; isamp < nsamp; isamp++)
      {
        //! return mean = 0 if it is zero division. -MK added 11-02-2022
        double mean = 0;
        if (sum[ich][isamp] != 0 || num[ich][isamp] != 0) mean = sum[ich][isamp] / num[ich][isamp];
        g->SetPoint(isamp,isamp, mean);
        g->SetPointError(isamp, 0, sqrt(mean - g->GetY()[isamp]*g->GetY()[isamp] ));
      }
      g->SetTitle(Form("Channel %d mean/rms",ich));
      g->GetXaxis()->SetTitle("Sample");
      g->GetYaxis()->SetTitle("ADU mean/RMS");
      g->Write(Form("g_ch%02d",ich));
      delete g;
    }
  }

  delete full_hists_file;
}
