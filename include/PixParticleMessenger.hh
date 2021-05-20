#ifndef PixParticleMessenger_h
#define PixParticleMessenger_h 1

#include "G4UImessenger.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

class PixPrimaryGenerator;

class PixParticleMessenger : public G4UImessenger
{
    public:
        PixParticleMessenger(PixPrimaryGenerator* pg);
        virtual ~PixParticleMessenger();

        virtual void SetNewValue(G4UIcommand* cmd, G4String newValues);
        virtual G4String GetCurrentValue(G4UIcommand* command);

    private:
        PixPrimaryGenerator* fParticleGun;
        G4UIdirectory* fParticleDir;

	// source type
	G4UIcmdWithAString* cmdSourceType;

        // particle species
        G4UIcmdWithAString* cmdParticle;

        // angular distribution
        G4UIdirectory* fAngDir;
        G4UIcmdWithoutParameter* cmdAngIso;
        G4UIcmdWithADoubleAndUnit* cmdAngGauss;
        G4UIcmdWithoutParameter* cmdAngCosmic;
        G4UIcmdWithADoubleAndUnit* cmdMaxTheta;

        // energy distribution
        G4UIdirectory* fEneDir;
        G4UIcmdWithADoubleAndUnit* cmdEneMean;
        G4UIcmdWithADoubleAndUnit* cmdEneSigma;
        G4UIcmdWithAString* cmdEneHist;

};

#endif
