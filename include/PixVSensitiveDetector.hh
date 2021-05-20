#ifndef PixSensitiveDetector_h
#define PixSensitiveDetector_h 1

#include "G4VSensitiveDetector.hh"
#include "G4SystemOfUnits.hh"

#include "PixHit.hh"
#include "PixIonization.hh"

class PixDetectorConstruction;

class PixVSensitiveDetector : public G4VSensitiveDetector
{
    public:
        PixVSensitiveDetector(const G4String& name, const G4String& hcName, PixDetectorConstruction* dc);
        virtual ~PixVSensitiveDetector();

        virtual void Initialize(G4HCofThisEvent* hitCollection);
        G4bool ProcessHits(G4Step* step, G4TouchableHistory*);
        virtual void EndOfEvent(G4HCofThisEvent* hitCollection);

        virtual void CollectElectrons(G4int nElectrons, 
                G4int pixX, 
                G4int pixY, 
                G4ThreeVector xi, 
                G4ThreeVector xf,
                G4String pname) = 0;

    protected:
        PixHitsCollection* fHitsCollection;

    private:
        PixIonization* fIonization;
	G4ThreeVector fxf;
	G4double fELeftover;

};

#endif
