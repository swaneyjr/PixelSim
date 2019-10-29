#include "PixSensitiveDetector.hh"

#include "G4RunManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"


PixSensitiveDetector::PixSensitiveDetector(const G4String& name, const G4String& hcName)
    :   G4VSensitiveDetector(name),
        fHitsCollection(nullptr),
        fChargeSpread(DEFAULT_CHARGE_SPREAD)

{
    collectionName.insert(hcName);
}

PixSensitiveDetector::~PixSensitiveDetector()
{ }

G4bool PixSensitiveDetector::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
    G4double etot = aStep->GetTotalEnergyDeposit();
    if (etot == 0.0) return false; 

    G4StepPoint* preStep = aStep->GetPreStepPoint();
    G4StepPoint* postStep = aStep->GetPostStepPoint();

    //G4cout << preStep->GetPosition().getZ()/um << " um ->" << postStep->GetPosition().getZ()/um << "um ";

    const G4VTouchable* h = preStep->GetTouchable();

    //G4cout << "PV:" << h->GetVolume()->GetName() << " ";
    //G4cout << "e-:" << etot / (1.1*eV) << " ";
    G4bool depletion = (h->GetHistoryDepth() == DEPLETION_DEPTH); 
    G4int xLvl = 0;
    G4int yLvl = 1;

    if (depletion)
    {
        xLvl++;
        yLvl++;
    } 

    G4int pixX = h->GetReplicaNumber(xLvl);
    G4int pixY = h->GetReplicaNumber(yLvl);
    //G4cout << " x = " << pixX << " y = " << pixY << "\n";

    if (depletion) {
        // assume perfect efficiency and minimal cross-talk
        PixHit* newHit = new PixHit();
        
        newHit->SetX(pixX);
        newHit->SetY(pixY);
        newHit->SetE(etot);

        fHitsCollection->insert(newHit);

    } else {
        // TODO: divide the energy among nearby pixels
        
    }
    return true;
}

void PixSensitiveDetector::Initialize(G4HCofThisEvent* hce)
{
    fHitsCollection =
        new PixHitsCollection(SensitiveDetectorName, collectionName[0]);

    G4SDManager* sdMan = G4SDManager::GetSDMpointer();

    G4int hcID = sdMan->GetCollectionID(collectionName[0]);
    hce->AddHitsCollection(hcID, fHitsCollection);
}

void PixSensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{ }


