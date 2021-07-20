# PixelSim

## Installation
 
To run this simulation, both ROOT and Geant4 must be installed. For ROOT, see [here](https://root.cern/install/); for Geant4, see [here](https://geant4-userdoc.web.cern.ch/UsersGuides/InstallationGuide/html/). After cloning this repository, the executable can then be built with:

```
mkdir build_dir/
cd build_dir/
cmake /path/to/PixelSim/
make -j2
```

where "2" can instead be substituted with the number of CPU cores to be used.

## Running the simulation

When the `pixelSim` executable has been built, running:

```
./pixelSim
```

will run the simulation in interactive mode using the visualization drivers configured by the Geant4 install. To instead run in batch mode, a macro must be supplied with an optional output file:

```
./pixelSim macros/example.mac [outfile.root]
```

Several example macro files are provided, and a full list of commands can be found in interactive mode. 