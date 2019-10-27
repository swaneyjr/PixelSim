#include "PixSensorMessenger.hh"
#include "PixDetectorConstruction.hh"

PixSensorMessenger::PixSensorMessenger(PixDetectorConstruction* dc)
    : fDC(dc)
{

    fGeometryDir = new G4UIdirectory("/sensor/geo/");
    fGeometryDir->SetGuidance("Pre-initialization sensor geometry");

    cmdResXY = new G4UIcmdWithAnInteger("/sensor/geo/res", this);
    cmdResXY->SetGuidance("Resolution of sensor (N x N)");
    cmdResXY->SetParameterName("resolution", true, true);
    cmdResXY->AvailableForStates(G4State_PreInit);

    cmdPixXY = new G4UIcmdWithADoubleAndUnit("/sensor/geo/pixSize", this);
    cmdPixXY->SetGuidance("x and y dimensions of pixels");
    cmdPixXY->SetParameterName("pixSize", true, true);
    cmdPixXY->AvailableForStates(G4State_PreInit);
    cmdPixXY->SetDefaultUnit("um");

    cmdPixZ = new G4UIcmdWithADoubleAndUnit("/sensor/geo/pixDepth", this);
    cmdPixZ->SetGuidance("Depth of silicon substrate");
    cmdPixZ->SetParameterName("pixDepth", true, true);
    cmdPixZ->AvailableForStates(G4State_PreInit);
    cmdPixZ->SetDefaultUnit("um");
 
    cmdGlassZ = new G4UIcmdWithADoubleAndUnit("/sensor/geo/glass", this);
    cmdGlassZ->SetGuidance("Thickness of lens");
    cmdGlassZ->SetParameterName("glass", true, true);
    cmdGlassZ->AvailableForStates(G4State_PreInit);
    cmdGlassZ->SetDefaultUnit("mm");

    
    fReadoutDir = new G4UIdirectory("/sensor/readout/");
    fReadoutDir->SetGuidance("Pixel readout parameters");
    
    cmdPixDepl = new G4UIcmdWithADoubleAndUnit("/sensor/readout/depletion", this);
    cmdPixDepl->SetGuidance("Length of depletion region");
    cmdPixDepl->SetParameterName("depletion", true, true);
    cmdPixDepl->AvailableForStates(G4State_PreInit);
    cmdPixDepl->SetDefaultUnit("um");

    cmdChargeSpread = new G4UIcmdWithADoubleAndUnit("/sensor/readout/chargeSpread", this);
    cmdChargeSpread->SetGuidance("Standard deviation of charge diffusion length");
    cmdChargeSpread->SetParameterName("chargeSpread", true, true);
    cmdChargeSpread->AvailableForStates(G4State_PreInit, G4State_Idle);
    cmdChargeSpread->SetDefaultUnit("um");

}

PixSensorMessenger::~PixSensorMessenger()
{
    delete fGeometryDir;
    
    delete cmdResXY;
    delete cmdPixXY;
    delete cmdPixZ; 
    delete cmdGlassZ;

    delete fReadoutDir;
    
    delete cmdPixDepl;
    delete cmdChargeSpread;

}

void PixSensorMessenger::SetNewValue(G4UIcommand* cmd, G4String values)
{
    if (cmd == cmdResXY)
        fDC->SetResXY(cmdResXY->GetNewIntValue(values));
    else if (cmd == cmdPixXY)
        fDC->SetPixXY(cmdPixXY->GetNewDoubleValue(values));
    else if (cmd == cmdPixZ)
        fDC->SetPixZ(cmdPixZ->GetNewDoubleValue(values));
    else if (cmd == cmdGlassZ)
        fDC->SetGlassZ(cmdGlassZ->GetNewDoubleValue(values));

    else if (cmd == cmdPixDepl)
        fDC->SetPixDepl(cmdPixDepl->GetNewDoubleValue(values));
    else if (cmd == cmdChargeSpread && fSD)
        fSD->SetChargeSpread(cmdChargeSpread->GetNewDoubleValue(values));
     
}

G4String PixSensorMessenger::GetCurrentValue(G4UIcommand* cmd)
{

    if (cmd == cmdResXY)
        return cmd->ConvertToString(fDC->GetResXY());
    else if (cmd == cmdPixXY)
        return cmd->ConvertToString(fDC->GetPixXY());
    else if (cmd == cmdPixZ)
        return cmd->ConvertToString(fDC->GetPixZ());
    else if (cmd == cmdPixDepl)
        return cmd->ConvertToString(fDC->GetPixDepl());
    else if (cmd == cmdGlassZ)
        return cmd->ConvertToString(fDC->GetGlassZ());
    else if (cmd == cmdChargeSpread)
    {
        if(fSD)
            return cmd->ConvertToString(fSD->GetChargeSpread());
        else
            return "-1";
    }
    
    else return "";
}

