# RayTraceCorrelator

## Introduction
The RayTraceCorrelator (RTC) is a routine to make interferometric maps of ARA data.
The RTC is (currently) designed to make correlation maps as a function of theta
and phi for fixed radius.
So the maps give the summed correlation value for all points on the sky for
a source at a radius R from the detector's center of gravity.

The RayTraceCorrelator is "unique" in that the time delays used to sample
the correlation functions are calculated using a ray tracer, so that
the index of refraction can vary as a function of depth: n(z).
The time delays are stored as tables in ROOT files.

## Quick Start
An example of how to use the interferometer is provided in `makeRTCorrelationMaps.cxx`.

An instance of the RayTraceCorrelator can be called as such:

```c++
RayTraceCorrelator *theCorrelator = new RayTraceCorrelator(int station, 
        double radius, double angular_size, int iceModel, int unixTime
    );
```

After the correlator is created, one must load the tables holding the arrival times:
```c++
theCorrelator->LoadTables("/path/to/tables/");
```

The interferometer takes as inputs (1) interpolated waveforms, (2) pairs of antennas which
are to be included in the map, and (3) the solution hypothesis (direct or reflected/refracted).
The map making routine returns a ROOT TH2D, and can be called like:

```c++
TH2D *map = theCorrelator->GetInterferometricMap(waveforms, pairs, solution);
```

For more detailed discussion, see below, or see the example.

## Design Philosopy

The correlator tries to conceptually separate (1) *making* the maps
from (2) *calculating* the arrival times. For this reason, arrival times
are stored in tables that are loaded by the correlator.
Construction of the arrival time tables is technically up to user's preferences, 
though we usually use AraSim ray tracer.
Because the AraSim ray tracer is used, the index of refraction
model is tied to the AraSim `RAY_TRACE_ICE_MODEL_PARAMS` variable.
This is currently a simple integer.
A script to calculate the timing tables via AraSim 
is provided (`makeRTArrivalTimeTables.cpp`). 
See below for more details on the ice model and using the table generation script.

Because the RTC tries to separate making maps from calculating arrival times,
an instance of the RTC explicitly does not know about the station geometry, etc.
It interacts with the timing tables and the timing tables only.

The RTC also tries to strictly be a *correlator*, __not__ a data handler.
The inputs to the ray trace correlator are mainly (1) waveforms 
and (2) what pairs of  those waveforms should be correlated 
and included in the interferometric map 
(along with what solution hypothesis to test).
Because the RTC ingests the waveforms and pairs directly, 
the user has lots of flexibility.
In particular:
- The user can adjust the waveforms (e.g. CW filter, bandpassing, etc.) before they arrive at the correlator.
- If the user wants a "single-pair" map (the contribution of a single antenna pair to the overall map), just pass that single pair to the correlator.
- The user can correlate any arbitrary mix of waveforms. E.g. just VPol, just HPol, but also cross-polarization (VPol and HPol). This also allows a user to dynamically adjust what waveforms are used for a specific event, for example, by only including channels above a specific SNR threshold.

### History
The "original" RayTraceCorrelator was developed by Eugene Hong and Carl Pfender
at Ohio State circa 2013. Thanks for all their hard work!

We learned a few things from the original OSU correlator.

One of the major takeaways is that the core correlator routine
should *just* be a correlator, and not try to handle more complex operations 
like identifying pairs, filtering waveforms, etc.

We also took great pains to remove AraSim dependencies from the correlation routines.
The correlator requires user provided timing tables, and that's it.
In practice, most people will want to use AraSim to calculate the delay tables.
But as it's written now, we avoid making AraSim a dependency of AraRoot,
which was a *major* drawback to the original OSU correltaor.

## Documentation

### Coordinate System

The coordinate system for the RayTraceCorrelator is righthanded,
and in the *station local* coordinate system.
This means that the local \hat{x} (x-axis unit vector)
aligns with the ice flow direction.

The origin of the coordinate system is the center of gravity of the station.
Meaning the average location of the (usually 16) deep antennas in the station.

The phi/azimuth angle varies from -180 to 180 degrees,
where zero corresponds to \hat{x} = x-axis unit vector.

The theta/zenith angle varies from -90 to 90 degrees,
where -90 = source is straight below the station center,
0 = source is horizontal/sideways to the station center,
90 = source is straight above the station center.

The RTC works by varying the *source location*. So if a map
has a peak at theta = 30, that means the location of a hypothetical
source at radius R produces a maximum summed correlation
when the source is 30 degrees above horizontal (relative to station center).
The maps give hypothetical *source locations*. 
__Not__ arrival directions at the station or at an antenna.
(This is a common point of confusion!!)

### Variables and Definitions

Creating an instance of the RayTraceCorrelator requires 
the user to specify a few variables:
- station: this must be an ARA station between 0 (Testbed) and 5 (A5)
- radius: the distance from the station center of gravity for a putative source
- angular size: the angular binning (in degrees)
- icemodel: the ice model, which specifies the depth dependent index of refraction
- unixtime: default is 0; use actual run time if you like 

