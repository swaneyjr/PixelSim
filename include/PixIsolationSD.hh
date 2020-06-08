#ifndef PixIsolationSD_h
#define PixIsolationSD_h 1

#include "PixVSensitiveDetector.hh"

class PixIsolationSD : public PixVSensitiveDetector
{
    public:
        PixIsolationSD(const G4String& name, const G4String& hcName, PixDetectorConstruction* dc);
        virtual ~PixIsolationSD();

        void CollectElectrons(G4int nElectrons,
               G4int pixX,
               G4int pixY,
               G4ThreeVector xi, 
               G4ThreeVector xf,
               G4String pname);

    private:
        G4double fEpiDepth;
        G4double fDiffusionLength;

};

#endif
