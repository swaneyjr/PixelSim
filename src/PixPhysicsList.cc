#include "PixPhysicsList.hh"
#include "PixPhysMessenger.hh"

#include "G4HadronPhysicsFTFP_BERT.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4IonPhysics.hh"
#include "G4StoppingPhysics.hh"
#include "G4NeutronTrackingCut.hh"
#include "G4EmExtraPhysics.hh"
#include "G4EmLowEPPhysics.hh"
#include "G4StepLimiterPhysics.hh"
#include "G4Region.hh"
#include "G4RegionStore.hh"
#include "G4ProductionCuts.hh"
#include "G4ProductionCutsTable.hh"

PixPhysicsList::PixPhysicsList()
    :   G4VModularPhysicsList(),
        fMaxStepSensor(DEFAULT_MAX_STEP_SENSOR),
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
    RegisterPhysics(new G4EmLowEPPhysics(verbose));
    
    RegisterPhysics(new G4StepLimiterPhysics());
}

PixPhysicsList::~PixPhysicsList()
{ 
    delete fStepLimit;
    delete fPhysMessenger;
}

void PixPhysicsList::SetCuts()
{ 
    // world default
    SetCutsWithDefault();
    
    // first get the sensor region
    G4String regName = "sensorRegion";
    G4Region* sensorRegion = G4RegionStore::GetInstance()
        ->GetRegion(regName);
    
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

    // min production threshold
    G4ProductionCutsTable::GetProductionCutsTable()
        ->SetEnergyRange(fMinProdThresh, 10*GeV);
}
