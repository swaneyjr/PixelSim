#include "PixParticleMessenger.hh"
#include "PixDetectorConstruction.hh"
#include "PixPrimaryGenerator.hh"
#include "PixAngDistribution.hh"

#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4RunManager.hh"
#include "G4ios.hh"

PixParticleMessenger::PixParticleMessenger(PixPrimaryGenerator* pg)
    :   G4UImessenger(),
        fParticleGun(pg)
{ 
    fParticleDir = new G4UIdirectory("/source/");
    
    cmdParticle = new G4UIcmdWithAString("/source/particle", this);
    cmdParticle->AvailableForStates(G4State_PreInit, G4State_Idle);
    cmdParticle->SetParameterName("name", true, true);
    cmdParticle->SetGuidance("Particle species to simulate");
    G4String candidateList;
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleTable::G4PTblDicIterator* itr = particleTable->GetIterator();
    itr->reset();
    while( (*itr)() )
    {
        G4ParticleDefinition* pd = itr->value();
        if( !(pd->IsShortLived()) || pd->GetDecayTable() )
        {
            candidateList += pd->GetParticleName();
            candidateList += " ";
        }
    }
    cmdParticle->SetCandidates(candidateList);
    cmdParticle->SetDefaultValue("geantino");

    // angle
    fAngDir = new G4UIdirectory("/source/ang/");

    cmdAngIso = new G4UIcmdWithoutParameter("/source/ang/iso", this);
    cmdAngIso->AvailableForStates(G4State_PreInit, G4State_Idle);
    cmdAngIso->SetGuidance("Isotropic source in one hemisphere");

    cmdAngGauss = new G4UIcmdWithADoubleAndUnit("/source/ang/gauss", this);
    cmdAngGauss->AvailableForStates(G4State_PreInit, G4State_Idle);
    cmdAngGauss->SetParameterName("sigma", true, true);
    cmdAngGauss->SetDefaultUnit("deg");
    cmdAngGauss->SetGuidance("Gaussian angular spread with selected sigma");
    
    cmdAngCosmic = new G4UIcmdWithoutParameter("/source/ang/cosmic", this);
    cmdAngCosmic->AvailableForStates(G4State_PreInit, G4State_Idle);
    cmdAngCosmic->SetGuidance("Cosine squared angular distribution");

    cmdMaxTheta = new G4UIcmdWithADoubleAndUnit("/source/ang/maxTheta", this);
    cmdMaxTheta->SetGuidance("Maximum angle of incidence allowed (determines world size)");
    cmdMaxTheta->SetParameterName("theta", true, true);
    cmdMaxTheta->AvailableForStates(G4State_PreInit);
    cmdMaxTheta->SetDefaultUnit("deg");

    // energy
    fEneDir = new G4UIdirectory("/source/ene/");
    
    cmdEneMean = new G4UIcmdWithADoubleAndUnit("/source/ene/mean", this);
    cmdEneMean->AvailableForStates(G4State_PreInit, G4State_Idle);
    cmdEneMean->SetParameterName("mu_E", true, true);
    cmdEneMean->SetDefaultUnit("MeV");
    cmdEneMean->SetGuidance("Mean particle energy");

    cmdEneSigma = new G4UIcmdWithADoubleAndUnit("/source/ene/sigma", this);
    cmdEneSigma->AvailableForStates(G4State_PreInit, G4State_Idle);
    cmdEneSigma->SetParameterName("sigma_E", true, true);
    cmdEneSigma->SetDefaultUnit("MeV");
    cmdEneSigma->SetGuidance("Spread in particle energy (default 0)");

    cmdEneHist = new G4UIcmdWithAString("/source/ene/hist", this);
    cmdEneHist->AvailableForStates(G4State_PreInit, G4State_Idle);
    cmdEneHist->SetParameterName("file", false, false);
    cmdEneHist->SetGuidance("File with energy bins");
}

