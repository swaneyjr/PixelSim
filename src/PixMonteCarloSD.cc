#include "PixMonteCarloSD.hh"

#include "PixHit.hh"
#include "PixDetectorConstruction.hh"

#include "G4RandomDirection.hh"
#include "Randomize.hh"

#include <math.h>

PixMonteCarloSD::PixMonteCarloSD(const G4String& name, const G4String& hcName, PixDetectorConstruction* dc)
    :   PixVSensitiveDetector(name, hcName, dc)
{ }

PixMonteCarloSD::~PixMonteCarloSD()
{ }

void PixMonteCarloSD::CollectElectrons(G4int nElectrons, G4int pixX, G4int pixY, G4ThreeVector xi, G4ThreeVector xf)
{
    
    // get parameters from detector construction
    G4double stepSize = fDC->GetMCStep();
    G4double diffusionLen = fDC->GetDiffusionLength();
    G4double xymax = fDC->GetPixXY() / 2;
    G4double zmax = fDC->GetPixZ() - fDC->GetPixDepl();
    G4double isoDepth = fDC->GetDTIDepth() - fDC->GetPixDepl();
    G4int res = fDC->GetResXY();

    G4double pRecombination = (stepSize*stepSize) / (diffusionLen*diffusionLen);

    //G4cout << "electrons: " << nElectrons << "\n";
    for (G4int i=0; i<nElectrons; i++)
    {    
        G4int iPixX = pixX;
        G4int iPixY = pixY;

        // pick a random point along the step
        G4double rand = G4UniformRand();
        G4ThreeVector mci = xi*rand + xf*(1-rand);
        G4ThreeVector mcf;
 
        // check each loop for recombination
        while (G4UniformRand() > pRecombination)
        {
                    
            G4ThreeVector step = stepSize * G4RandomDirection();
            mcf = mci + step;

            //G4cout << "(" << mci.x()/nm << ", " << mci.y()/nm << ", " << mci.z()/nm << ") -> ";
            //G4cout << "(" << mcf.x()/nm << ", " << mcf.y()/nm << ", " << mcf.z()/nm << ")\n";


            // create hit if electron has moved to depletion zone
            if (mcf.z() < 0)
            {
                if (std::min(iPixX, iPixY) >= 0 && std::max(iPixX, iPixY) < res)
                {

                    PixHit* newHit = new PixHit(); 

                    newHit->SetX(iPixX);
                    newHit->SetY(iPixY);
                    newHit->SetN(1); 

                    fHitsCollection->insert(newHit);
                }
                break;
            } 
            
            // reflection over boundaries
            if (mcf.z() > zmax)
            {
                G4double znew = 2*zmax - mcf.z();
                mcf.setZ(znew);
            }

            if (fabs(mcf.x()) > xymax)
            {
                G4int sgn = copysign(1, mcf.x());
                G4double xnew = mcf.x() - 2*sgn*xymax;
                if (mci.z() > isoDepth) 
                {
                    iPixX += sgn; 
                    mcf.setX(xnew);   
                }
                else
                    mcf.setX(-1*xnew);
            }
            if (fabs(mcf.y()) > xymax)
            {
                G4int sgn = copysign(1, mcf.y());
                G4double ynew = mcf.y() - 2*sgn*xymax;
                if (mci.z() > isoDepth) 
                {
                    iPixY += sgn;
                    mcf.setY(ynew);
                }
                else 
                    mcf.setY(-1*ynew);
            }

            // setup for next step
            mci = mcf;

        }

    }
}
