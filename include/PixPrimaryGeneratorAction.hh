#ifndef PixPrimaryGeneratorAction_h
#define PixPrimaryGeneratorAction_h 1

#include "G4Event.hh"
#include "G4VUserPrimaryGeneratorAction.hh"

class PixPrimaryGenerator;

class PixPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
    public:
        PixPrimaryGeneratorAction();
        ~PixPrimaryGeneratorAction();

        virtual void GeneratePrimaries(G4Event*);

        inline PixPrimaryGenerator* GetPrimaryGenerator() { return fParticleGun; }

    private:
        PixPrimaryGenerator* fParticleGun;
};

#endif
