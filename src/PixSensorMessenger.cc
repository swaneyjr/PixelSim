#include "PixSensorMessenger.hh"
#include "PixDetectorConstruction.hh"

PixSensorMessenger::PixSensorMessenger(PixDetectorConstruction* dc)
    : fDC(dc)
{

    fGeometryDir = new G4UIdirectory("/sensor/geo/");
    fGeometryDir->SetGuidance("Pre-initialization sensor geometry");

    cmdResXY = new G4UIcmdWithAnInteger("/sensor/geo/res", this);
    cmdResXY->SetGuidance("Resolution of sensor (N x N)");
    cmdResXY->SetParameterName("resolution", true);
    cmdResXY->SetDefaultValue(PixDetectorConstruction::DEFAULT_RES_XY);
    cmdResXY->AvailableForStates(G4State_PreInit);

    cmdPixXY = new G4UIcmdWithADoubleAndUnit("/sensor/geo/pixSize", this);
    cmdPixXY->SetGuidance("x and y dimensions of pixels");
    cmdPixXY->SetParameterName("pixSize", true);
    cmdPixXY->AvailableForStates(G4State_PreInit);
    cmdPixXY->SetDefaultValue(PixDetectorConstruction::DEFAULT_PIX_XY);
    cmdPixXY->SetDefaultUnit("um");

    cmdPixZ = new G4UIcmdWithADoubleAndUnit("/sensor/geo/pixDepth", this);
    cmdPixZ->SetGuidance("Depth of silicon substrate");
    cmdPixZ->SetParameterName("pixDepth", true);
    cmdPixZ->AvailableForStates(G4State_PreInit);
    cmdPixZ->SetDefaultValue(PixDetectorConstruction::DEFAULT_PIX_Z);
    cmdPixZ->SetDefaultUnit("um");
 
    cmdGlassZ = new G4UIcmdWithADoubleAndUnit("/sensor/geo/glass", this);
    cmdGlassZ->SetGuidance("Thickness of lens");
    cmdGlassZ->SetParameterName("glass", true);
    cmdGlassZ->AvailableForStates(G4State_PreInit);
    cmdGlassZ->SetDefaultValue(PixDetectorConstruction::DEFAULT_GLASS_Z);
    cmdGlassZ->SetDefaultUnit("mm");

    
    fReadoutDir = new G4UIdirectory("/sensor/readout/");
    fReadoutDir->SetGuidance("Pixel readout parameters");

    cmdPixDepl = new G4UIcmdWithADoubleAndUnit("/sensor/readout/depletion", this);
    cmdPixDepl->SetGuidance("Length of depletion region");
    cmdPixDepl->SetParameterName("depletion", true);
    cmdPixDepl->AvailableForStates(G4State_PreInit);
    cmdPixDepl->SetDefaultValue(PixDetectorConstruction::DEFAULT_PIX_DEPL);
    cmdPixDepl->SetDefaultUnit("um");

    cmdDiffModel = new G4UIcmdWithAString("/sensor/readout/diffusionModel", this);
    cmdDiffModel->SetGuidance("Model for distribution of electrons created in the substrate.");
    cmdDiffModel->SetCandidates("MC Isolation");
    cmdDiffModel->SetDefaultValue(PixDetectorConstruction::DEFAULT_DIFFUSION_MODEL);

    cmdDiffLen = new G4UIcmdWithADoubleAndUnit("/sensor/readout/diffusion", this);
    cmdDiffLen->SetGuidance("Electron diffusion length");
    cmdDiffLen->SetParameterName("L_diff", true);
    cmdDiffLen->AvailableForStates(G4State_PreInit);
    cmdDiffLen->SetDefaultValue(PixDetectorConstruction::DEFAULT_DIFFUSION_LENGTH);
    cmdDiffLen->SetDefaultUnit("um");

    cmdDTI = new G4UIcmdWithADoubleAndUnit("/sensor/readout/dti", this);
    cmdDTI->SetGuidance("Depth of Deep Trench Isolation (DTI)");
    cmdDTI->SetParameterName("depth", true);
    cmdDTI->AvailableForStates(G4State_PreInit);
    cmdDTI->SetDefaultValue(PixDetectorConstruction::DEFAULT_DTI_DEPTH);
    cmdDTI->SetDefaultUnit("um");

    cmdDiffStep = new G4UIcmdWithADoubleAndUnit("/sensor/readout/diffStep", this);
    cmdDiffStep->SetGuidance("Size of steps in Monte Carlo diffusion of charge");
    cmdDiffStep->SetParameterName("stepSize", true);
    cmdDiffStep->AvailableForStates(G4State_PreInit);
    cmdDiffStep->SetDefaultValue(PixDetectorConstruction::DEFAULT_DIFF_STEP);
    cmdDiffStep->SetDefaultUnit("nm");


    fFastMCDir = new G4UIdirectory("/sensor/fastMC/");
    fFastMCDir->SetGuidance("Parameters for pre-calculating the distribution of diffused charge.");
    
    cmdInterpolation = new G4UIcmdWithAString("/sensor/fastMC/interpolation", this);
    cmdInterpolation->SetGuidance("Interpolation for 3D grid of diffusion distributions.");
    cmdInterpolation->SetGuidance("If 'Off', no grid is calculated, and each electron is diffused in the simulation");
    cmdInterpolation->SetParameterName("method", true);
    cmdInterpolation->AvailableForStates(G4State_PreInit);
    cmdInterpolation->SetCandidates("Nearest Linear Off");
    cmdInterpolation->SetDefaultValue(PixDetectorConstruction::DEFAULT_FAST_MC_INTERPOLATION);
    
    cmdGridSpacing = new G4UIcmdWithADoubleAndUnit("/sensor/fastMC/gridSpacing", this);
    cmdGridSpacing->SetGuidance("Maximum distance between grid points");
    cmdGridSpacing->SetParameterName("dxyz", true);
    cmdGridSpacing->AvailableForStates(G4State_PreInit);
    cmdGridSpacing->SetDefaultValue(PixDetectorConstruction::DEFAULT_FAST_MC_GRID_SPACING);
    cmdGridSpacing->SetDefaultUnit("nm");

    cmdMaxSpread = new G4UIcmdWithAnInteger("/sensor/fastMC/maxSpread", this);
    cmdMaxSpread->SetGuidance("Maximum distance in pixels for which the diffusion distribution is tracked");
    cmdMaxSpread->SetParameterName("dpix", true);
    cmdMaxSpread->AvailableForStates(G4State_PreInit);
    cmdMaxSpread->SetDefaultValue(PixDetectorConstruction::DEFAULT_FAST_MC_MAX_SPREAD);

    cmdSampleSize = new G4UIcmdWithAnInteger("/sensor/fastMC/sampleSize", this);
    cmdSampleSize->SetGuidance("Number of electrons to simulate at each grid point");
    cmdSampleSize->SetParameterName("N", true);
    cmdSampleSize->AvailableForStates(G4State_PreInit);
    cmdSampleSize->SetDefaultValue(PixDetectorConstruction::DEFAULT_FAST_MC_SAMPLE_SIZE);

}

