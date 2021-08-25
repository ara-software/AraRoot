#include <iostream>

#include "AraGeomTool.h"
#include "RayTraceCorrelator.h"

int main(int argc, char **argv)
{

    int station = 2;
    double radius = 300.;
    double angular_size = 1.;
    int iceModel = 0;
    int unixTime = 0;

    RayTraceCorrelator *theCorrelator = new RayTraceCorrelator(station, 
        radius, angular_size, iceModel, 
        "/mnt/home/baclark/ara/araroot_rt_correlator/tables/",
        unixTime
    );
    std::cout<<"Num theta bins "<<theCorrelator->GetNumThetaBins()<<std::endl;
    std::cout<<"Num phi bins "<<theCorrelator->GetNumPhiBins()<<std::endl;
    std::cout<<"Angular bin size "<<theCorrelator->GetAngularSize()<<std::endl;
    std::cout<<"Station ID is "<<theCorrelator->GetStationID()<<std::endl;
    std::cout<<"Radius is "<<theCorrelator->GetRadius()<<std::endl;
    std::cout<<"Num antennas is "<<theCorrelator->GetNumAntennas()<<std::endl;
    std::cout<<"Unixtime is "<<theCorrelator->GetUnixtime()<<std::endl;

}

