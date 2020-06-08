#include "PixDepletionSD.hh"

#include "PixHit.hh"

PixDepletionSD::PixDepletionSD(const G4String& name, const G4String& hcName)
    :   PixVSensitiveDetector(name, hcName)
{ }

PixDepletionSD::~PixDepletionSD()
{ }

void PixDepletionSD::CollectElectrons(G4int nElectrons, G4int pixX, G4int pixY, G4ThreeVector, G4ThreeVector, G4String pname)
{
    // assume perfect efficiency and negligible cross-talk
    PixHit* newHit = new PixHit();
        
    newHit->SetX(pixX);
    newHit->SetY(pixY);
    newHit->SetN(nElectrons);
    newHit->SetParticleName(pname);

    fHitsCollection->insert(newHit);
}
