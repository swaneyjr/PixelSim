#include "PixAnalysisMessenger.hh"

#include "RootIO.hh"


PixAnalysisMessenger::PixAnalysisMessenger(RootIO* root)
{
    fROOT = root;

    fROOTDirectory = new G4UIdirectory("/root/");
    fROOTDirectory->SetGuidance("ROOT IO options");

    cmdMinPix = new G4UIcmdWithAnInteger("/root/minPix", this);
    cmdMinPix->SetGuidance("Maximum step size inside sensor");
    cmdMinPix->AvailableForStates(G4State_PreInit);

}

PixAnalysisMessenger::~PixAnalysisMessenger()
{ 
    delete cmdMinPix;

    delete fROOTDirectory;
}

void PixAnalysisMessenger::SetNewValue(G4UIcommand* cmd, G4String values)
{ 
    if (cmd == cmdMinPix)
        fROOT->SetMinPix(cmdMinPix->GetNewIntValue(values));

}

G4String PixAnalysisMessenger::GetCurrentValue(G4UIcommand* cmd)
{
    if (cmd == cmdMinPix)
        return cmd->ConvertToString(fROOT->GetMinPix());
    
    return "";
}