PixSensorMessenger::~PixSensorMessenger()
{    
    delete cmdResXY;
    delete cmdPixXY;
    delete cmdPixZ; 
    delete cmdGlassZ;

    delete fGeometryDir; 
    
    delete cmdPixDepl;
    delete cmdDiffModel;
    delete cmdDiffLen;
    delete cmdDTI;
    delete cmdDiffStep;

    delete fReadoutDir;

    delete cmdInterpolation;
    delete cmdGridSpacing;
    delete cmdMaxSpread;
    delete cmdSampleSize;
    

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
    else if (cmd == cmdDiffModel)
        fDC->SetDiffusionModel(values);
    else if (cmd == cmdDiffLen)
        fDC->SetDiffusionLength(cmdDiffLen->GetNewDoubleValue(values));
    else if (cmd == cmdDTI)
        fDC->SetDTIDepth(cmdDiffLen->GetNewDoubleValue(values));
    else if (cmd == cmdDiffStep)
        fDC->SetDiffStep(cmdDiffStep->GetNewDoubleValue(values));
     
    else if (cmd == cmdInterpolation)
        fDC->SetFastMCInterpolation(values);
    else if (cmd == cmdGridSpacing)
        fDC->SetFastMCGridSpacing(cmdGridSpacing->GetNewDoubleValue(values));
    else if (cmd == cmdMaxSpread)
        fDC->SetFastMCMaxSpread(cmdMaxSpread->GetNewIntValue(values));
    else if (cmd == cmdSampleSize)
        fDC->SetFastMCSampleSize(cmdSampleSize->GetNewIntValue(values));


}

G4String PixSensorMessenger::GetCurrentValue(G4UIcommand* cmd)
{

    if (cmd == cmdResXY)
        return cmd->ConvertToString(fDC->GetResXY());
    else if (cmd == cmdPixXY)
        return cmd->ConvertToString(fDC->GetPixXY());
    else if (cmd == cmdPixZ)
        return cmd->ConvertToString(fDC->GetPixZ());
    else if (cmd == cmdGlassZ)
        return cmd->ConvertToString(fDC->GetGlassZ());

    else if (cmd == cmdPixDepl)
        return cmd->ConvertToString(fDC->GetPixDepl());
    else if (cmd == cmdDiffModel)
        return fDC->GetDiffusionModel();
    else if (cmd == cmdDiffLen)
        return cmd->ConvertToString(fDC->GetDiffusionLength());
    else if (cmd == cmdDTI)
        return cmd->ConvertToString(fDC->GetDTIDepth());
    else if (cmd == cmdDiffStep)
        return cmd->ConvertToString(fDC->GetDiffStep());

    else if (cmd == cmdInterpolation)
        return fDC->GetFastMCInterpolation();
    else if (cmd == cmdGridSpacing)
        return cmd->ConvertToString(fDC->GetFastMCGridSpacing());
    else if (cmd == cmdMaxSpread)
        return cmd->ConvertToString(fDC->GetFastMCMaxSpread());
    else if (cmd == cmdSampleSize)
        return cmd->ConvertToString(fDC->GetFastMCSampleSize());

    else return "";
}

