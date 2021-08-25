#ifndef RAYTRACECORRELATOR_H
#define RAYTRACECORRELATOR_H

#include "TObject.h"
#include "AraStationInfo.h"

class RayTraceCorrelator : public TObject
{

    private:
        int stationID_;                  ///< Station ID for this correlator instance
        double radius_;                  ///< Radial distance from the center of gravity
        double angularSize_;             ///< Angular bin size in degrees
        int numPhiBins_;                 ///< Number of phi bins (azimuth) in the arrival time tables
        int numThetaBins_;               ///< Number of theta bins (zenith) in the arrival time tables
        int numAntennas_;                ///< Number of antennas in the station
        int unixTime_;                   ///< Unixtime of the geometry database we want

        // The following setter functions (which nominally do trivial things)
        // are included to enable sanity checks on the arguments 
        // e.g. ensuring radii are positive, station IDs are supported, etc.

        void SetupStationInfo(int stationID, int unixTime);
        void SetRadius(double radius);
        void SetAngularConfig(double angularSize);

        // a high-dimensional vector to store the arrival times at antennats
        // first index is direct/reflected
        // second index is theta bins
        // third index is phi bins
        // fourth index is number of antennas
        std::vector < std::vector < std::vector < std::vector < double > > > > arrivalTimes_;
        
        void ConfigureArrivalTimesVector(); ///< Function to set the dimensions of arrivalTimes_ correctly

    public:

        // these are getter functions to provide an interface
        int GetStationID(){ return stationID_; }
        int GetNumThetaBins(){return numThetaBins_; }
        int GetNumPhiBins(){return numPhiBins_; }
        double GetAngularSize(){ return angularSize_; }
        double GetRadius(){ return radius_; }
        double GetNumAntennas(){ return numAntennas_; }
        double GetUnixtime(){ return unixTime_; }

        //! function to load the arrival time tables
        /*!
            \param filename full (absolute) path to the arrival timing tables
            \param solNum Which solution table do we want to load
            \return void
        */
        void LoadArrivalTimeTables(const std::string &filename, int solNum);


        //! constructor for the RayTraceCorrelator
        /*!
            \param stationID ID of the station
            \param radius The radius for the ray trace correlator
            \param angularSize The angular binning
            \param iceModel The AraSim icemodel (should match with RAY_TRACE_ICE_MODEL_PARAMS)
            \param tableDir The full path to the location of the arrival time tables
            \param unixTime The unixTime of the events you want to correlated
            \return an instance of the ray trace correlator
        */
       // NB: We must enable passing of the unixTime argument, since the geometry databases are time-evolving
        RayTraceCorrelator(int stationID, double radius, double angularSize, int iceModel,
            const std::string &tableDir, int unixTime=0);



        ~RayTraceCorrelator(); ///< Destructor




    
    // private:
        // std::string table_dir_;

    ClassDef(RayTraceCorrelator,0);

};

#endif //RAYTRACECORRELATOR_H