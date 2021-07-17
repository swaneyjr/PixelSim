#ifndef PixEneDistribution_h
#define PixEneDistribution_h 1

#include "G4SPSEneDistribution.hh"
#include "G4ParticleDefinition.hh"
#include "G4PhysicsOrderedFreeVector.hh"

class PixEneDistribution : public G4SPSEneDistribution
{
    public:
	PixEneDistribution();
	~PixEneDistribution();

	void SetEnergyDisType(G4String aType);
	const G4String GetEnergyDisType();

	void DiscreteEnergyHistoFile(G4String aFile);
	G4double GenerateOne(G4ParticleDefinition*);
	G4double GetProbability(G4double e);

    private:
	G4bool fDiscrete;
	G4PhysicsOrderedFreeVector fDiscreteEnergyCDF;
	G4double fPhiTot;
	G4PhysicsOrderedFreeVector ZeroPhysVector;

    private:
	G4double GenerateDiscreteEnergies();
	
	

};

#endif
