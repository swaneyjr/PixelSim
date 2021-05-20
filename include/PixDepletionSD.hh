#ifndef PixDepletionSD_h
#define PixDepletionSD_h 1

#include "PixVSensitiveDetector.hh"

#include "PixDetectorConstruction.hh"

class PixDepletionSD : public PixVSensitiveDetector
{
    public:
        PixDepletionSD(const G4String& name, const G4String& hcName, PixDetectorConstruction* dc);
        virtual ~PixDepletionSD();

        void CollectElectrons(G4int nElectrons,
               G4int pixX,
               G4int pixY,
               G4ThreeVector, 
               G4ThreeVector,
               G4String pname);

};

#endif
