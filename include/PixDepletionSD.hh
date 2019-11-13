#ifndef PixDepletionSD_h
#define PixDepletionSD_h 1

#include "PixVSensitiveDetector.hh"

class PixDepletionSD : public PixVSensitiveDetector
{
    public:
        PixDepletionSD(const G4String& name, const G4String& hcName);
        virtual ~PixDepletionSD();

        void CollectElectrons(G4int nElectrons,
               G4int pixX,
               G4int pixY,
               G4ThreeVector, G4ThreeVector);

};

#endif
