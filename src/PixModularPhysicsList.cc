#include "PixModularPhysicsList.hh"
#include "PixPhysMessenger.hh"
#include "PixEmMuElecPhysics.hh"

#include "G4HadronPhysicsFTFP_BERT.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4IonPhysics.hh"
#include "G4StoppingPhysics.hh"
#include "G4NeutronTrackingCut.hh"
#include "G4EmStandardPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4EmLowEPPhysics.hh"
#include "G4StepLimiterPhysics.hh"
#include "G4Region.hh"
#include "G4RegionStore.hh"
#include "G4ProductionCuts.hh"
#include "G4ProductionCutsTable.hh"

PixModularPhysicsList::PixModularPhysicsList()
    :   G4VModularPhysicsList(),
        fMaxStepSensor(DEFAULT_MAX_STEP_SENSOR),
        fRangeCutGlassShielding(DEFAULT_RANGE_CUT_GLASS_SHIELDING),
	fRangeCutSensor(DEFAULT_RANGE_CUT_SENSOR),
        fMinProdThresh(DEFAULT_MIN_PROD_THRESH),
        fStepLimit(0),
        fPhysMessenger(new PixPhysMessenger(this))
{

    defaultCutValue = DEFAULT_RANGE_CUT_WORLD;

    G4int verbose = 1;

    // the standard FTFP_BERT physics
    RegisterPhysics(new G4HadronPhysicsFTFP_BERT(verbose));
    RegisterPhysics(new G4HadronElasticPhysics(verbose));
    RegisterPhysics(new G4DecayPhysics(verbose));
    RegisterPhysics(new G4IonPhysics(verbose));
    RegisterPhysics(new G4StoppingPhysics(verbose));
    RegisterPhysics(new G4NeutronTrackingCut(verbose));
    RegisterPhysics(new G4EmExtraPhysics(verbose));
    // use low energy EM physics option
    RegisterPhysics(new G4EmStandardPhysics_option4(verbose));
    //RegisterPhysics(new G4EmLivermorePhysics(verbose));
    //RegisterPhysics(new G4EmPenelopePhysics(verbose));
    //RegisterPhysics(new G4EmLowEPPhysics(verbose));
    //RegisterPhysics(new PixEmMuElecPhysics(verbose));


    RegisterPhysics(new G4StepLimiterPhysics());
}

PixModularPhysicsList::~PixModularPhysicsList()
{ 
    delete fStepLimit;
    delete fPhysMessenger;
}

void PixModularPhysicsList::SetCuts()
{ 
    // world default
    SetCutsWithDefault();
    
    // first get the regions for low energy production cuts
    G4RegionStore* regStore = G4RegionStore::GetInstance();
    G4Region* sensorRegion = regStore->GetRegion("sensorRegion");
    G4Region* glassShieldingRegion = regStore->GetRegion("glassShieldingRegion");

    // max step size
    if (fMaxStepSensor > 0)
    {
        fStepLimit = new G4UserLimits(fMaxStepSensor);
        sensorRegion->SetUserLimits(fStepLimit);
    }

    // range cuts
    G4ProductionCuts* sensorCuts = new G4ProductionCuts;
    sensorCuts->SetProductionCut(fRangeCutSensor);
    sensorRegion->SetProductionCuts(sensorCuts);

    G4ProductionCuts* glassShieldingCuts = new G4ProductionCuts;
    glassShieldingCuts->SetProductionCut(fRangeCutGlassShielding);
    glassShieldingRegion->SetProductionCuts(glassShieldingCuts);

    // min production threshold
    G4ProductionCutsTable::GetProductionCutsTable()
        ->SetEnergyRange(fMinProdThresh, 10*GeV);
}
