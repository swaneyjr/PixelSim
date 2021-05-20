#ifndef PixMonteCarloSD_h
#define PixMonteCarloSD_h 1

#include "PixVSensitiveDetector.hh"

class PixMonteCarlo;

class PixMonteCarloSD : public PixVSensitiveDetector
{
    public:
        PixMonteCarloSD(const G4String& name, const G4String& hcName, PixDetectorConstruction* dc);
        virtual ~PixMonteCarloSD();

        void CollectElectrons(G4int nElectrons,
               G4int pixX,
               G4int pixY,
               G4ThreeVector xi, 
               G4ThreeVector xf,
               G4String pname);

    private:
        PixMonteCarlo* fMC;
        G4int* fPixElectrons;

        G4int fRes;
	G4int fSgn;



};

#endif
