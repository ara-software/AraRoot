/*
runAraDisplay.C
By: Jonathan Davies
Updated: Brian Clark (Feb 2019)

This is meant to be used as a ROOT macro. For example:

1) Open ROOT: `root` (with x-forwarding enabled)
2) Load the macro: `.L runAraDisplay.C`
3) Run the function: `runAraDisplay("/path/to/data/event1407.root", "/path/to/pedestal/pedestalValues.txt", 2);

*/


void runAraDisplay(char *eventFile, char *pedFile, int stationID) {
    if (pedFile) AraEventCalibrator::Instance()->setAtriPedFile(pedFile,stationID);
    AraDisplay *magicPtr = new AraDisplay(eventFile,AraCalType::kJustPed);
    magicPtr->startEventDisplay();
}
