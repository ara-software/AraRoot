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
        std::map<int, std::map<int, TH2D > > arrivalTimes_;
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

            Calls LoadArrivalTimeTables(filename, solNum) (below)

            \param tableDir The full path to the location of the arrival time tables
            \return void
        */
        void LoadTables();

        //! function to load the arrival time tables
        // calls 
        void LoadArrivalTimeTables(const std::string &filename, int solNum);

        //! a function to return the pairs to be used in the interferometery
        /*!
            \param stationID an ARA station ID
            \param geomTool an instance of an AraGeom Tool
            \param polarization what polarization to use
            \param excludedChannels what channels to exclude in forming pairs (default: exclude none)
            \return vector of vectors, holding the pairings
        */
        std::map<int, std::vector<int> > SetupPairs(
            int stationID,
            AraGeomTool *geomTool,
            AraAntPol::AraAntPol_t polarization, 
            std::vector<int> excludedChannels = {}
        );

        //! Get the arrival delays
        /*!
            \param pairs the pairs of antennas for which you want us to compute delays
            \return a complicated data structure; see more information below

            In this refactorization of the RTC, we want to cache the arrival *delays* for a give pair.
            And we want to store them time ordered, for potential future performance reasons (e.g. wanting to use a GSL interpolatino accelerator).
            For this reason, for a given solution/pair we need to keep track of *both* the TH2D bin it belongs to, and, the actual time delay.
            So this function returns a std::pair.
            pair->first is the bins, pair->second are the delays
            So pair->first[solNum][pair][iterator] gives you the TH2D bin number for a given solution and pair and global "iterator" variable.
            while pair->second[solNum][pair] gives you the delay in ns for a given solution, pair, and global "iterator" variable.
            The TH2D bin it belongs to is in pair->first.
        */
        std::pair< 
            std::vector< std::vector< std::vector< int > > >,
            std::vector< std::vector< std::vector< double > > > > GetArrivalDelays(std::map<int, std::vector<int > > pairs);

        //! function to get correlation functions
        /*!
            \param pairs a std::map of pair indices to antenna indices
            \param interpolatedWaveforms a std::map of antenna indices to interpolated waveforms
            \param applyHilbertEnvelope whether or not to apply hilbert enveloping to the correlation functions
            \return a std::vector of the correlation functions (one for each pair)
        */
        std::vector<TGraph>GetCorrFunctions(
            std::map<int, std::vector<int> > pairs,
            std::map<int, TGraph*> interpolatedWaveforms,
            bool applyHilbertEnvelope = true
        );

        //! function to get an interferometric map
        /*!
            \param pairs a std::map of antenna pairs
            \param corrFunctions a std::vector of correlation functions, one for each pair in pairs (in that order!)
            \param arrivalDelays a std::pair; the arrival delays; this is the output of GetArrivalDelays
            \param solNum whether to have the first or second (0 or 1) solution hypothesis
            \param weights weights to apply to each map; default = equal weights, or 1/pairs.size()
            \return a 2D histogram with the values filled with the interferometric sums
        */
        TH2D GetInterferometricMap(
            const std::map<int, std::vector<int> > pairs,
            const std::vector<TGraph> &corrFunctions,
            const std::pair< std::vector< std::vector< std::vector< int > > >, std::vector< std::vector< std::vector< double > > > > &arrivalDelays,
            const int solNum,
            std::map<int, double> weights = {}
        );

        //! function to make sure the user is asking for a reasonable theta/phi, and return the global bin number in the TH2D
        /*!
            \param theta the theta desired (angle! in degrees!)
            \param phi the phi desired (angle! in degrees!)
            \return globalBin the TH2D global bin number, found with TH2D::FindBin(theta,phi)
        */        
        int ValidateAnglesGetGlobalBinNumber(double theta, double phi);

        int ConvertAnglesToTH2DGlobalBin(double theta, double phi);


        //! function to get lookup the arrival time information
        /*!
            \param ant antenna index
            \param solNum which solution number (0 = direct, 1 = reflected/refracted)
            \param theta the theta desired (angle! in degrees!)
            \param phi the phi desired (angle! in degrees!)
            \return arrival time
        */        
        double LookupArrivalTime(
            int ant, int solNum,
            double theta, double phi
        );

        //! function to get lookup the antenna arrival information
        /*!
            \param ant antenna index
            \param solNum which solution number (0 = direct, 1 = reflected/refracted)
            \param theta the theta desired (angle! in degrees!)
            \param phi the phi desired (angle! in degrees!)
            \param arrivalTheta passed by reference, content is replaced with the arrival theta angle (where the signal is COMING FROM)
            \param arrivalPhi passed by reference, content is replaced with the arrival phi angle (where the signal is COMING FROM)
            \return void
        */
        void LookupArrivalAngles(
            int ant, int solNum,
            double theta, double phi,
            double &arrivalTheta, double &arrivalPhi
        );

        //! function to get lookup the ray launch information
        /*!
            \param ant antenna index
            \param solNum which solution number (0 = direct, 1 = reflected/refracted)
            \param theta the theta desired (angle! in degrees!)
            \param phi the phi desired (angle! in degrees!)
            \param launchTheta passed by reference, content is replaced with the launch theta angle (in a coordinate where z-axis is along the Earth's radius)
            \param launchPhi passed by reference, content is replaced with the launch phi angle (in a coordinate where z-axis is along the Earth's radius)
            \return void
        */
        void LookupLaunchAngles(
            int ant, int solNum,
            double theta, double phi,
            double &launchTheta, double &launchPhi
        );


        ////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////
        /// This part is the header declaration for the _detail file
        ////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////

        //! a function to get a correlation graph with some special normalization
        /*!
            \param gr1 the graph to be cross correlated
            \param gr2 the second graph to be cross correlated
            \return TGraph* the cross-correlation function
        */
        TGraph* getCorrelationGraph_WFweight(TGraph * gr1, TGraph * gr2);


        //! a function to get a correlation graph with the normalization used in the OSU A2/3 Diffuse Analysis
        /*!
            \param gr1 the graph to be cross correlated
            \param gr2 the second graph to be cross correlated
            \return TGraph* the cross-correlation function
        */
        TGraph* getCorrelationGraph_OSUNormalization(TGraph * gr1, TGraph * gr2);


        //! a function to get the un-normalized cross correlation function between two ararys
        /*!
            \param length the length of the two arrays
            \param oldY1 array for first data series to be correlated
            \param oldY2 array for the second series to be correlated
            \return array of doubles, corresponding oto the un-normalized cross correlation function
        */
        double * getCorrelation_NoNorm(int length, double * oldY1, double * oldY2);


        //! a function to normalize a waveform by it's RMS
        /*!
            \param grIn the graph to be normalized
            \return TGraph* the normalized graph (a new object)
        */
        std::unique_ptr<TGraph> getNormalisedGraphByRMS(TGraph *grIn);

    ClassDef(RayTraceCorrelator,1);

};
#endif //RAYTRACECORRELATOR_H