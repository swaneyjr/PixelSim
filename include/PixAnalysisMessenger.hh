#ifndef PixAnalysisMessenger_h
#define PixAnalysisMessenger_h 1

#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"

#include "RootIO.hh"

class G4UIcommand;

class PixAnalysisMessenger : public G4UImessenger
{
    public:
        PixAnalysisMessenger(RootIO* root);
        virtual ~PixAnalysisMessenger();

        virtual void SetNewValue(G4UIcommand* cmd, G4String values);
        virtual G4String GetCurrentValue(G4UIcommand* cmd);

    private:
        RootIO* fROOT;

        G4UIdirectory* fROOTDirectory;

        G4UIcmdWithAnInteger* cmdMinPix;

};


#endif
