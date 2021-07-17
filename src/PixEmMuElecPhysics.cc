#include "PixEmMuElecPhysics.hh"

#include "G4VPhysicsConstructor.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"

// *** Processes and models

// gamma
#include "G4PhotoElectricEffect.hh"
#include "G4LivermorePhotoElectricModel.hh"

#include "G4ComptonScattering.hh"
#include "G4LivermoreComptonModel.hh"

#include "G4GammaConversion.hh"
#include "G4LivermoreGammaConversionModel.hh"

#include "G4RayleighScattering.hh" 
#include "G4LivermoreRayleighModel.hh"

// e+-
#include "G4eMultipleScattering.hh"
#include "G4UniversalFluctuation.hh"

#include "G4eIonisation.hh"
#include "G4LivermoreIonisationModel.hh"

#include "G4eBremsstrahlung.hh"
#include "G4LivermoreBremsstrahlungModel.hh"
#include "G4Generator2BS.hh"

// e+
#include "G4eplusAnnihilation.hh"

// mu+-
#include "G4MuMultipleScattering.hh"
#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"

#include "G4MuBremsstrahlungModel.hh"
#include "G4MuPairProductionModel.hh"
#include "G4hBremsstrahlungModel.hh"
#include "G4hPairProductionModel.hh"

// hadrons
#include "G4hMultipleScattering.hh"
#include "G4MscStepLimitType.hh"

#include "G4hBremsstrahlung.hh"
#include "G4hPairProduction.hh"

#include "G4hIonisation.hh"
#include "G4ionIonisation.hh"
#include "G4alphaIonisation.hh"
#include "G4IonParametrisedLossModel.hh"
#include "G4NuclearStopping.hh"

// msc models
#include "G4UrbanMscModel.hh"
#include "G4WentzelVIModel.hh"
#include "G4GoudsmitSaundersonMscModel.hh"
#include "G4CoulombScattering.hh"

// interfaces
#include "G4LossTableManager.hh"
#include "G4EmProcessOptions.hh"
#include "G4UAtomicDeexcitation.hh"

// particles

#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4KaonPlus.hh"
#include "G4KaonMinus.hh"
#include "G4Proton.hh"
#include "G4AntiProton.hh"
#include "G4Deuteron.hh"
#include "G4Triton.hh"
#include "G4He3.hh"
#include "G4Alpha.hh"
#include "G4GenericIon.hh"

// microelectronics
#include "G4MicroElecElasticModel.hh"
#include "G4MicroElecInelasticModel.hh"

//
#include "G4PhysicsListHelper.hh"
#include "G4BuilderType.hh"
#include "G4Region.hh"
#include "G4RegionStore.hh"

// factory
#include "G4PhysicsConstructorFactory.hh"
//
G4_DECLARE_PHYSCONSTR_FACTORY(PixEmMuElecPhysics);

PixEmMuElecPhysics::PixEmMuElecPhysics(G4int ver)
  : G4VPhysicsConstructor("PixEmMuElecPhysics"), 
    verbose(ver)
{ 
    G4LossTableManager::Instance();
    SetPhysicsType(bElectromagnetic);
}

PixEmMuElecPhysics::PixEmMuElecPhysics(G4int ver, const G4String&)
  : G4VPhysicsConstructor("PixEmMuElecPhysics"), 
    verbose(ver)
{ 
    G4LossTableManager::Instance();
    SetPhysicsType(bElectromagnetic);
}

PixEmMuElecPhysics::~PixEmMuElecPhysics()
{ }

void PixEmMuElecPhysics::ConstructParticle()
{
    // gamma
    G4Gamma::Gamma();
  
    // leptons
    G4Electron::Electron();
    G4Positron::Positron();
    G4MuonPlus::MuonPlus();
    G4MuonMinus::MuonMinus();
  
    // mesons
    G4PionPlus::PionPlusDefinition();
    G4PionMinus::PionMinusDefinition();
    G4KaonPlus::KaonPlusDefinition();
    G4KaonMinus::KaonMinusDefinition();
  
    // baryons
    G4Proton::Proton();
    G4AntiProton::AntiProton();
   
    // ions
    G4Deuteron::Deuteron();
    G4Triton::Triton();
    G4He3::He3();
    G4Alpha::Alpha();
    G4GenericIon::GenericIonDefinition();
}

