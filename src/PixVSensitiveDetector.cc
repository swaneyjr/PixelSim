#include "PixVSensitiveDetector.hh"

#include "PixDetectorConstruction.hh"
#include "RootIO.hh"

#include "G4AffineTransform.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"

#include <math.h>

PixVSensitiveDetector::PixVSensitiveDetector(const G4String& name, const G4String& hcName, PixDetectorConstruction* dc)
    :   G4VSensitiveDetector(name),
        fHitsCollection(nullptr)
{
    collectionName.insert(hcName);
    fIonization = new PixIonization(dc);
    fELeftover = 0;
}

PixVSensitiveDetector::~PixVSensitiveDetector()
{ 
    delete fIonization;
}

G4bool PixVSensitiveDetector::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{

    G4String pname = aStep->GetTrack()
        ->GetParticleDefinition()
        ->GetParticleName();

    G4double etot = aStep->GetTotalEnergyDeposit();
    if (etot == 0.0) return false;  
    
    const G4VTouchable* h = aStep->GetPreStepPoint()->GetTouchable();

    G4int pixX = h->GetReplicaNumber(1);
    G4int pixY = h->GetReplicaNumber(2);
   
    // first find coordinates in the logical volume
    G4ThreeVector xi = aStep->GetPreStepPoint()->GetPosition();
    G4ThreeVector xf = aStep->GetPostStepPoint()->GetPosition(); 

    // get leftover energy from last step
    if (xi == fxf) {
        etot += fELeftover;	
    }
    fxf = xf;
    fELeftover = 0;

    const G4AffineTransform* localTransform =
        h->GetHistory()->GetPtrTopTransform();

    // depletion region is the xy plane
    G4ThreeVector xiLocal = localTransform->TransformPoint(xi);
    G4ThreeVector xfLocal = localTransform->TransformPoint(xf); 

    // now handle electron collection
    G4int nElectrons = fIonization->Ionize(etot, &fELeftover);
    //G4cout << "E: " << etot/eV << " -> e-: " << nElectrons << "\n";
    CollectElectrons(nElectrons, pixX, pixY, xiLocal, xfLocal, pname); 
    
    //G4cout << preStep->GetPosition().getZ()/um << " um -> " << postStep->GetPosition().getZ()/um << "um ";
    //G4cout << "LV:" << h->GetVolume()->GetLogicalVolume()->GetName() << "\n";
    //G4cout << "e-:" << etot / (SI_BAND_GAP) << " ";

    //G4cout << " x = " << pixX << " y = " << pixY << "\n";

    return true;

}

void PixVSensitiveDetector::Initialize(G4HCofThisEvent* hce)
{
    fHitsCollection =
        new PixHitsCollection(SensitiveDetectorName, collectionName[0]);

    G4SDManager* sdMan = G4SDManager::GetSDMpointer();

    G4int hcID = sdMan->GetCollectionID(collectionName[0]);
    hce->AddHitsCollection(hcID, fHitsCollection);
}

void PixVSensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{ }

