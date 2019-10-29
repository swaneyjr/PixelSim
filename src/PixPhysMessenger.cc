#include "PixPhysMessenger.hh"

#include "PixPhysicsList.hh"


PixPhysMessenger::PixPhysMessenger(PixPhysicsList* phys)
{
    fPhys = phys;

    fCutDirectory = new G4UIdirectory("/pixcuts/");
    fCutDirectory->SetGuidance("Management of production cuts");

    cmdMaxStepSensor = new G4UIcmdWithADoubleAndUnit("/pixcuts/sensorStep", this);
    cmdMaxStepSensor->SetGuidance("Maximum step size inside sensor");
    cmdMaxStepSensor->AvailableForStates(G4State_PreInit);
    cmdMaxStepSensor->SetDefaultValue(PixPhysicsList::DEFAULT_MAX_STEP_SENSOR);
    cmdMaxStepSensor->SetDefaultUnit("um");
    cmdMaxStepSensor->SetParameterName("step", true);

    cmdRangeCutWorld = new G4UIcmdWithADoubleAndUnit("/pixcuts/worldCut", this);
    cmdRangeCutWorld->SetGuidance("Range cut outside of sensor");
    cmdRangeCutWorld->AvailableForStates(G4State_PreInit);
    cmdRangeCutWorld->SetDefaultValue(PixPhysicsList::DEFAULT_RANGE_CUT_WORLD);
    cmdRangeCutWorld->SetDefaultUnit("um");
    cmdRangeCutWorld->SetParameterName("range", true);

    cmdRangeCutSensor = new G4UIcmdWithADoubleAndUnit("/pixcuts/sensorCut", this);
    cmdRangeCutSensor->SetGuidance("Range cut for pixels");
    cmdRangeCutSensor->AvailableForStates(G4State_PreInit);
    cmdRangeCutSensor->SetDefaultValue(PixPhysicsList::DEFAULT_RANGE_CUT_SENSOR);
    cmdRangeCutSensor->SetDefaultUnit("um");
    cmdRangeCutSensor->SetParameterName("range", true);

    cmdMinProdThresh = new G4UIcmdWithADoubleAndUnit("/pixcuts/minSecondaryE", this);
    cmdMinProdThresh->SetGuidance("Minimum production threshold allowed by range cuts");
    cmdMinProdThresh->AvailableForStates(G4State_PreInit);
    cmdMinProdThresh->SetDefaultValue(PixPhysicsList::DEFAULT_MIN_PROD_THRESH);
    cmdMinProdThresh->SetDefaultUnit("eV");
    cmdMinProdThresh->SetParameterName("ESecondary", true);

}

PixPhysMessenger::~PixPhysMessenger()
{ 
    delete cmdMaxStepSensor;
    delete cmdRangeCutWorld;
    delete cmdRangeCutSensor;
    delete cmdMinProdThresh;

    delete fCutDirectory;
}

void PixPhysMessenger::SetNewValue(G4UIcommand* cmd, G4String values)
{ 
    if (cmd == cmdMaxStepSensor)
        fPhys->SetMaxStepSensor(cmdMaxStepSensor->GetNewDoubleValue(values));
    else if (cmd == cmdRangeCutWorld)
        fPhys->SetRangeCutWorld(cmdRangeCutWorld->GetNewDoubleValue(values));
    else if (cmd == cmdRangeCutSensor)
        fPhys->SetRangeCutSensor(cmdRangeCutSensor->GetNewDoubleValue(values));
    else if (cmd == cmdMinProdThresh)
        fPhys->SetMinProdThresh(cmdMinProdThresh->GetNewDoubleValue(values));

}

G4String PixPhysMessenger::GetCurrentValue(G4UIcommand* cmd)
{
    if (cmd == cmdMaxStepSensor)
        return cmd->ConvertToString(fPhys->GetMaxStepSensor());
    else if (cmd == cmdRangeCutWorld)
        return cmd->ConvertToString(fPhys->GetRangeCutWorld());
    else if (cmd == cmdRangeCutSensor)
        return cmd->ConvertToString(fPhys->GetRangeCutSensor());
    else if (cmd == cmdMinProdThresh)
        return cmd->ConvertToString(fPhys->GetMinProdThresh());
    
    return "";
}
