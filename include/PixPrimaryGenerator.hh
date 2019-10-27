#ifndef PixPrimaryGenerator_h
#define PixPrimaryGenerator_h 1

#include "G4Event.hh"
#include "G4VPrimaryGenerator.hh"
#include "G4SPSPosDistribution.hh"
#include "G4SPSEneDistribution.hh"
#include "G4SPSRandomGenerator.hh"

class PixParticleMessenger;
class PixAngDistribution;

class PixPrimaryGenerator : public G4VPrimaryGenerator
{
    public:
        PixPrimaryGenerator();
        virtual ~PixPrimaryGenerator();

        virtual void GeneratePrimaryVertex(G4Event *anEvent);

        inline G4SPSPosDistribution* GetPosDist() const { return fPos; };
        inline PixAngDistribution* GetAngDist() const { return fAng; };
        inline G4SPSEneDistribution* GetEneDist() const { return fEne; };
        inline G4SPSRandomGenerator* GetRandGen() const { return fRnd; };

        inline void SetParticleDefinition(G4ParticleDefinition* def) { fDef = def; };
        inline G4ParticleDefinition* GetParticleDefinition() const { return fDef; }; 

        
    private:
        PixParticleMessenger* fMessenger;

        G4SPSPosDistribution* fPos;
        PixAngDistribution* fAng;
        G4SPSEneDistribution* fEne;
        G4SPSRandomGenerator* fRnd;

        G4ParticleDefinition* fDef;
};


#endif
