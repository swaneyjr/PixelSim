#include "PixDepletionSD.hh"

#include "PixHit.hh"

PixDepletionSD::PixDepletionSD(const G4String& name, const G4String& hcName, PixDetectorConstruction* dc)
    :   PixVSensitiveDetector(name, hcName, dc)
{ }

PixDepletionSD::~PixDepletionSD()
{ }

void PixDepletionSD::CollectElectrons(G4int nElectrons, G4int pixX, G4int pixY, G4ThreeVector, G4ThreeVector)
{
    // assume perfect efficiency and negligible cross-talk
    PixHit* newHit = new PixHit();
        
    newHit->SetX(pixX);
    newHit->SetY(pixY);
    newHit->SetN(nElectrons);

    fHitsCollection->insert(newHit);
}
