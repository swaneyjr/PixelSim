#ifndef PixSensorMessenger_h
#define PixSensorMessenger_h 1

#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "globals.hh"

#include "PixSensitiveDetector.hh"

class G4UIcommand;
class PixDetectorConstruction;

class PixSensorMessenger : public G4UImessenger
{
    public:
        PixSensorMessenger(PixDetectorConstruction* dc);
        virtual ~PixSensorMessenger();

        virtual void SetNewValue(G4UIcommand* cmd, G4String newValues);
        virtual G4String GetCurrentValue(G4UIcommand* cmd);

        inline void SetSensitiveDetector(PixSensitiveDetector* sd) { fSD = sd; };
        inline PixSensitiveDetector* GetSensitiveDetector() const { return fSD; };

    private:
        PixDetectorConstruction* fDC;
        PixSensitiveDetector* fSD;
        
        // geometry
        G4UIdirectory* fGeometryDir;

        G4UIcmdWithAnInteger* cmdResXY;
        G4UIcmdWithADoubleAndUnit* cmdPixXY;
        G4UIcmdWithADoubleAndUnit* cmdPixZ; 
        G4UIcmdWithADoubleAndUnit* cmdGlassZ;

        // readout
        G4UIdirectory* fReadoutDir;

        G4UIcmdWithADoubleAndUnit* cmdPixDepl;
        G4UIcmdWithADoubleAndUnit* cmdChargeSpread;
        G4UIcmdWithADoubleAndUnit* cmdMaxTheta;
};

#endif
