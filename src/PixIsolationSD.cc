#include "PixIsolationSD.hh"

#include "PixDetectorConstruction.hh"
#include "PixHit.hh"

#include "Randomize.hh"

#include <math.h>

PixIsolationSD::PixIsolationSD(const G4String& name, const G4String& hcName, PixDetectorConstruction* dc)
    :   PixVSensitiveDetector(name, hcName, dc)
{ 
    fDiffusionLength = dc->GetDiffusionLength() / sqrt(3); // correct for 1D 
    fEpiDepth = dc->GetPixZ() - dc->GetPixDepl();
}

PixIsolationSD::~PixIsolationSD()
{ }

void PixIsolationSD::CollectElectrons(G4int nElectrons, G4int pixX, G4int pixY, G4ThreeVector xi, G4ThreeVector xf, G4String pname)
{
    // collection efficiency can be reduced to a simple
    // analytical result

    G4double zdep = fEpiDepth / 2 - (xi.z() + xf.z()) / 2;

    G4double eff = cosh(zdep/fDiffusionLength) / cosh(fEpiDepth/fDiffusionLength);
    G4int nCollected = 0;

    // makeshift binomial
    for (G4int i=0; i<nElectrons; i++)
    {
        if (G4UniformRand() < eff) nCollected++;
    }

    PixHit* newHit = new PixHit();
        
    newHit->SetX(pixX);
    newHit->SetY(pixY);
    //std::cout << pname << ": (" << pixX << ", " << pixY << ", " << nCollected << ")\n";
    if (nCollected < 0) std::cout << "nCollected: " << nCollected << "\n";
    newHit->SetN(nCollected);
    newHit->SetParticleName(pname);

    fHitsCollection->insert(newHit);
}
