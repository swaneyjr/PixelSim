#include "PixSpectrumMessenger.hh"
#include "PixDetectorConstruction.hh"
#include "PixPrimaryGeneratorAction.hh"
#include "PixPrimaryGenerator.hh"

#include "G4RunManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

#include <glob.h>

PixSpectrumMessenger::PixSpectrumMessenger()
    :   fSpectrumPath("spectra/0"),
        fResTot(1)
{
    fSpectrumDir = new G4UIdirectory("/spectrum/");
    fSpectrumDir->SetGuidance("Tools for executing a spectrum of particles");

    cmdSpectrumPath = new G4UIcmdWithAString("/spectrum/path", this);
    cmdSpectrumPath->SetGuidance("Location of the spectrum folder");
    cmdSpectrumPath->AvailableForStates(G4State_PreInit, G4State_Idle);
    cmdSpectrumPath->SetParameterName("path", true, true);

    cmdResTot = new G4UIcmdWithAnInteger("/spectrum/npix", this);
    cmdResTot->SetGuidance("For using /spectrum/runT, the area over which to calculate total flux, inunits of pixels.");
    cmdResTot->SetGuidance("default: 1");
    cmdResTot->AvailableForStates(G4State_PreInit, G4State_Idle);
    cmdResTot->SetParameterName("npix", true);

    cmdRunN = new G4UIcmdWithAnInteger("/spectrum/runN", this);
    cmdRunN->SetGuidance("Run with spectrum fluxes for a total of N hits");
    cmdRunN->AvailableForStates(G4State_PreInit, G4State_Idle);
    cmdRunN->SetParameterName("N", true);

    cmdRunT = new G4UIcmdWithADoubleAndUnit("/spectrum/runT", this);
    cmdRunT->SetGuidance("Generate particles from spectrum fluxes over a specified time (in s).");
    cmdRunT->SetGuidance("To adjust effective area, use /spectrum/npix");
    cmdRunT->AvailableForStates(G4State_PreInit, G4State_Idle);
    cmdRunT->SetParameterName("t", true);
    cmdRunT->SetUnitCandidates("s");

}

PixSpectrumMessenger::~PixSpectrumMessenger()
{
    delete cmdSpectrumPath;
    delete cmdResTot;
    delete cmdRunN;
    delete cmdRunT;

    delete fSpectrumDir;
}

void PixSpectrumMessenger::SetNewValue(G4UIcommand* cmd, G4String values)
{
    if (cmd == cmdSpectrumPath)
        fSpectrumPath = values;
    else if (cmd == cmdResTot)
        fResTot = cmdResTot->GetNewIntValue(values);
    else if (cmd == cmdRunN)
        RunSpectrum(cmdRunN->GetNewIntValue(values), false);
    else if (cmd == cmdRunT)
        RunSpectrum(cmdRunT->GetNewDoubleValue(values), true);
}

G4String PixSpectrumMessenger::GetCurrentValue(G4UIcommand* cmd)
{
    if (cmd == cmdSpectrumPath)
        return fSpectrumPath;
    else if (cmd == cmdResTot)
        return cmdResTot->ConvertToString(fResTot);
    
    else return "";
}


void PixSpectrumMessenger::RunSpectrum(G4double val, G4bool tVal)
{ 
 
    // get all files in the directory
    G4String pathname = fSpectrumPath + "/*.dat";
    const char* pathc = pathname.c_str();
    glob_t* pglob = new glob_t();
    glob(pathc, 0, nullptr, pglob);

    // now loop through files to get total flux
    const G4int nParticles = pglob->gl_pathc;
    G4String* particles = new G4String[nParticles]();
    G4double* fluxes = new G4double[nParticles]();
    G4double totalFlux = 0;

    for(G4int p=0; p < nParticles; p++) {
        G4String ppath = pglob->gl_pathv[p];

        // first find relative particle abundances
        std::ifstream inFile(ppath);
        G4double lastEnergy = 0;
        G4double lastFlux = 0;
        G4String data_pt;
        while (inFile) {
            std::getline(inFile, data_pt);
            if (data_pt != "") {	
                G4int i = data_pt.index("\t");
                if( i < 0 ) {
                    i = data_pt.index(" ");
                } 
                G4double energy = std::stod(data_pt(0,i)) * MeV;
                G4double flux = std::stod(data_pt(i+1, data_pt.length()-(i+1)))
                    / (MeV * s * cm2);
      
                *(fluxes+p) += (flux+lastFlux)/2 * (energy-lastEnergy);
      
                lastEnergy = energy;
                lastFlux = flux;
            }
        }
        inFile.close();

        // now parse to find particle name
        // get rid of .dat
        ppath.erase(ppath.size()-4, 4);
        // and leading directory
        ppath.erase(0, pathname.size()-5);
        *(particles + p) = ppath;
        totalFlux += *(fluxes + p);
    }

    globfree(pglob);
 
    G4RunManager* runManager = G4RunManager::GetRunManager();
    PixPrimaryGeneratorAction* pga = (PixPrimaryGeneratorAction*)
        runManager->GetUserPrimaryGeneratorAction();
    PixPrimaryGenerator* ppg = pga->GetPrimaryGenerator();
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

    for (G4int i=0; i<nParticles; i++)
    {
        
        // configure particle source
        G4ParticleDefinition* pd = particleTable->FindParticle(*(particles+i));
        ppg->SetParticleDefinition(pd);

        G4String histName = fSpectrumPath + "/" + particles[i] + ".dat";
        ppg->GetEneDist()
            ->ArbEnergyHistoFile(histName);

        G4int np;
        if (tVal)
        {
            PixDetectorConstruction* dc = (PixDetectorConstruction*)
                runManager->GetUserDetectorConstruction();
            G4double pixXY = dc->GetPixXY();
            G4double area = pixXY * pixXY * fResTot;
            np = (G4int)(val * (*(fluxes+i)) * area);
        }
        else
        {
            np = (G4int)(*(fluxes+i) * val / totalFlux);
        }

        std::cout << *(particles+i) << ": " << np << "\n";

        if ( np ) {
            runManager->SetVerboseLevel(6);
            runManager->BeamOn(np); 
        }
    }

    delete [] particles;
    delete [] fluxes;

}

