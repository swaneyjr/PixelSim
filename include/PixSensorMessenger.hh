#ifndef PixSensorMessenger_h
#define PixSensorMessenger_h 1

#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "globals.hh"

class G4UIcommand;
class PixDetectorConstruction;

class PixSensorMessenger : public G4UImessenger
{
    public:
        PixSensorMessenger(PixDetectorConstruction* dc);
        virtual ~PixSensorMessenger();

        virtual void SetNewValue(G4UIcommand* cmd, G4String newValues);
        virtual G4String GetCurrentValue(G4UIcommand* cmd);

    private:
        PixDetectorConstruction* fDC;
        
        // geometry
        G4UIdirectory* fGeometryDir;

        G4UIcmdWithAnInteger* cmdResXY;
        G4UIcmdWithADoubleAndUnit* cmdPixXY;
        G4UIcmdWithADoubleAndUnit* cmdPixZ; 
        G4UIcmdWithADoubleAndUnit* cmdGlassZ;

        // readout
        G4UIdirectory* fReadoutDir;

        G4UIcmdWithADoubleAndUnit* cmdPixDepl;
        G4UIcmdWithAString* cmdDiffModel;
        G4UIcmdWithADoubleAndUnit* cmdDiffLen;
        G4UIcmdWithADoubleAndUnit* cmdDTI;
        G4UIcmdWithADoubleAndUnit* cmdDiffStep;

        // monte carlo
        G4UIdirectory* fFastMCDir;
 
        G4UIcmdWithAString* cmdInterpolation;
        G4UIcmdWithADoubleAndUnit* cmdGridSpacing;
        G4UIcmdWithAnInteger* cmdMaxSpread;
        G4UIcmdWithAnInteger* cmdSampleSize;
        G4UIcmdWithAString* cmdFromFile;

};

#endif