#### Angular Size
The `angularSize` parameter controls the angular binning.
An `angularSize = 1` results in 1 degree bins, distrbuted uniformly 
from -180 degrees to 180 degrees in phi and -90 to 90 degrees in theta
(see coordinate systems above). 
This means there will be 360 phi bins and 180 theta bins.
The bin centers are placed at half an angular step size.
So for `angularSize = 1`, the phi bin centers will be at 88.5 degrees,
87.5 degrees, and so on.

As implemented, this results in uniform sampling in zenith, 
as opposed to cos(zenith), which is more traditional.
Adding this as a feature is on the to-do list. (See below.)

Because the angular size sets both the theta and phi binning,
the setting up of the arrival angles etc. is completely
internally managed by the correlator. Do not try and control
the number of bins, etc manually. This is reasonably
locked away via `private` member variables and setters.
If you need to access variables like "the number of phi bins,"
please use the correct corresponding getter function, e.g.:

```c++
int numPhiBins theCorrelator->GetNumPhiBins();
```

#### IceModel Params

AraSim is used as the ray tracer, and so the icemodels are (presently) keyed 
to the AraSim `RAY_TRACE_ICE_MODEL_PARAMS` variable.
AraSim always assumes a exponential profile, with the parameters of that
profile changing.
See the [AraSim Settings.h](https://github.com/ara-software/AraSim/blob/2758c07dc56fb1a9a784e460306868f0f940b499/Settings.h#L264) file for a list
of currently available ice models.

Because the correlator doesn't interface with AraSim at all (only the delay tables)
it should be possible to remove this icemodel key completely.
And only pass e.g. the path to the tables. See the to-do list.

#### Unixtime

Because the ray trace correlator needs to know how many deep antennas
are in a station, it does need to talk to the AraGeomTool.
And since the station geometries change a bit as a function of time,
it also needs to know the unixtime of the events you would like to correlate.
Since the number of deep antennas is static for every station except
maybe A5 (which was merged with the PA DAQ in 2019), it is unlikely
that is argument will ever be needed. But is provided for future proofing.

An alternative might be to allow the user to specify `numAntennas_` manually.
See the to-do list.

## Making Correlation Maps

There are three arguments to the `GetInterferometricMap` routine:
- waveforms: a map of antennas (as keys) to their waveforms (as values)
- radius: a map of pair indices (as keys) to their respective antennas (as values)
- solution: what solution hypothesis to assume (direct or reflected/refracted)

We use C++ maps to make handling things easier. Maps are much
easier to use than you might think 
(they function remarkably like python dictionaries, but with stronger typing).
See e.g. [this page](https://www.freecodecamp.org/news/c-plus-plus-map-explained-with-examples/)
for a crash-course.


### Waveforms

The waveforms need to be presented to the correlator as a map of
antenna indices to waveforms:

```c++
std::map<int, TGraph*> waveforms;
```

We choose to use a C++ `std::map` to streamline and make more robust
the identification of a specific antenna identifier with a specific waveform.
(For example, traditionally this could have been a `std::vector<TGraph*>`,
but that implicitly assumes that the vector index is meaningfully tied to the waveform,
which isn't generally a good idea.)

Populating a C++ map is easy, and shown in the example.

The waveform must be interpolated to a common timebase!
This is easily done with e.g. `FFTtools::getInterpolatedGraph`.
The correlator probably won't explicitly fail if this is not true,
but the results might be ill-behaved.

### Pairs

The pairs need to be presented to the correlator as a map of
pair indices to antenna numbers:

```c++
std::map<int, std::vector<int> > pairs;
```

We choose to use a C++ `std::map` to streamline and make more robust
the identification of a specific pair with their constituent antennas.
For example, if we wanted to include the pairings of channel 0 and 1,
and call it "pair 0", we would do the following:

```c++
std::map<int, std::vector<int> > pairs;
std::vector<int> temp;
temp.push_back(0);
temp.push_back(1);
pairs[0] = temp; // we'll call this one "pair 0"
```

Construction of the pairs is totally up to the user.
However, anticiptaing that most users will want to do "standard" things
like "just VPol" or "just HPol" maps, we provided one helper function:

```c++
theCorrelator->SetupPairs(AraAntPol::kVertical, excludedChannels);
```

But we emphasize that you can form whatever pairs you want!

## To Do
1. Remove the IceModel dependence in the correlator. All the user needs to do is specify the tables.
2. Remove the unixTime dependence. E.g. let the user specify `numAntennas_` manually?
3. Add support for uniform binning in cos(theta) instead of theta.
4. Make sure unixTime is getting handled correctly
5. Add the ability to "weight" various pairs
6. Add helper functions for "get peak," "get max correlation," etc.
7. Add ability to take weights for each pair in the `GetInterferometricMap` function.


More on 3: This should be done by changing the correlator constructor  
so that the intializer can be used for both uniform zenith 
and uniform cos(zenith) sampling. 
That is, DO NOT CREATE A NEW CONSTRUCTOR FOR THE COS(THETA) VERSION. 
Update the existing contructor. 
Controlling it through e.g. an NSIDE paramter 
and healpix might have been more robust, but the C++ implementation seemed
burdensome and we wanted to avoid more dependencies.
NB: ROOT TH2's do support projections,
see e.g. the [earth.C example](https://root.cern.ch/doc/v622/earth_8C.html).
So maybe this is easier than it looks.
