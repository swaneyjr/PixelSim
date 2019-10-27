#ifndef PixAngDistribution_h
#define PixAngDistribution_h 1

#include "G4SPSAngDistribution.hh"


class PixAngDistribution : public G4SPSAngDistribution
{
    public:
        PixAngDistribution();
        ~PixAngDistribution();

        void SetAngDistType(G4String aType);
        G4String GetDistType();
        G4ParticleMomentum GenerateOne();
        
    private:
        void GenerateCosmicFlux(G4ParticleMomentum& momentum);

    private:
        G4bool fCosmic;

        static constexpr G4double THETA_THRESH = 1e-4;
};

#endif
