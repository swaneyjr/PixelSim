#include "PixDigitizer.hh"

#include "PixDetectorConstruction.hh"
#include "PixHit.hh"

#include "G4RunManager.hh"
#include "G4DigiManager.hh"

PixDigitizer::PixDigitizer(G4String name)
    :   G4VDigitizerModule(name)
{ 
    G4String colName = "PixDigiCollection";
    collectionName.push_back(colName);

    G4RunManager* runMan = G4RunManager::GetRunManager();
    PixDetectorConstruction* dc = (PixDetectorConstruction*)
        runMan->GetUserDetectorConstruction();

    fResXY = dc->GetResXY();

    fPixEnergies = new G4double[fResXY * fResXY]();
}

PixDigitizer::~PixDigitizer()
{ 
    delete [] fPixEnergies;
}

void PixDigitizer::Digitize()
{
    
    fDigiCollection = new PixDigiCollection
        ("PixDigitizer", "PixDigiCollection");

    
    G4DigiManager* digiMan = G4DigiManager::GetDMpointer();
    G4int sensorID = digiMan->GetHitsCollectionID("PixHitsCollection");
    PixHitsCollection* hc = (PixHitsCollection*)
        (digiMan->GetHitsCollection(sensorID));

    if ( hc ) {

        G4int nHits = hc->entries();

        for (G4int i=0; i<nHits; i++)
        {
            PixHit* hit = (*hc)[i];

            G4int x = hit->GetX();
            G4int y = hit->GetY();
            G4double e = hit->GetE();

            G4int index = (fResXY * y + x);

            *(fPixEnergies + index) += e;
        }
        
        // convert energies to electrons
        G4int nPix = fResXY * fResXY;
        for (G4int index=0; index<nPix; index++)
        {
            G4double eTot = *(fPixEnergies + index);
            G4int nElectrons = (G4int) (eTot / fBandGap);
            *(fPixEnergies + index) = 0;

            if (nElectrons == 0) continue;

            G4int x = index % fResXY;
            G4int y = index / fResXY;

            PixDigi* digi = new PixDigi();

            digi->SetX(x);
            digi->SetY(y);
            digi->SetN(nElectrons);

            fDigiCollection->insert(digi);
        }
    
    }
    
    StoreDigiCollection(fDigiCollection);
}
