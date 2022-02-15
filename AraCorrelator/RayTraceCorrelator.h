#ifndef RAYTRACECORRELATOR_H
#define RAYTRACECORRELATOR_H

#include <map>
class TGraph;
class TH2D;
class AraGeomTool;

class RayTraceCorrelator : public TObject
{

    private:
        int stationID_;                  ///< Station ID for this correlator instance
        double radius_;                  ///< Radial distance from the center of gravity
        double angularSize_;             ///< Angular bin size in degrees
        int numPhiBins_;                 ///< Number of phi bins (azimuth) in the arrival time tables
        int numThetaBins_;               ///< Number of theta bins (zenith) in the arrival time tables
        int numAntennas_;                ///< Number of antennas in the station
        std::vector<double> phiAngles_;   ///< Vector of the phi points to be sampled (in radians!)
        std::vector<double> thetaAngles_; ///< Vector of the theta points to be sampled (in radians!)
        std::string dirSolTablePath_;     ///< Full path to the direct solution tables
        std::string refSolTablePath_;     ///< Full path to the reflected/refracted solution tables

        // The following setter functions (which nominally do trivial things)
        // are included to enable sanity checks on the arguments 
        // e.g. ensuring radii are positive, station IDs are supported, etc.

        void SetupStationInfo(int stationID, int numAntennas);
        void SetRadius(double radius);
        void SetAngularConfig(double angularSize);
        void SetTablePaths(const std::string &dirPath, const std::string &refPath);

        // a high-dimensional vector to store the arrival times at antennas
        // first index is direct/reflected
        // second index is theta bins
        // third index is phi bins
        // fourth index is number of antennas
        std::vector < std::vector < std::vector < std::vector < double > > > > arrivalTimes_;

        // same dimensions and explanations, just for theta and phi
        std::vector < std::vector < std::vector < std::vector < double > > > > arrivalThetas_;
        std::vector < std::vector < std::vector < std::vector < double > > > > arrivalPhis_;

        
        void ConfigureArrivalVectors(); ///< Function to set the dimensions of arrivalTimes_, arrivalThetas_, etc. correctly

    public:

        // these are getter functions to provide an interface
        int GetStationID(){ return stationID_; }
        int GetNumThetaBins(){return numThetaBins_; }
        int GetNumPhiBins(){return numPhiBins_; }
        double GetAngularSize(){ return angularSize_; }
        double GetRadius(){ return radius_; }
        double GetNumAntennas(){ return numAntennas_; }
        std::vector<double> GetPhiAngles(){ return phiAngles_; }
        std::vector<double> GetThetaAngles(){ return thetaAngles_; }


        //! function to load the arrival time tables
        void LoadArrivalTimeTables(const std::string &filename, int solNum);


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


        //! function to get correlation functions
        /*!
            \param pairs a std::map of pair indices to antenna indices
            \param interpolatedWaveforms a std::map of antenna indices to interpolated waveforms
            \param applyHilbertEnvelope whether or not to apply hilbert enveloping to the correlation functions
            \return a std::vector of the correlation functions (one for each pair)
        */
        std::vector<TGraph*> GetCorrFunctions(
            std::map<int, std::vector<int> > pairs,
            std::map<int, TGraph*> interpolatedWaveforms,
            bool applyHilbertEnvelope = true
        );


        //! function to get lookup the antenna arrival information
        /*!
            \param ant antenna index
            \param solNum which solution number (0 = direct, 1 = reflected/refracted)
            \param thetaBin the theta bin desired (bin space, not angle space!!)
            \param phiBin the phi bin desired (bin space, not angle space!!)
            \param arrivalTheta passed by reference, content is replaced with the arrival theta angle (where the signal is COMING FROM)
            \param arrivalPhi passed by reference, content is replaced with the arrival phi angle (where the signal is COMING FROM)
            \return void
        */
        void LookupArrivalAngles(
            int ant, int solNum,
            int thetaBin, int phiBin,
            double &arrivalTheta, double &arrivalPhi
        );

        //! function to get lookup the bin numbers of a source hypothesis direction
        /*!
            \param theta source hypothesis direction up/down angle, from -90 to 90
            \param theta source hypothesis direction left/right angle, from -180 to 180
            \param thetaBin the angular bin corresponding to theta, passed by reference (is replaced by the bin value)
            \param phiBin the angular bin corresponding to phi, passed by reference (is replaced by the bin value)
            \return void
        */
        void ConvertAngleToBins(
            double theta, double phi,
            int &thetaBin, int &phiBin
        );


        //! function to get an interferometric map
        /*!
            \param pairs a std::map of antenna pairs
            \param corrFunctions a std::vector of correlation functions, one for each pair in pairs (in that order!)
            \param solNum whether to have the first or second (0 or 1) solution hypothesis
            \param weights weights to apply to each map; default = equal weights, or 1/pairs.size()
            \return a 2D histogram with the values filled with the interferometric sums
        */
        TH2D* GetInterferometricMap(
            std::map<int, std::vector<int> > pairs,
            std::vector<TGraph*> corrFunctions,
            int solNum,
            std::map<int, double> weights = {}
        );


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