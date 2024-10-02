# Making Timing Tables

The program in this folder shows how to build a timing table,
specifically with AraSim.
The structure of the timing table is the only thing the correlator cares about.
You could build the timing table (with an identical structure)
using any other method you like.

## Compilation and Usage

Use of the script is straightforward. Because it relies on AraSim,
it needs to be built against those associated libraries.

The makefile is provided. Compile like:

```make
make makeRTArrivalTimeTables -f makeRTArrivalTimeTables.mk
```

To execute, you must provide (1) a station, (2) a radius, and (3) an output directory.

```sh
./makeRTArrivalTimeTables 2 300 /path/to/output/folder
```

Because this code relies on the AraSim IceModel class,
it will try and read support files from the `data` folder 
that is usually in the AraSim directory.
The easiest way to fix this is to symlink the AraSim data
folder into a local folder.
This must bed done *before* executing the code.

```sh
ln -s /path/to/AraSim/data data
```

## About the Code

The code to use the AraSim ray tracer is a bit tricky.
At a high level, this stems from the fact that AraSim
wants the antenna positions to be given in *earth* coordinates,
where normally AraRoot stores them in *station local* coordinates.

Part of this requires the ARA antennas be "displaced"
to a detector core in AraSim, which is usually 10000, 10000.
But it also means the depths need to be referenced to the ice surface.

In AraSim, this is normally handled by a combination of the
`ImportStationInfo` function in `Detector.cc` (defined [here](https://github.com/ara-software/AraSim/blob/2758c07dc56fb1a9a784e460306868f0f940b499/Detector.cc#L5945) and called [here](https://github.com/ara-software/AraSim/blob/2758c07dc56fb1a9a784e460306868f0f940b499/Detector.cc#L1923))
and `FlatToEarth_ARA_sharesurface` (defined [here](https://github.com/ara-software/AraSim/blob/2758c07dc56fb1a9a784e460306868f0f940b499/Detector.cc#L3524) and called [here](https://github.com/ara-software/AraSim/blob/2758c07dc56fb1a9a784e460306868f0f940b499/Detector.cc#L2165)).
These earth centric coordinates are then flattened back to a plane in the RaySolver
with the `Earth_to_Flat_same_angle` function (defined [here](https://github.com/ara-software/AraSim/blob/2758c07dc56fb1a9a784e460306868f0f940b499/RaySolver.cc#L39) 
and called [here](https://github.com/ara-software/AraSim/blob/2758c07dc56fb1a9a784e460306868f0f940b499/RaySolver.cc#L893)
).

So, the timing tables script essentially replicates the work of those two functions
(`ImportStationInfo` and `FlatToEarth_ARA_sharesurface`).
That is why it looks so convoluted.

## To Do
1. Try and eliminate assumptions about number of strings, antennas, etc. in the function.

More on 1: E.g. don't hardcode the loop to be over four strings, or 16 antennas, etc.
