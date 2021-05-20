#include "PixPrimaryGenerator.hh"
#include "PixParticleMessenger.hh"
#include "PixDetectorConstruction.hh"
#include "PixAngDistribution.hh"
#include "PixEneDistribution.hh"

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
    fEne = new PixEneDistribution();

    fPos->SetBiasRndm(fRnd);
    fAng->SetBiasRndm(fRnd);
    fEne->SetBiasRndm(fRnd);

    fAng->SetPosDistribution(fPos);

    fDef = G4Geantino::GeantinoDefinition();
    SetSourceType(DEFAULT_SOURCE_TYPE);

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

    // angle
    G4ParticleMomentum pDir = fAng->GenerateOne(); 
    
    // energy
    G4double E = fEne->GenerateOne(fDef);
    
    G4PrimaryParticle* particle = 
        new G4PrimaryParticle(fDef);
    particle->SetMomentumDirection(pDir);
    particle->SetKineticEnergy(E);

    // calculate initial position
    G4PrimaryVertex* vertex;
    if (fSourceType == "target") {
	vertex = GenerateTargetVertex(dc, pDir);
    } else if (fSourceType == "point") {
	vertex = GeneratePointVertex(dc);
    } else if (fSourceType == "diffuse") {
        vertex = GenerateDiffuseVertex(dc);
    }

    vertex->SetPrimary(particle);
    anEvent->AddPrimaryVertex(vertex);
}

G4PrimaryVertex* PixPrimaryGenerator::GenerateTargetVertex(PixDetectorConstruction* dc, G4ThreeVector pDir)
{
    G4double px = pDir.x();
    G4double py = pDir.y();
    G4double pz = pDir.z();
    G4double z0 = dc->GetGlassZ() + dc->GetShieldingZ() + dc->GetShieldingGap();
    
    // offset position
    G4double halfXY = dc->GetPixXY() / 2;
    fPos->SetHalfX(halfXY);
    fPos->SetHalfY(halfXY);
    G4ThreeVector offset = fPos->GenerateOne();

    G4double x0 = z0 * px / pz + offset.x();
    G4double y0 = z0 * py / pz + offset.y();

    return new G4PrimaryVertex(x0, y0, z0, 0);
}

G4PrimaryVertex* PixPrimaryGenerator::GeneratePointVertex(PixDetectorConstruction* dc) 
{
    G4double z0 = dc->GetGlassZ() + dc->GetShieldingZ() + dc->GetShieldingGap();
    return new G4PrimaryVertex(0, 0, z0, 0);
}

G4PrimaryVertex* PixPrimaryGenerator::GenerateDiffuseVertex(PixDetectorConstruction* dc)
{
    G4double z0 = dc->GetGlassZ() + dc->GetShieldingZ() + dc->GetShieldingGap();
    G4double halfXY = dc->GetWorldXY() / 2;
    fPos->SetHalfX(halfXY);
    fPos->SetHalfY(halfXY);
    G4ThreeVector pos = fPos->GenerateOne();

    return new G4PrimaryVertex(pos.x(), pos.y(), z0, 0);

}
