#include "PixDigitizer.hh"

#include "PixHit.hh"

#include "G4RunManager.hh"
#include "G4DigiManager.hh"

PixDigitizer::PixDigitizer(G4String name)
    :   G4VDigitizerModule(name)
{ 
    G4String colName = "PixDigiCollection";
    collectionName.push_back(colName);
}

PixDigitizer::~PixDigitizer()
{ }

void PixDigitizer::Digitize()
{
    
    fDigiCollection = new PixDigiCollection
        ("PixDigitizer", "PixDigiCollection");

    G4DigiManager* digiMan = G4DigiManager::GetDMpointer();
    
    G4int deplID = digiMan->GetHitsCollectionID("DepletionHC");
    G4int epiID = digiMan->GetHitsCollectionID("EpiHC");

    PixHitsCollection* deplHC = (PixHitsCollection*)
        (digiMan->GetHitsCollection(deplID));
    PixHitsCollection* epiHC = (PixHitsCollection*)
        (digiMan->GetHitsCollection(epiID)); 

    if ( deplHC && epiHC ) {

        G4int minX = 1000000;
        G4int maxX = 0;
        G4int minY = 1000000;
        G4int maxY = 0;

        // first find bounding box
        for (const auto hc : {deplHC, epiHC})
        {
            G4int nHits = hc->entries();

            for (G4int i=0; i<nHits; i++)
            {
                PixHit* hit = (*hc)[i];
                
                minX = std::min(minX, hit->GetX());
                maxX = std::max(maxX, hit->GetX());
                minY = std::min(minY, hit->GetY());
                maxY = std::max(maxY, hit->GetY());
            }
        }

        G4int rangeX = maxX - minX + 1;
        G4int rangeY = maxY - minY + 1;

        G4int nPix = rangeX * rangeY;
        
        if (nPix > 0) 
        {
            fPixTot.resize(nPix);
            fPixProton.resize(nPix);
            fPixElectron.resize(nPix);
            fPixAlpha.resize(nPix);
            fPixPion.resize(nPix);
            fPixDeuteron.resize(nPix);
            fPixOther.resize(nPix);
        }

        fPixTot.clear();
        fPixProton.clear();
        fPixElectron.clear();
        fPixAlpha.clear();
        fPixPion.clear();
        fPixDeuteron.clear();
        fPixOther.clear();

        // find total electron counts
        for (const auto hc : {deplHC, epiHC})
        {

            G4int nHits = hc->entries();

            for (G4int i=0; i<nHits; i++)
            {
                PixHit* hit = (*hc)[i];

                G4int x = hit->GetX() - minX;
                G4int y = hit->GetY() - minY;
                G4int n = hit->GetN();
                G4String pname = hit->GetParticleName();

                G4int index = (rangeX * y + x);

                fPixTot[index] += n;
                
                if (pname == "proton")
                    fPixProton[index] += n;
                else if (pname == "e-" || pname == "e+")
                    fPixElectron[index] += n;
                else if (pname == "alpha")
                    fPixAlpha[index] += n;
                else if (pname == "pi+" || pname == "pi-" || pname == "pi0")
                    fPixPion[index] += n;
                else if (pname == "deuteron")
                    fPixDeuteron[index] += n;
                else
                    fPixOther[index] += n;
            }
        
        }
        
        // create digi
        for (G4int index=0; index<nPix; index++)
        {
            G4int nTot = fPixTot[index];
            if (nTot == 0) continue;

            G4int x = index % rangeX + minX;
            G4int y = index / rangeX + minY;

            PixDigi* digi = new PixDigi();

            digi->SetX(x);
            digi->SetY(y);
            digi->SetN(nTot);

            digi->SetNProton(fPixProton[index]);
            digi->SetNElectron(fPixElectron[index]);
            digi->SetNAlpha(fPixAlpha[index]);
            digi->SetNPion(fPixPion[index]);
            digi->SetNDeuteron(fPixDeuteron[index]);
            digi->SetNOther(fPixOther[index]);

            fDigiCollection->insert(digi);
        }
    
    }
    
    StoreDigiCollection(fDigiCollection);
}
