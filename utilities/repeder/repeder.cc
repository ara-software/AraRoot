
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
const int samp_per_block = SAMPLES_PER_BLOCK; 
const int chan_per_dda = RFCHAN_PER_DDA; 
const int dda_per_atri = DDA_PER_ATRI; 


int min_adu = 1750-512; 
int max_adu = 1750+512; 
int adu_bin = 1; 
unsigned hist_mask = 0xf0f0f0f0; 

const char * input_file = 0; 
const char * pedestal_file = 0; 
bool use_median = false; 
const char * root_output = 0; 


void usage() 
{
  std::cout << "Usage: repeder input_file.root output_pedestal_file.dat " << std::endl 
            << "      [-d] [-h] [-o output_file.root] [-x hist_channel_mask=0xf0f0f0f0] " << std::endl 
            <<"       -m min_hist_adu="<< min_adu << "] [-M max_hist_adu=" << max_adu <<" ] [-b hist_adu_bin="<< adu_bin <<"]" << std::endl; 
  std::cout << "-h :  Display this message" << std::endl; 
  std::cout << "-d :  Use median instead of mean (for channels defined in hist mask only)" << std::endl; 
  std::cout << "-o :  Auxilliary ROOT output. Will contain histograms for channels in hist mask and also mean/rms graphs. " << std::endl; 
  std::cout << "-x :  Histogram mask. Has no effect if neither -o nor -d are defined. " << std::endl; 
  std::cout << "-m,-M,-b :   Set histogram bounds /binning.  " << std::endl; 
}

double get_median_slice(const TH2 * h, int xbin) 
{
  int sum = 0; 
  for (int i = 1; i <= h->GetNbinsY(); i++) 
  {
    sum += h->GetBinContent(xbin,i); 
  }

  int partial_sum = 0; 
  int i = 1; 
  while (partial_sum < sum/2)
  {
    partial_sum += h->GetBinContent(xbin,i++); 
  }
  return h->GetBinLowEdge(i); 
}


int parse(int nargs, char ** args) 
{

  int iarg = 0; 
  int ipositional = 0; 
  while (++iarg < nargs) 
  {

//    std::cout << iarg << " " << args[iarg] << std::endl; 
    if (!strcmp(args[iarg],"-o"))
    {
      root_output = args[++iarg]; 
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

    if (!strcmp(args[iarg],"-b"))
    {
      adu_bin = atoi(args[++iarg]); 
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

    ipositional++; 


    if (ipositional == 1) 
    {
      input_file = args[iarg]; 
      continue;
    }
    else if (ipositional == 2) 
    {
      pedestal_file = args[iarg]; 
      continue;
    }
    else if (ipositional > 2) 
    {
      std::cerr << "extra positional argument: " << args[iarg] << std::endl; 
      usage(); 
      return -1;
    }
  }

  if (ipositional < 2) 
  {
    usage(); 
    return -1; 
  }

  return 0; 
}




int main (int nargs, char ** args) 
{

  int parse_return = parse(nargs,args); 
  if (parse_return < 0) return 1; 
  if (parse_return) return 0; 


  TFile *  infile = TFile::Open(input_file); 

  if (!infile) 
  {
    std::cerr << "Problem opening " << infile << std::endl; 
    return 1; 

  }
  TTree *t = (TTree*) infile->Get("eventTree"); 

  if (!t) 
  {
    std::cerr << "Could not find eventTree in " << args[1] << std::endl; 
    return 1; 
  }

  bool do_full_hists = (use_median || root_output) && hist_mask; 

  TH2 * full_hists[nchan] ;  
  TFile * full_hists_file = 0; 

  if (do_full_hists) 
  {
    if (root_output) full_hists_file = new TFile(root_output,"RECREATE"); 
    for (int i = 0; i < nchan; i++) 
    {
      if ( hist_mask  & (1 << i) ) 
      {
        full_hists[i] = new TH2S(Form("samp_hist_ch%d", i), 
                                 Form("Sample Histogram Channel %d;Sample;ADU", i), nsamp, 0, nsamp, 
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
  t->SetCacheSize(300*1000*1000); 
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

  //write out pedestal file. This probably isn't in the normal order but the way it's read in, it doesn't matter. 

  std::ofstream pf(args[2]); 

  for (int ich = 0; ich < nchan; ich++) 
  {
    for (int blk = 0; blk < nblk; blk++) 

    {
      pf <<  ich / dda_per_atri <<  " " << blk << " " <<  ich % chan_per_dda ; 

      for (int isamp = 0; isamp < samp_per_block; isamp++) 
      {
        if (full_hists[ich] && use_median) 
        {
          pf << " " <<  int(get_median_slice(full_hists[ich], isamp+1)); 
        }
        else
        {
          pf << " " <<  int(round( sum[ich][isamp+blk*samp_per_block] / num[ich][isamp+blk*samp_per_block])); 
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
        full_hists[ih]->Write(); 
      }
    }

    for (int ich; ich < nchan; ich++) 
    {
      TGraphErrors * g = new TGraphErrors(nsamp); 

      for (int isamp = 0; isamp < nsamp; isamp++) 
      {
        g->SetPoint(isamp,isamp, sum[ich][isamp]/ num[ich][isamp]); 
        g->SetPointError(isamp, 0, sqrt(sum2[ich][isamp]/ num[ich][isamp] - g->GetY()[isamp]*g->GetY()[isamp] )); 
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
