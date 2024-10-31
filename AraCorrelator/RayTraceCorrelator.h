#ifndef RAYTRACECORRELATOR_H
#define RAYTRACECORRELATOR_H

#include <map>
#include <memory>
class TGraph;
class TH2D;
class AraGeomTool;
#include "TH2D.h"

class RayTraceCorrelator : public TObject
{

    private:
        int stationID_;                             ///< Station ID for this correlator instance
        double radius_;                             ///< Radial distance from the center of gravity
        double angularSize_;                        ///< Angular bin size in degrees
        int numPhiBins_;                            ///< Number of phi bins (azimuth) in the arrival time tables
        int numThetaBins_;                          ///< Number of theta bins (zenith) in the arrival time tables
        int numAntennas_;                           ///< Number of antennas in the station
        std::string dirSolTablePath_;               ///< Full path to the direct solution tables
        std::string refSolTablePath_;               ///< Full path to the reflected/refracted solution tables
        std::shared_ptr<TH2D> dummyMap;             ///< A dummy 2D histogram

        // The following setter functions (which nominally do trivial things)
        // are included to enable sanity checks on the arguments 
        // e.g. ensuring radii are positive, station IDs are supported, etc.

        void SetupStationInfo(int stationID, int numAntennas);
        void SetRadius(double radius);
        void SetAngularConfig(double angularSize);
        void SetTablePaths(const std::string &dirPath, const std::string &refPath);

        // containers
        // outer map layer has "key" of solution (0 or 1)
        // inner map layer has "key" of antenna (0->N), with "value" of the timing TH2D
        std::map<int, std::map<int, TH2D > > arrvialTimes_;
        std::map<int, std::map<int, TH2D > > arrivalThetas_;
        std::map<int, std::map<int, TH2D > > arrivalPhis_;
        std::map<int, std::map<int, TH2D > > launchThetas_;
        std::map<int, std::map<int, TH2D > > launchPhis_;
       
    public:

        // these are getter functions to provide an interface
        int GetStationID(){ return stationID_; }
        int GetNumThetaBins(){return numThetaBins_; }
        int GetNumPhiBins(){return numPhiBins_; }
        double GetAngularSize(){ return angularSize_; }
        double GetRadius(){ return radius_; }
        double GetNumAntennas(){ return numAntennas_; }
        std::shared_ptr<TH2D> GetTemplateMap(){return dummyMap;}


        //! constructor for the RayTraceCorrelator
        /*!
            \param stationID ID of the station
            \param numAntennas number of antennas in the timing tables (that we expect the correlator to know about)
            \param numAntennas The radius for the ray trace correlator
            \param angularSize The angular binning
            \param dirSolTablePath Complete path to the direct RT solution tables
            \param refSolTablePath Complete path to the reflected/refracted RT solution tables
            \return an instance of the ray trace correlator
        */
        RayTraceCorrelator(int stationID, int numAntennas, double radius, double angularSize, 
            const std::string &dirSolTablePath, const std::string &refSolTablePath
        );

        ~RayTraceCorrelator(); ///< Destructor


        //! constructor for the RayTraceCorrelator
        /*!
            \param tableDir The full path to the location of the arrival time tables
            \return void
        */
        void LoadTables();

        //! function to load the arrival time tables
        void LoadArrivalTimeTables(const std::string &filename, int solNum);

    ClassDef(RayTraceCorrelator,1);

};

#endif //RAYTRACECORRELATOR_H