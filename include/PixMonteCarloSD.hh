#ifndef PixMonteCarloSD_h
#define PixMonteCarloSD_h 1

#include "PixVSensitiveDetector.hh"

class PixMonteCarloSD : public PixVSensitiveDetector
{
    public:
        PixMonteCarloSD(const G4String& name, const G4String& hcName, PixDetectorConstruction* dc);
        virtual ~PixMonteCarloSD();

        void CollectElectrons(G4int nElectrons,
               G4int pixX,
               G4int pixY,
               G4ThreeVector xi, 
               G4ThreeVector xf);

};

#endif
