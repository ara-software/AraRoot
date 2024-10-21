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
        double radius, double angular_size, string dirSolTablePath, string refSolTablePath
    );
```

After the correlator is created, one must load the tables holding the arrival times:
```c++
theCorrelator->LoadTables();
```

The interferometer takes as inputs (1) pairs of antennas which
are to be included in the map, (2) correlation functions, 
(3) a table of arrival delays between the pairs,
and (4) the solution hypothesis (direct or reflected/refracted).
The map making routine returns a ROOT TH2D, and can be called like:

```c++
std::vector<TGraph*> corr_funcs = theCorrelator->GetCorrFunctions(pairs, wavforms);
auto arrival_delays = theCorrelator->GetArrivalDelays(pairs);
TH2D *map = theCorrelator->GetInterferometricMap(pairs, corr_funcs, arrival_delays, solution);
```

For more detailed discussion, see below, or see the example.

## Design Philosopy

The correlator tries to conceptually separate (1) *making* the maps
from (2) *calculating* the arrival times. For this reason, arrival times
are stored in tables that are loaded by the correlator.
Construction of the arrival time tables is technically up to user's preferences, 
though we usually use AraSim ray tracer.
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

Calculating the correlation functions is actually separate
from calling the map making rountines. This is on purpose.
So if you need to make >=1 map (e.g. D and R, multiple radii, etc.),
it is always in our favor to "cache" the waveforms in this way.
This also makes it easier to examing the correlation functions
directly for debugging purposes.
The arrival delays between pairs is also cached, as this doesn't change
from event to event.

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

This underwent significant optimization in Oct 2024 by Brian Clark and Marco Muzio.
We added caching of the arrival delays, and were much more careful about the GetMap function,
and use of refernces instead of passing objects in memory, etc.
We managed to speed things up by about a factor of 5-6, albiet at the cost
of slightly lowered readability, and less safety (fewer if statements).

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
- direct tables path: path to the file containing the direct ray tracing solutions solution timing table
- reflected tables path: path to the file containing the reflected/refracted ray tracing solution timing tables

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

## Making Correlation Maps

First, we must calculate the correlation functions.
There are two arguments to the `GetCorrFuncs` routine:
- pairs: a list of pairs of antennas to be correlated together, with pair indices (as keys) to their respective antennas (as values)
- interpolated waveforms: a std::map of interpolated waveforms, with antenna numbers as keys and waveforms as values

And one optional argument:
- whether or not to apply hilbert smoothing to the correlation function

There are three arguments to the `GetInterferometricMap` routine:
- pairs: a list of pairs of antennas to be correlated together, with pair indices (as keys) to their respective antennas (as values)
- correlation functions: a vector of correlation functions for each pair
- solution: what solution hypothesis to assume (direct or reflected/refracted)

We use C++ maps to make handling things easier. Maps are much
easier to use than you might think 
(they function remarkably like python dictionaries, but with stronger typing).
See e.g. [this page](https://www.freecodecamp.org/news/c-plus-plus-map-explained-with-examples/)
for a crash-course.

And one optional arguments:
- weights: weights to apply to each pair during the map making

### Waveforms

The waveforms need to be presented to the correlation routine as a map of
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

### Weights

This allows one to apply different weights to the pairs in a correlation map.
In most circumstances, one wants to apply an equal weight to all pairs.
That is, have the total map be the average of the individual maps,
so that the weight on each map is 1/num_pairs.
This is the default behavior of the interferometer if the weights are
left as an empty map, e.g. `std::map<int, double> weights = {}`.

### Hilbert Envelope Smoothing

In many cases, we want to smooth the correlation functions with an Hilbert envelope.
This is the default behavior in the correlator.
If you want to turn this behavior off, set the argument to `false`.

## To Do
1. ~Remove the IceModel dependence in the correlator. All the user needs to do is specify the tables.~ (Done, BAC Sep 2021)
2. ~Remove the unixTime dependence. E.g. let the user specify `numAntennas_` manually?~ (Done, BAC Sep 2021)
3. Add support for uniform binning in cos(theta) instead of theta.
4. ~Make sure unixTime is getting handled correctly~ (Done, See 1)
5. Add helper functions for "get peak," "get max correlation," etc.
6. ~Add ability to take weights for each pair in the `GetInterferometricMap` function.~ (Done, BAC).
7. Should we remove the stationID dependence all together?


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
