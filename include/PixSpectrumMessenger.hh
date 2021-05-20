#ifndef PixSpectrumMessenger_h
#define PixSpectrumMessenger_h 1

#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithABool.hh"

#include <vector>

class G4UIcommand;

class PixSpectrumMessenger : public G4UImessenger
{
    public:
        PixSpectrumMessenger();
        virtual ~PixSpectrumMessenger();

        virtual void SetNewValue(G4UIcommand* cmd, G4String newValues);
        virtual G4String GetCurrentValue(G4UIcommand* cmd);

    private:
        void RunSpectrum(G4double val, G4bool tVal);
        
    private:
        G4UIdirectory* fSpectrumDir;

        G4UIcmdWithAString* cmdSpectrumPath;
	G4UIcmdWithABool* cmdDiscrete;
        G4UIcmdWithAnInteger* cmdResTot;
        G4UIcmdWithAnInteger* cmdRunN;
        G4UIcmdWithADoubleAndUnit* cmdRunT;

        G4String fSpectrumPath;
	G4bool fDiscrete;
        G4int fResTot;

};


#endif
