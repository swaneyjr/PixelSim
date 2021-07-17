#ifndef PixEmMuElecPhysics_h
#define PixEmMuElecPhysics_h 1

#include "G4VPhysicsConstructor.hh"
#include "globals.hh"

class PixEmMuElecPhysics : public G4VPhysicsConstructor
{
    public:
        PixEmMuElecPhysics(G4int ver = 1);
	PixEmMuElecPhysics(G4int ver, const G4String& name);
	virtual ~PixEmMuElecPhysics();

	virtual void ConstructParticle();
	virtual void ConstructProcess();

    private:
	G4int verbose;
};


#endif
