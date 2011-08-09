#include "AraWebPlotterConfig.h"
#include "configLib/configLib.h"
#include "kvpLib/keyValuePair.h"

#include <iostream>
#include <cstring>
#include <cstdlib>


AraWebPlotterConfig::AraWebPlotterConfig()
{
  //Default constructor
  readConfigFile();
}


void AraWebPlotterConfig::readConfigFile()
{
 /* Config file thingies */
    ConfigErrorCode status=CONFIG_E_OK;
    //    int tempNum=12;
    //    KvpErrorCode kvpStatus=KVP_E_OK;
    char *tempString;
//     kvpReset();
//     status = configLoad ("araWebPlotter.config","input") ;    
//     if(status == CONFIG_E_OK) {       
// 	tempString = kvpGetString("eventLinkDir");
// 	if(tempString) {
// 	    strncpy(fEventLinkDir,tempString,FILENAME_MAX);
// 	}
// 	else {
// 	    std::cerr << "Couldn't get eventLinkDir" << std::endl;
// 	    exit(0);
// 	}   

// 	tempString = kvpGetString("hkLinkDir");
// 	if(tempString) {
// 	    strncpy(fHkLinkDir,tempString,FILENAME_MAX);
// 	}
// 	else {
// 	    std::cerr << "Couldn't get hkLinkDir" << std::endl;
// 	    exit(0);
// 	}
//     }
//     else {
//       std::cerr << "Error reading araWebPlotter.config: " << configErrorString (status)  << std::endl;
//     }


    kvpReset();
    status = configLoad ("araWebPlotter.config","output") ;    
    if(status == CONFIG_E_OK) {       
	tempString = kvpGetString("rootFileDir");
	if(tempString) {
	    strncpy(fRootFileDir,tempString,FILENAME_MAX);
	}
	else {
	    std::cerr << "Couldn't get rootFileDir" << std::endl;
	    exit(0);
	}       

	tempString = kvpGetString("plotDir");
	if(tempString) {
	    strncpy(fPlotDir,tempString,FILENAME_MAX);
	}
	else {
	    std::cerr << "Couldn't get plotDir" << std::endl;
	    exit(0);
	}   
	fMakeEventPlots=kvpGetInt("makeEventPlots",0);
    }
    else {
      std::cerr << "Error reading araWebPlotter.config: " << configErrorString (status)  << std::endl;

    }

//     kvpReset();
//     status = configLoad ("araWebPlotter.config","output") ;    
//     if(status == CONFIG_E_OK) {       
// 	tempString = kvpGetString("plotDir");
// 	if(tempString) {
// 	    strncpy(plotDir,tempString,FILENAME_MAX);
// 	}
// 	else {
// 	    std::cerr << "Couldn't get plotDir" << std::endl;
// 	    exit(0);
// 	}
// 	tempString = kvpGetString("dataDir");
// 	if(tempString) {
// 	    strncpy(dataDir,tempString,FILENAME_MAX);
// 	}
// 	else {
// 	    std::cerr << "Couldn't get dataDir" << std::endl;
// 	    exit(0);
// 	}
//     }
//     else {
// 	eString=configErrorString (status) ;
// 	std::cerr << "Error reading araWebPlotter.config: " << eString << std::endl;
//     }

//     kvpReset();
//     status = configLoad ("araWebPlotter.config","plots") ;    
//     if(status == CONFIG_E_OK) {  
// 	makeHeaderPlots=kvpGetInt("makeHeaderPlots",1);
// 	makeSurfHkPlots=kvpGetInt("makeSurfHkPlots",1);
// 	makeTurfRatePlots=kvpGetInt("makeTurfRatePlots",1);
// 	makeMonitorPlots=kvpGetInt("makeMonitorPlots",1);
// 	makeHkPlots=kvpGetInt("makeHkPlots",1);
// 	makeGpsPlots=kvpGetInt("makeGpsPlots",1);
// 	makeEventPlots=kvpGetInt("makeEventPlots",1);
//     }
//     else {
// 	eString=configErrorString (status) ;
// 	std::cerr << "Error reading araWebPlotter.config: " << eString << std::endl;
//     }
    
    //    return status;
    //    std::cout << fEventLinkDir << "\t" << fHkLinkDir << "\n";
}