void PixEmMuElecPhysics::ConstructProcess()
{
    // first get the sensor region
    G4String regName = "sensorRegion";
    G4Region* sensorRegion = G4RegionStore::GetInstance()
        ->GetRegion(regName);
	
    G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();

    // muon & hadron bremsstrahlung and pair production
    G4MuBremsstrahlung* mub = new G4MuBremsstrahlung();
    G4MuPairProduction* mup = new G4MuPairProduction();
    G4hBremsstrahlung* pib = new G4hBremsstrahlung();
    G4hPairProduction* pip = new G4hPairProduction();
    G4hBremsstrahlung* kb = new G4hBremsstrahlung();
    G4hPairProduction* kp = new G4hPairProduction();
    G4hBremsstrahlung* pb = new G4hBremsstrahlung();
    G4hPairProduction* pp = new G4hPairProduction();
  
    // multiple scattering
    G4MuMultipleScattering* mumsc = new G4MuMultipleScattering();
    mumsc->AddEmModel(0, new G4WentzelVIModel());
    G4hMultipleScattering* hmsc = new G4hMultipleScattering();
    hmsc->AddEmModel(0, new G4UrbanMscModel(), sensorRegion);
    G4hMultipleScattering* pmsc = new G4hMultipleScattering();
    pmsc->AddEmModel(0, new G4UrbanMscModel(), sensorRegion);
    G4hMultipleScattering* pimsc = new G4hMultipleScattering();
    pimsc->AddEmModel(0, new G4UrbanMscModel(), sensorRegion);
    G4hMultipleScattering* kmsc = new G4hMultipleScattering();
    kmsc->AddEmModel(0, new G4UrbanMscModel(), sensorRegion);
  
    // Add Livermore EM Processes
    GetParticleIterator()->reset();
  
    while( (*GetParticleIterator())() ){
    
      G4ParticleDefinition* particle = GetParticleIterator()->value();
      G4String particleName = particle->GetParticleName();
      
      if(verbose > 1)
        G4cout << "### " << GetPhysicsName() << " instantiates for " 
           << particleName << G4endl;
  
      //Applicability range for Livermore models
      //for higher energies, the Standard models are used   
      G4double LivermoreHighEnergyLimit = GeV;
  
      if (particleName == "gamma") {
  
        G4PhotoElectricEffect* thePhotoElectricEffect = new G4PhotoElectricEffect();
        G4LivermorePhotoElectricModel* theLivermorePhotoElectricModel = 
          new G4LivermorePhotoElectricModel();
        theLivermorePhotoElectricModel->SetHighEnergyLimit(LivermoreHighEnergyLimit);
        thePhotoElectricEffect->AddEmModel(0, theLivermorePhotoElectricModel);
        ph->RegisterProcess(thePhotoElectricEffect, particle);
  
        G4ComptonScattering* theComptonScattering = new G4ComptonScattering();
        G4LivermoreComptonModel* theLivermoreComptonModel = 
          new G4LivermoreComptonModel();
        theLivermoreComptonModel->SetHighEnergyLimit(LivermoreHighEnergyLimit);
        theComptonScattering->AddEmModel(0, theLivermoreComptonModel);
        ph->RegisterProcess(theComptonScattering, particle);
  
        G4GammaConversion* theGammaConversion = new G4GammaConversion();
        G4LivermoreGammaConversionModel* theLivermoreGammaConversionModel = 
          new G4LivermoreGammaConversionModel();
        theLivermoreGammaConversionModel->SetHighEnergyLimit(LivermoreHighEnergyLimit);
        theGammaConversion->AddEmModel(0, theLivermoreGammaConversionModel);
        ph->RegisterProcess(theGammaConversion, particle);
  
        G4RayleighScattering* theRayleigh = new G4RayleighScattering();
        G4LivermoreRayleighModel* theRayleighModel = new G4LivermoreRayleighModel();
        theRayleighModel->SetHighEnergyLimit(LivermoreHighEnergyLimit);
        theRayleigh->AddEmModel(0, theRayleighModel);
        ph->RegisterProcess(theRayleigh, particle);
  
      } else if (particleName == "e-") {

	G4eMultipleScattering* emsc = new G4eMultipleScattering();
        G4MicroElecElasticModel* microElecElastic = new G4MicroElecElasticModel();
	microElecElastic->SetHighEnergyLimit(100*MeV);
	microElecElastic->SetLowEnergyLimit(16.7*eV);
	//emsc->AddEmModel(0, microElecElastic, sensorRegion);
        ph->RegisterProcess(emsc, particle);
        
        // Ionisation
	G4eIonisation* eIoni = new G4eIonisation();
        G4MicroElecInelasticModel* eIoniMicroElec = new G4MicroElecInelasticModel();
        eIoniMicroElec->SetLowEnergyLimit(16.7*eV);
	eIoniMicroElec->SetHighEnergyLimit(100*MeV);
	//G4LivermoreIonisationModel* theIoniLivermore = new G4LivermoreIonisationModel();
	//theIoniLivermore->SetLowEnergyLimit(16.7*eV);
	//theIoniLivermore->SetHighEnergyLimit(100*MeV);
	//eIoni->AddEmModel(0, theIoniLivermore, new G4UniversalFluctuation(), sensorRegion);
	eIoni->SetStepFunction(0.2, 100*um);	
        ph->RegisterProcess(eIoni, particle);
        
        // Bremsstrahlung
        G4eBremsstrahlung* eBrem = new G4eBremsstrahlung();
        G4LivermoreBremsstrahlungModel* theBremLivermore = new
        G4LivermoreBremsstrahlungModel();
        theBremLivermore->SetHighEnergyLimit(25*MeV);
        theBremLivermore->SetAngularDistribution(new G4Generator2BS());
        eBrem->AddEmModel(0, theBremLivermore);
        ph->RegisterProcess(eBrem, particle);

	// Electron capture
	// G4ElectronCapture* ecap = new G4ElectronCapture(regName, 16.7*eV);
	//ph->RegisterProcess(ecap, particle);
  
      } else if (particleName == "e+") {
  
        // Identical to G4EmStandardPhysics_option3
        
        //msc->AddEmModel(0, new G4GoudsmitSaundersonMscModel());
        G4eMultipleScattering* emsc = new G4eMultipleScattering();
        emsc->AddEmModel(0, new G4UrbanMscModel(), sensorRegion);

	G4eIonisation* eIoni = new G4eIonisation();
        eIoni->SetStepFunction(0.2, 100*um);      
  
        ph->RegisterProcess(emsc, particle);
        ph->RegisterProcess(eIoni, particle);
        ph->RegisterProcess(new G4eBremsstrahlung(), particle);
        ph->RegisterProcess(new G4eplusAnnihilation(), particle);
  
      } else if (particleName == "mu+" ||
                 particleName == "mu-"    ) {
  
        G4MuIonisation* muIoni = new G4MuIonisation();
        muIoni->SetStepFunction(0.2, 50*um);          
  
        ph->RegisterProcess(mumsc, particle);
        ph->RegisterProcess(muIoni, particle);
        ph->RegisterProcess(mub, particle);
        ph->RegisterProcess(mup, particle);
        ph->RegisterProcess(new G4CoulombScattering(), particle);
  
      } else if (particleName == "alpha" ||
                 particleName == "He3" ) {
  
        // Identical to G4EmStandardPhysics_option3
        
        G4ionIonisation* ionIoni = new G4ionIonisation();
        ionIoni->SetStepFunction(0.1, 10*um);
  
        ph->RegisterProcess(hmsc, particle);
        ph->RegisterProcess(ionIoni, particle);
        ph->RegisterProcess(new G4NuclearStopping(), particle);
  
      } else if (particleName == "GenericIon") {
  
        // Identical to G4EmStandardPhysics_option3
        
        G4ionIonisation* ionIoni = new G4ionIonisation();
        ionIoni->SetEmModel(new G4IonParametrisedLossModel());
	G4MicroElecInelasticModel* ionMicroElec = new G4MicroElecInelasticModel();
	ionMicroElec->SetLowEnergyLimit(50*keV);
	ionMicroElec->SetHighEnergyLimit(10*TeV);
	ionIoni->AddEmModel(0, ionMicroElec, new G4UniversalFluctuation(), sensorRegion);
        ionIoni->SetStepFunction(0.1, 1*um);
  
        ph->RegisterProcess(hmsc, particle);
        ph->RegisterProcess(ionIoni, particle);
        ph->RegisterProcess(new G4NuclearStopping(), particle);
  
      } else if (particleName == "pi+" ||
                 particleName == "pi-" ) {
  
        G4hIonisation* hIoni = new G4hIonisation();
        hIoni->SetStepFunction(0.2, 50*um);
  
        ph->RegisterProcess(pimsc, particle);
        ph->RegisterProcess(hIoni, particle);
        ph->RegisterProcess(pib, particle);
        ph->RegisterProcess(pip, particle);
  
      } else if (particleName == "kaon+" ||
                 particleName == "kaon-" ) {
  
        G4hIonisation* hIoni = new G4hIonisation();
        hIoni->SetStepFunction(0.2, 50*um);
  
        ph->RegisterProcess(kmsc, particle);
        ph->RegisterProcess(hIoni, particle);
        ph->RegisterProcess(kb, particle);
        ph->RegisterProcess(kp, particle);
  
      } else if (particleName == "proton") {
        G4hIonisation* hIoni = new G4hIonisation();
	G4MicroElecInelasticModel* pIoniMicroElec = new
        G4MicroElecInelasticModel();
        pIoniMicroElec->SetHighEnergyLimit(10*GeV); 
	pIoniMicroElec->SetLowEnergyLimit(50*keV);
        hIoni->AddEmModel(0, pIoniMicroElec, new G4UniversalFluctuation(), sensorRegion);

	hIoni->SetStepFunction(0.2, 50*um);

	ph->RegisterProcess(pmsc, particle);
	ph->RegisterProcess(hIoni, particle);
	ph->RegisterProcess(pb, particle);
	ph->RegisterProcess(pp, particle);

      } else if (particleName == "anti_proton") {
  
        G4hIonisation* hIoni = new G4hIonisation();
        hIoni->SetStepFunction(0.2, 50*um);
  
        ph->RegisterProcess(pmsc, particle);
        ph->RegisterProcess(hIoni, particle);
        ph->RegisterProcess(pb, particle);
        ph->RegisterProcess(pp, particle);
  
      } else if (particleName == "B+" ||
                 particleName == "B-" ||
                 particleName == "D+" ||
                 particleName == "D-" ||
                 particleName == "Ds+" ||
                 particleName == "Ds-" ||
                 particleName == "anti_He3" ||
                 particleName == "anti_alpha" ||
                 particleName == "anti_deuteron" ||
                 particleName == "anti_lambda_c+" ||
                 particleName == "anti_omega-" ||
                 particleName == "anti_sigma_c+" ||
                 particleName == "anti_sigma_c++" ||
                 particleName == "anti_sigma+" ||
                 particleName == "anti_sigma-" ||
                 particleName == "anti_triton" ||
                 particleName == "anti_xi_c+" ||
                 particleName == "anti_xi-" ||
                 particleName == "deuteron" ||
                 particleName == "lambda_c+" ||
                 particleName == "omega-" ||
                 particleName == "sigma_c+" ||
                 particleName == "sigma_c++" ||
                 particleName == "sigma+" ||
                 particleName == "sigma-" ||
                 particleName == "tau+" ||
                 particleName == "tau-" ||
                 particleName == "triton" ||
                 particleName == "xi_c+" ||
                 particleName == "xi-" ) {
  
        // Identical to G4EmStandardPhysics_option3
        
        ph->RegisterProcess(hmsc, particle);
        ph->RegisterProcess(new G4hIonisation(), particle);
  
      }
    }
      
    // Em options
    //      
    G4EmProcessOptions opt;
    opt.SetVerbose(verbose);
    
    // Multiple Coulomb scattering
    //
    opt.SetPolarAngleLimit(CLHEP::pi);
      
    // Physics tables
    //
  
    opt.SetMinEnergy(16.7*eV);
    opt.SetMaxEnergy(10*TeV);
    opt.SetDEDXBinning(220);
    opt.SetLambdaBinning(220);
  
    // Ionization
    //
    //opt.SetSubCutoff(true);    
  
    // Deexcitation
    //
    G4VAtomDeexcitation* de = new G4UAtomicDeexcitation();
    G4LossTableManager::Instance()->SetAtomDeexcitation(de);
    de->SetFluo(true);

}
