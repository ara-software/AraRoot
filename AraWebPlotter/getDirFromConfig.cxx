#include<iostream>
#include<string.h>
//ARA includes
#include "AraWebPlotterConfig.h"

void usage(int argc, char **argv)
{
  std::cout << "The ARA Web Plotter Config Directory Getter\n";
  std::cout << "Usage instructions, call one of the following\n";
  //  std::cout << argv[0] << " --eventLinkDir\n";
  //  std::cout << argv[0] << " --hkLinkDir\n";
  std::cout << argv[0] << " --rootFileDir\n";
  std::cout << argv[0] << " --plotDir\n";
    
}


int main(int argc, char **argv) {
  //Step one is to read the config file.
  AraWebPlotterConfig *araConfig = new AraWebPlotterConfig();


  if(argc==1) {
    usage(argc,argv);
    return -1;
  }
//   if(strcmp(argv[1],"--eventLinkDir")==0) {
//     std::cout << araConfig->getEventLinkDir() << "\n";
//     return 0;
//   }
//   if(strcmp(argv[1],"--hkLinkDir")==0) {
//     std::cout << araConfig->getHkLinkDir() << "\n";
//     return 0;
//   }
  if(strcmp(argv[1],"--rootFileDir")==0) {
    std::cout << araConfig->getRootFileDir() << "\n";
    return 0;
  }
  if(strcmp(argv[1],"--plotDir")==0) {
    std::cout << araConfig->getPlotDir() << "\n";
    return 0;
  }
  usage(argc,argv);
  return -1;


}
