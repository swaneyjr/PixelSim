#include "PixMonteCarloSD.hh"

#include "PixHit.hh"
#include "PixDetectorConstruction.hh"
#include "PixMonteCarlo.hh"

#include "Randomize.hh"


PixMonteCarloSD::PixMonteCarloSD(const G4String& name, const G4String& hcName, PixDetectorConstruction* dc)
    :   PixVSensitiveDetector(name, hcName)
{  
    fRes = dc->GetResXY();

    fPixElectrons = new G4int[fRes*fRes]();
    fMC = new PixMonteCarlo(dc);
}

PixMonteCarloSD::~PixMonteCarloSD()
{ 
    delete [] fPixElectrons; 
    delete fMC;
}

void PixMonteCarloSD::CollectElectrons(G4int nElectrons, G4int pixX, G4int pixY, G4ThreeVector xi, G4ThreeVector xf)
{

    //G4cout << "electrons: " << nElectrons << "\n";
    for (G4int i=0; i<nElectrons; i++)
    {    
        // pick a random point along the step
        G4double rand = G4UniformRand();
        G4ThreeVector xMC = xi*rand + xf*(1-rand);

        PixCoords dxy = fMC->GenerateHit(xMC);

        if (&dxy == &PixCoordsNull) continue;

        G4int iPixX = pixX + dxy.first;
        G4int iPixY = pixY + dxy.second;

        if (std::min(iPixX, iPixY) >= 0 && std::max(iPixX, iPixY) < fRes)
            (*( fPixElectrons + iPixX + fRes * iPixY ))++;
    }

    for (G4int ix=0; ix<fRes; ix++)
    {
        for (G4int iy=0; iy<fRes; iy++)
        {
            G4int ePix = *( fPixElectrons + ix + fRes*iy );
            if ( ePix )
            {
                PixHit* newHit = new PixHit();
                
                newHit->SetX(ix);
                newHit->SetY(iy);
                newHit->SetN(ePix);

                fHitsCollection->insert(newHit);

                *( fPixElectrons + ix + fRes*iy  ) = 0;
            }
        }
    }
}
