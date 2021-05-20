#include "PixPhysMessenger.hh"

#include "PixModularPhysicsList.hh"


PixPhysMessenger::PixPhysMessenger(PixModularPhysicsList* phys)
{
    fPhys = phys;

    fCutDirectory = new G4UIdirectory("/pixcuts/");
    fCutDirectory->SetGuidance("Management of production cuts");

    cmdMaxStepSensor = new G4UIcmdWithADoubleAndUnit("/pixcuts/sensorStep", this);
    cmdMaxStepSensor->SetGuidance("Maximum step size inside sensor");
    cmdMaxStepSensor->AvailableForStates(G4State_PreInit);
    cmdMaxStepSensor->SetDefaultValue(PixModularPhysicsList::DEFAULT_MAX_STEP_SENSOR);
    cmdMaxStepSensor->SetDefaultUnit("um");
    cmdMaxStepSensor->SetParameterName("step", true);

    cmdRangeCutWorld = new G4UIcmdWithADoubleAndUnit("/pixcuts/worldCut", this);
    cmdRangeCutWorld->SetGuidance("Range cut in the surrounding air");
    cmdRangeCutWorld->AvailableForStates(G4State_PreInit);
    cmdRangeCutWorld->SetDefaultValue(PixModularPhysicsList::DEFAULT_RANGE_CUT_WORLD);
    cmdRangeCutWorld->SetDefaultUnit("um");
    cmdRangeCutWorld->SetParameterName("range", true);

    cmdRangeCutGlassShielding = new G4UIcmdWithADoubleAndUnit("/pixcuts/glassShieldingCut", this);
    cmdRangeCutGlassShielding->SetGuidance("Range cut for shielding outside sensor");
    cmdRangeCutGlassShielding->AvailableForStates(G4State_PreInit);
    cmdRangeCutGlassShielding->SetDefaultValue(PixModularPhysicsList::DEFAULT_RANGE_CUT_GLASS_SHIELDING);
    cmdRangeCutGlassShielding->SetDefaultUnit("um");
    cmdRangeCutGlassShielding->SetParameterName("range", true);

    cmdRangeCutSensor = new G4UIcmdWithADoubleAndUnit("/pixcuts/sensorCut", this);
    cmdRangeCutSensor->SetGuidance("Range cut for pixels");
    cmdRangeCutSensor->AvailableForStates(G4State_PreInit);
    cmdRangeCutSensor->SetDefaultValue(PixModularPhysicsList::DEFAULT_RANGE_CUT_SENSOR);
    cmdRangeCutSensor->SetDefaultUnit("um");
    cmdRangeCutSensor->SetParameterName("range", true);

    cmdMinProdThresh = new G4UIcmdWithADoubleAndUnit("/pixcuts/minSecondaryE", this);
    cmdMinProdThresh->SetGuidance("Minimum production threshold allowed by range cuts");
    cmdMinProdThresh->AvailableForStates(G4State_PreInit);
    cmdMinProdThresh->SetDefaultValue(PixModularPhysicsList::DEFAULT_MIN_PROD_THRESH);
    cmdMinProdThresh->SetDefaultUnit("eV");
    cmdMinProdThresh->SetParameterName("ESecondary", true);

}

PixPhysMessenger::~PixPhysMessenger()
{ 
    delete cmdMaxStepSensor;
    delete cmdRangeCutWorld;
    delete cmdRangeCutGlassShielding;
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
    else if (cmd == cmdRangeCutGlassShielding)
	fPhys->SetRangeCutGlassShielding(cmdRangeCutGlassShielding->GetNewDoubleValue(values));
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
    else if (cmd == cmdRangeCutGlassShielding)
	return cmd->ConvertToString(fPhys->GetRangeCutGlassShielding());
    else if (cmd == cmdRangeCutSensor)
        return cmd->ConvertToString(fPhys->GetRangeCutSensor());
    else if (cmd == cmdMinProdThresh)
        return cmd->ConvertToString(fPhys->GetMinProdThresh());
    
    return "";
}
