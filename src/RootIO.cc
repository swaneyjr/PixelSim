#include "RootIO.hh"

#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"

static RootIO* fInstance = nullptr;

RootIO::RootIO()
    :   fAnalysisManager(G4AnalysisManager::Instance()),
        fParticles()
{ } 

RootIO::~RootIO()
{
    delete fAnalysisManager;
}

RootIO* RootIO::GetInstance()
{
    if (fInstance == nullptr)
    {
        fInstance = new RootIO();
    }
    return fInstance;
}

void RootIO::BeginFile(G4String& filename)
{
    // specify directory for primaries before creating file
    fAnalysisManager->SetHistoDirectoryName("primaries");
    
    fAnalysisManager->OpenFile(filename);

    // now make ntuple
    fAnalysisManager->CreateNtuple("hits","Pixel hits");

    fAnalysisManager->CreateNtupleSColumn("primary_name");
    fAnalysisManager->CreateNtupleDColumn("primary_ke");
    fAnalysisManager->CreateNtupleDColumn("primary_theta");
    fAnalysisManager->CreateNtupleDColumn("primary_phi"); 

    fAnalysisManager->CreateNtupleIColumn("pix_x", fX);
    fAnalysisManager->CreateNtupleIColumn("pix_y", fY);
    fAnalysisManager->CreateNtupleIColumn("pix_e", fN);
 
    fAnalysisManager->FinishNtuple();

}

void RootIO::AddDigits(PixDigiCollection* dc)
{
    G4int nDigi = dc->entries();

    for (G4int i=0; i<nDigi; i++)
    {
        PixDigi* digi = (*dc)[i];

        fX.push_back(digi->GetX());
        fY.push_back(digi->GetY());
        fN.push_back(digi->GetN());
    }
}

void RootIO::AddPrimary(G4PrimaryParticle* primary)
{
    const G4ParticleDefinition* pdef = primary->GetParticleDefinition();
    G4int pID = pdef->GetParticleDefinitionID();
    G4String pname = pdef->GetParticleName();
    G4double ke = primary->GetKineticEnergy();
    const G4ThreeVector pHat = primary->GetMomentumDirection();

    G4double theta = acos(abs(pHat.z()));
    G4double phi = atan2(pHat.y(), pHat.x());

    // fill hist
    G4int histID;
    if (fParticles.insert(pID).second)
        histID = fAnalysisManager
            ->CreateH1(pname, pname, 200, 0.1, 1000000, "MeV", "none", "log");
    else
        histID = fAnalysisManager
            ->GetH1Id(pname);
        
    fAnalysisManager->FillH1(histID, ke);

    // fill ntuple
    fAnalysisManager->FillNtupleSColumn(0, pname);
    fAnalysisManager->FillNtupleDColumn(1, ke / MeV);
    fAnalysisManager->FillNtupleDColumn(2, theta);
    fAnalysisManager->FillNtupleDColumn(3, phi);

}

void RootIO::WriteEvent()
{
    fAnalysisManager->AddNtupleRow();

    fX.clear();
    fY.clear();
    fN.clear();
}

void RootIO::Close()
{
    fAnalysisManager->Write();
    fAnalysisManager->CloseFile();
}
