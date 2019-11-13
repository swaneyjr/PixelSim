#ifndef PixMonteCarlo_h
#define PixMonteCarlo_h 1

#include "G4ThreeVector.hh"
#include "globals.hh"

#include <utility>

class PixDetectorConstruction;

typedef std::pair<G4int, G4int> PixCoords;
extern PixCoords PixCoordsNull;

class PixMonteCarlo
{
    public:
        PixMonteCarlo(PixDetectorConstruction* dc);
        virtual ~PixMonteCarlo();

        const PixCoords GenerateHit(G4ThreeVector xlocal);

    private:
        void GenerateGrid();
        const PixCoords FastMC(G4ThreeVector x);
        const PixCoords InterpolateNearest(G4ThreeVector x);
        const PixCoords InterpolateLinear(G4ThreeVector x);

    private:
        G4double fStepSize;
        G4double fProbRecombination;
        G4double fLimXY;
        G4double fLimZ;
        G4double fIsoDepth;

        G4int fSpacesXY;
        G4int fSpacesZ;
        G4int fRes;
        G4int fMCElectrons;
        G4String fInterpolation;

        G4int* fMCGrid;

};


#endif
