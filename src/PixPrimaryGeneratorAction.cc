#include "PixPrimaryGeneratorAction.hh"
#include "PixPrimaryGenerator.hh"

PixPrimaryGeneratorAction::PixPrimaryGeneratorAction()
:   G4VUserPrimaryGeneratorAction()
{
    fParticleGun = new PixPrimaryGenerator();
}

PixPrimaryGeneratorAction::~PixPrimaryGeneratorAction()
{
    delete fParticleGun;
}

void PixPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    fParticleGun->GeneratePrimaryVertex(anEvent);
}
