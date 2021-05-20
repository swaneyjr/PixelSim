#include "PixEventAction.hh"

#include "G4DigiManager.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "globals.hh"

#include "PixDigitizer.hh"
#include "RootIO.hh"

PixEventAction::PixEventAction()
{ 
    fDigitizer = new PixDigitizer("PixDigitizer");
    G4DigiManager::GetDMpointer()->AddNewModule(fDigitizer);
}

PixEventAction::~PixEventAction()
{ }

void PixEventAction::BeginOfEventAction(const G4Event* anEvent)
{ 
    G4int evtID = anEvent->GetEventID();
    G4cout << "Event: " << evtID << "\r" << std::flush;
}

void PixEventAction::EndOfEventAction(const G4Event* anEvent)
{ 
    fDigitizer->Digitize();

    G4DigiManager* digiMan = G4DigiManager::GetDMpointer();    

    G4int digiID = digiMan->GetDigiCollectionID("PixDigitizer/PixDigiCollection");
    PixDigiCollection* dc = (PixDigiCollection*)
        digiMan->GetDigiCollection(digiID);

    RootIO* root = RootIO::GetInstance();

    G4PrimaryVertex* primaryVertex = anEvent->GetPrimaryVertex();
    if ( primaryVertex )
    {
        G4int nParticles = primaryVertex->GetNumberOfParticle();
        for (G4int i=0; i<nParticles; i++)
        {
            G4PrimaryParticle* particle = primaryVertex->GetPrimary(i);
            root->AddPrimary(particle);
        }
    }

    if ( dc )
        root->AddDigits(dc);

    root->WriteEvent();
}
