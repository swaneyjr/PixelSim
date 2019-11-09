#include "PixIsolationSD.hh"

#include "PixDetectorConstruction.hh"
#include "PixHit.hh"

#include "Randomize.hh"

#include <math.h>

PixIsolationSD::PixIsolationSD(const G4String& name, const G4String& hcName, PixDetectorConstruction* dc)
    :   PixVSensitiveDetector(name, hcName, dc)
{ }

PixIsolationSD::~PixIsolationSD()
{ }

void PixIsolationSD::CollectElectrons(G4int nElectrons, G4int pixX, G4int pixY, G4ThreeVector xi, G4ThreeVector xf)
{
    // collection efficiency can be reduced to a simple
    // analytical result

    G4double zdep = (xi.z() + xf.z()) / 2;
    G4double ztot = fDC->GetPixZ() - fDC->GetPixDepl();
    G4double diffLen = fDC->GetDiffusionLength();

    G4double eff = cosh(zdep/diffLen) / cosh(ztot/diffLen);
    G4int nCollected = 0;

    // makeshift binomial
    for (G4int i=0; i<nElectrons; i++)
    {
        if (G4UniformRand() < eff) nCollected++;
    }

    PixHit* newHit = new PixHit();
        
    newHit->SetX(pixX);
    newHit->SetY(pixY);
    newHit->SetN(nCollected);

    fHitsCollection->insert(newHit);
}
