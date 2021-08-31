#ifndef RAYTRACECORRELATOR_H
#define RAYTRACECORRELATOR_H

#include <map>
class TGraph;
class TH2D;

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
        int iceModel_;                   ///< The AraSim RAY_TRACE_ICE_MODEL_PARAMS choice
        std::vector<double> phiAngles_;   ///< Vector of the phi points to be sampled (in radians!)
        std::vector<double> thetaAngles_; ///< Vector of the theta points to be sampled (in radians!)

        // The following setter functions (which nominally do trivial things)
        // are included to enable sanity checks on the arguments 
        // e.g. ensuring radii are positive, station IDs are supported, etc.

        void SetupStationInfo(int stationID, int unixTime);
        void SetRadius(double radius);
        void SetAngularConfig(double angularSize);
        void SetIceModel(int iceModel);

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
        int GetIceModel(){return iceModel_; }
        double GetAngularSize(){ return angularSize_; }
        double GetRadius(){ return radius_; }
        double GetNumAntennas(){ return numAntennas_; }
        double GetUnixtime(){ return unixTime_; }
        std::vector<double> GetPhiAngles(){ return phiAngles_; }
        std::vector<double> GetThetaAngles(){ return thetaAngles_; }


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
            \param unixTime The unixTime of the events you want to correlated
            \return an instance of the ray trace correlator
        */
       // NB: We must enable passing of the unixTime argument, since the geometry databases are time-evolving
        RayTraceCorrelator(int stationID, double radius, double angularSize, int iceModel, int unixTime=0);


        ~RayTraceCorrelator(); ///< Destructor


        //! constructor for the RayTraceCorrelator
        /*!
            \param tableDir The full path to the location of the arrival time tables
            \return void
        */
        void LoadTables(const std::string &tableDir);


        //! function to get an interferometric map
        /*!
            \param interpolatedWaveforms a std::map of antenna numbers to interpolated waveforms
            \param pairs a std::map of antenna pairs
            \param solNum whether to have the first or second (0 or 1) solution hypothesis
            \param applyHilbertEnvelope whether or not to apply a hilbert envelope to the correlation function
            \param weights weights to apply to each map; default = equal weights, or 1/pairs.size()
            \return a 2D histogram with the values filled with the interferometric sums
        */
        TH2D* GetInterferometricMap(
            std::map<int, TGraph*> interpolatedWaveforms,
            std::map<int, std::vector<int> > pairs,
            int solNum,
            std::map<int, double> weights = {},
            bool applyHilbertEnvelope = true
        );


        //! a function to return the pairs to be used in the interferometery
        /*!
            \param polarization what polarization to use
            \param excludedChannels what channels to exclude in forming pairs (default: exclude none)
            \return vector of vectors, holding the pairings
        */
        std::map<int, std::vector<int> > SetupPairs(
            AraAntPol::AraAntPol_t polarization, 
            std::vector<int> excludedChannels = {}
        );


        //! a function to get a correlation graph with some special normalization
        /*!
            \param gr1 the graph to be cross correlated
            \param gr2 the second graph to be cross correlated
            \return TGraph* the cross-correlation function
        */
        TGraph* getCorrelationGraph_WFweight(TGraph * gr1, TGraph * gr2);


        //! a function to get the un-normalized cross correlation function between two ararys
        /*!
            \param length the length of the two arrays
            \param oldY1 array for first data series to be correlated
            \param oldY2 array for the second series to be correlated
            \return array of doubles, corresponding oto the un-normalized cross correlation function
        */
        double * getCorrelation_NoNorm(int length, double * oldY1, double * oldY2);


        //! a function to get the linearly interpolated value of a function at a time
        /*!
            \param grIn the TGraph to be evaluated
            \param xvalue the point at which to get the interpolated value
            \return the interpolated value of grIn at point xalue
        */
        double fastEvalForEvenSampling(TGraph* grIn, double xvalue);

    ClassDef(RayTraceCorrelator,0);

};

#endif //RAYTRACECORRELATOR_H