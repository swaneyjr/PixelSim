#ifndef PixPhysMessenger_h
#define PixPhysMessenger_h 1

#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

class G4UIcommand;
class PixModularPhysicsList;

class PixPhysMessenger : public G4UImessenger
{
    public:
        PixPhysMessenger(PixModularPhysicsList* phys);
        virtual ~PixPhysMessenger();

        virtual void SetNewValue(G4UIcommand* cmd, G4String values);
        virtual G4String GetCurrentValue(G4UIcommand* cmd);

    private:
        PixModularPhysicsList* fPhys;

        G4UIdirectory* fCutDirectory;

        G4UIcmdWithADoubleAndUnit* cmdMaxStepSensor;
        G4UIcmdWithADoubleAndUnit* cmdRangeCutWorld;
        G4UIcmdWithADoubleAndUnit* cmdRangeCutGlassShielding;
	G4UIcmdWithADoubleAndUnit* cmdRangeCutSensor;
        G4UIcmdWithADoubleAndUnit* cmdMinProdThresh;

};


#endif
