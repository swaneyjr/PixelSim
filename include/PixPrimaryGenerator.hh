#ifndef PixPrimaryGenerator_h
#define PixPrimaryGenerator_h 1

#include "G4Event.hh"
#include "G4VPrimaryGenerator.hh"
#include "G4SPSPosDistribution.hh"
#include "G4SPSRandomGenerator.hh"

class PixParticleMessenger;
class PixEneDistribution;
class PixAngDistribution;
class PixDetectorConstruction;

class PixPrimaryGenerator : public G4VPrimaryGenerator
{
    public:
        PixPrimaryGenerator();
        virtual ~PixPrimaryGenerator();

        virtual void GeneratePrimaryVertex(G4Event *anEvent);

        inline G4SPSPosDistribution* GetPosDist() const { return fPos; };
        inline PixAngDistribution* GetAngDist() const { return fAng; };
        inline PixEneDistribution* GetEneDist() const { return fEne; };
        inline G4SPSRandomGenerator* GetRandGen() const { return fRnd; };

	inline void SetSourceType(G4String val) { fSourceType = val; };
	inline G4String GetSourceType() const { return fSourceType; };

        inline void SetParticleDefinition(G4ParticleDefinition* def) { fDef = def; };
        inline G4ParticleDefinition* GetParticleDefinition() const { return fDef; }; 

        
    private:
        PixParticleMessenger* fMessenger;

        G4SPSPosDistribution* fPos;
        PixAngDistribution* fAng;
        PixEneDistribution* fEne;
        G4SPSRandomGenerator* fRnd;

	G4String fSourceType;
        G4ParticleDefinition* fDef;

    public:
	static constexpr const char* DEFAULT_SOURCE_TYPE = "target";

    private:
	G4PrimaryVertex* GenerateTargetVertex(PixDetectorConstruction* dc, G4ThreeVector pDir);
	G4PrimaryVertex* GeneratePointVertex(PixDetectorConstruction* dc);
	G4PrimaryVertex* GenerateDiffuseVertex(PixDetectorConstruction* dc);
};


#endif