PixParticleMessenger::~PixParticleMessenger()
{
    delete cmdParticle;
    delete cmdAngIso;
    delete cmdAngGauss;
    delete cmdAngCosmic;
    delete cmdEneMean;
    delete cmdEneSigma;
    delete cmdEneHist;
    delete cmdMaxTheta;

    delete fAngDir;
    delete fEneDir;
    delete fParticleDir;
}

void PixParticleMessenger::SetNewValue(G4UIcommand* cmd, G4String values)
{ 
    
    if (cmd == cmdParticle)
    {
        G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
        G4ParticleDefinition* pd = particleTable->FindParticle(values);
        if(pd != 0) {
            fParticleGun->SetParticleDefinition( pd );
        } else {
            G4ExceptionDescription ed;
            ed << "Particle [" << values << "] is not found.";
            cmd->CommandFailed(ed);
        }
    }

    else if (cmd == cmdAngIso)
    {
        fParticleGun->GetAngDist()
            ->SetAngDistType("iso");
    }
    else if (cmd == cmdAngGauss)
    {
        fParticleGun->GetAngDist()
            ->SetAngDistType("beam1d");
        fParticleGun->GetAngDist()
            ->SetBeamSigmaInAngR(cmdAngGauss->GetNewDoubleValue(values));
    }
    else if (cmd == cmdAngCosmic)
        fParticleGun->GetAngDist()
            ->SetAngDistType("cosmic");
    else if (cmd == cmdMaxTheta) 
    {
        G4RunManager* runMan = G4RunManager::GetRunManager();
        PixDetectorConstruction* dc = (PixDetectorConstruction*)
            runMan->GetUserDetectorConstruction();
        dc->SetMaxTheta(cmdMaxTheta->GetNewDoubleValue(values));
    }
    else if (cmd == cmdEneMean)
    {
        fParticleGun->GetEneDist()
            ->SetMonoEnergy(cmdEneMean->GetNewDoubleValue(values));
        fParticleGun->GetEneDist()
            ->SetEnergyDisType("Gauss");
    }
    else if (cmd == cmdEneSigma)
    {
        fParticleGun->GetEneDist()
            ->SetBeamSigmaInE(cmdEneMean->GetNewDoubleValue(values));
        fParticleGun->GetEneDist()
            ->SetEnergyDisType("Gauss");
    }
    else if (cmd == cmdEneHist)
        fParticleGun->GetEneDist()
            ->ArbEnergyHistoFile(values);

}

G4String PixParticleMessenger::GetCurrentValue(G4UIcommand* cmd)
{ 
    if (cmd == cmdParticle)
        return fParticleGun->GetParticleDefinition()->GetParticleName();
    else if (cmd == cmdAngIso)
        return (fParticleGun->GetAngDist()->GetDistType() == "iso") ? "true" : "false";
    else if (cmd == cmdAngGauss)
        return (fParticleGun->GetAngDist()->GetDistType() == "gauss") ? "true" : "false";
    else if (cmd == cmdAngCosmic)
        return (fParticleGun->GetAngDist()->GetDistType() == "cosmic") ? "true" : "false";
    else if (cmd == cmdMaxTheta) 
    {
        G4RunManager* runMan = G4RunManager::GetRunManager();
        PixDetectorConstruction* dc = (PixDetectorConstruction*)
            runMan->GetUserDetectorConstruction();
        return cmd->ConvertToString(dc->GetMaxTheta());
    }

    else if (cmd == cmdEneMean)
        return cmd->ConvertToString(fParticleGun->GetEneDist()->GetMonoEnergy());
    else if (cmd == cmdEneSigma)
        return cmd->ConvertToString(fParticleGun->GetEneDist()->GetSE());
    else if (cmd == cmdEneHist)
        return (fParticleGun->GetEneDist()->GetEnergyDisType() == "Arb") ? "true" : "false";

    else return "";
}
