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

    fPixElectrons = new G4int[fResXY * fResXY]();
}

PixDigitizer::~PixDigitizer()
{ 
    delete [] fPixElectrons;
}

void PixDigitizer::Digitize()
{
    
    fDigiCollection = new PixDigiCollection
        ("PixDigitizer", "PixDigiCollection");

    
    G4DigiManager* digiMan = G4DigiManager::GetDMpointer();
    
    G4int depletionID = digiMan->GetHitsCollectionID("DepletionHC");
    G4int substrateID = digiMan->GetHitsCollectionID("SubstrateHC");

    PixHitsCollection* depletionHC = (PixHitsCollection*)
        (digiMan->GetHitsCollection(depletionID));
    PixHitsCollection* substrateHC = (PixHitsCollection*)
        (digiMan->GetHitsCollection(substrateID));


    if ( depletionHC && substrateHC ) {

        for (const auto hc : {depletionHC, substrateHC})
        {

            G4int nHits = hc->entries();

            for (G4int i=0; i<nHits; i++)
            {
                PixHit* hit = (*hc)[i];

                G4int x = hit->GetX();
                G4int y = hit->GetY();
                G4int n = hit->GetN();

                G4int index = (fResXY * y + x);

                *(fPixElectrons + index) += n;
            }
        
        }
        
        // create digi
        G4int nPix = fResXY * fResXY;
        for (G4int index=0; index<nPix; index++)
        {
            G4int nTot = *(fPixElectrons + index);
            *(fPixElectrons + index) = 0;

            if (nTot == 0) continue;

            G4int x = index % fResXY;
            G4int y = index / fResXY;

            PixDigi* digi = new PixDigi();

            digi->SetX(x);
            digi->SetY(y);
            digi->SetN(nTot);

            fDigiCollection->insert(digi);
        }
    
    }
    
    StoreDigiCollection(fDigiCollection);
}
