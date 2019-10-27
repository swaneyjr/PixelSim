#include "PixPrimaryGenerator.hh"
#include "PixParticleMessenger.hh"
#include "PixDetectorConstruction.hh"
#include "PixAngDistribution.hh"

#include "G4Geantino.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4RunManager.hh"


PixPrimaryGenerator::PixPrimaryGenerator()
{ 
    fMessenger = new PixParticleMessenger(this);

    fRnd = new G4SPSRandomGenerator();
    fPos = new G4SPSPosDistribution();
    fAng = new PixAngDistribution();
    fEne = new G4SPSEneDistribution();

    fPos->SetBiasRndm(fRnd);
    fAng->SetBiasRndm(fRnd);
    fEne->SetBiasRndm(fRnd);

    fAng->SetPosDistribution(fPos);

    fDef = G4Geantino::GeantinoDefinition();

    // configure position for single pixel offset
    fPos->SetPosDisType("Plane");
    fPos->SetPosDisShape("Square");
}

PixPrimaryGenerator::~PixPrimaryGenerator()
{ 
    delete fMessenger;
    delete fRnd;
    delete fPos;
    delete fAng;
    delete fEne;
}

void PixPrimaryGenerator::GeneratePrimaryVertex(G4Event* anEvent)
{ 

    // get geometry
    G4RunManager* runMan = G4RunManager::GetRunManager();
    PixDetectorConstruction* dc = (PixDetectorConstruction*)
        runMan->GetUserDetectorConstruction();

    // offset position
    G4double halfXY = dc->GetPixXY() / 2;
    fPos->SetHalfX(halfXY);
    fPos->SetHalfY(halfXY);
    G4ThreeVector offset = fPos->GenerateOne();

    // angles
    fAng->SetMaxTheta(dc->GetMaxTheta());

    G4ParticleMomentum pDir = fAng->GenerateOne();
    G4double px = pDir.x();
    G4double py = pDir.y();
    G4double pz = pDir.z();
    
    G4double E = fEne->GenerateOne(fDef);

    // calculate initial position
    G4double z0 = dc->GetGlassZ();
    G4double x0 = z0 * px / pz + offset.x();
    G4double y0 = z0 * py / pz + offset.y();

    // create the vertex
    G4PrimaryVertex* vertex =
        new G4PrimaryVertex(x0, y0, z0, 0);

    G4PrimaryParticle* particle = 
        new G4PrimaryParticle(fDef);
    particle->SetMomentumDirection(pDir);
    particle->SetKineticEnergy(E);

    vertex->SetPrimary(particle);
    anEvent->AddPrimaryVertex(vertex);
}

