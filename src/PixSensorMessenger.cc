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

    cmdPixXY = new G4UIcmdWithADoubleAndUnit("/sensor/geo/pixPitch", this);
    cmdPixXY->SetGuidance("x and y dimensions of pixels");
    cmdPixXY->SetParameterName("pitch", true);
    cmdPixXY->AvailableForStates(G4State_PreInit);
    cmdPixXY->SetDefaultValue(PixDetectorConstruction::DEFAULT_PIX_XY);
    cmdPixXY->SetDefaultUnit("um");

    cmdPixZ = new G4UIcmdWithADoubleAndUnit("/sensor/geo/pixDepth", this);
    cmdPixZ->SetGuidance("Depth of active sensor region");
    cmdPixZ->SetParameterName("depth", true);
    cmdPixZ->AvailableForStates(G4State_PreInit);
    cmdPixZ->SetDefaultValue(PixDetectorConstruction::DEFAULT_PIX_Z);
    cmdPixZ->SetDefaultUnit("um");

    cmdFacingFront = new G4UIcmdWithABool("/sensor/geo/facingFront", this);
    cmdFacingFront->SetGuidance("Set epitaxial region upstream");
    cmdFacingFront->SetParameterName("facingFront", true);
    cmdFacingFront->AvailableForStates(G4State_PreInit);
    cmdFacingFront->SetDefaultValue(true);

    cmdGlassZ = new G4UIcmdWithADoubleAndUnit("/sensor/geo/glass", this);
    cmdGlassZ->SetGuidance("Thickness of lens");
    cmdGlassZ->SetParameterName("glass", true);
    cmdGlassZ->AvailableForStates(G4State_PreInit);
    cmdGlassZ->SetDefaultValue(PixDetectorConstruction::DEFAULT_GLASS_Z);
    cmdGlassZ->SetDefaultUnit("mm");

    cmdShieldingZ = new G4UIcmdWithADoubleAndUnit("/sensor/geo/shieldZ", this);
    cmdShieldingZ->SetGuidance("Thickness of external shielding");
    cmdShieldingZ->SetParameterName("shield", true);
    cmdShieldingZ->AvailableForStates(G4State_PreInit);
    cmdShieldingZ->SetDefaultValue(PixDetectorConstruction::DEFAULT_SHIELDING_Z);
    cmdShieldingZ->SetDefaultUnit("mm");

    cmdShieldingMat = new G4UIcmdWithAString("/sensor/geo/shieldMat", this);
    cmdShieldingMat->SetGuidance("Shielding material, from G4 NIST database");
    cmdShieldingMat->SetParameterName("mat", true);
    cmdShieldingMat->AvailableForStates(G4State_PreInit);
    cmdShieldingMat->SetDefaultValue(PixDetectorConstruction::DEFAULT_SHIELDING_MAT);

    cmdBackShieldingZ = new G4UIcmdWithADoubleAndUnit("/sensor/geo/backShieldZ", this);
    cmdBackShieldingZ->SetGuidance("Thickness of shielding for backscattering");
    cmdBackShieldingZ->SetParameterName("backshield", true);
    cmdBackShieldingZ->AvailableForStates(G4State_PreInit);
    cmdBackShieldingZ->SetDefaultValue(PixDetectorConstruction::DEFAULT_BACK_Z);
    cmdBackShieldingZ->SetDefaultUnit("mm");

    cmdBackShieldingMat = new G4UIcmdWithAString("/sensor/geo/backShieldMat", this);
    cmdBackShieldingMat->SetGuidance("Backscattering shielding material, from G4 NIST database");
    cmdBackShieldingMat->SetParameterName("backmat", true);
    cmdBackShieldingMat->AvailableForStates(G4State_PreInit);
    cmdBackShieldingMat->SetDefaultValue(PixDetectorConstruction::DEFAULT_BACK_MAT);

    cmdShieldingGap = new G4UIcmdWithADoubleAndUnit("/sensor/geo/shieldGap", this);
    cmdShieldingGap->SetGuidance("Gap between glass and shielding surfaces");
    cmdShieldingGap->SetParameterName("gap", true);
    cmdShieldingGap->AvailableForStates(G4State_PreInit);
    cmdShieldingGap->SetDefaultValue(PixDetectorConstruction::DEFAULT_SHIELDING_GAP);
    cmdShieldingGap->SetDefaultUnit("mm");

    cmdShieldingGapMat = new G4UIcmdWithAString("/sensor/geo/shieldGapMat", this);
    cmdShieldingGapMat->SetGuidance("Shielding material, from G4 NIST database");
    cmdShieldingGapMat->SetParameterName("mat", true);
    cmdShieldingGapMat->AvailableForStates(G4State_PreInit);
    cmdShieldingGapMat->SetDefaultValue(PixDetectorConstruction::DEFAULT_SHIELDING_GAP_MAT);

    cmdVoltage = new G4UIcmdWithADoubleAndUnit("/sensor/geo/voltage", this);
    cmdVoltage->SetGuidance("Voltage inside pixel");
    cmdVoltage->SetParameterName("voltage", true);
    cmdVoltage->AvailableForStates(G4State_PreInit);
    cmdVoltage->SetDefaultValue(PixDetectorConstruction::DEFAULT_VOLTAGE);
    cmdVoltage->SetDefaultUnit("volt");


    fReadoutDir = new G4UIdirectory("/sensor/readout/");
    fReadoutDir->SetGuidance("Pixel readout parameters");

    cmdPixDepl = new G4UIcmdWithADoubleAndUnit("/sensor/readout/depletion", this);
    cmdPixDepl->SetGuidance("Length of depletion region");
    cmdPixDepl->SetParameterName("depletion", true);
    cmdPixDepl->AvailableForStates(G4State_PreInit);
    cmdPixDepl->SetDefaultValue(PixDetectorConstruction::DEFAULT_PIX_DEPL);
    cmdPixDepl->SetDefaultUnit("um");

    cmdDTIDepth = new G4UIcmdWithADoubleAndUnit("/sensor/readout/dtiDepth", this);
    cmdDTIDepth->SetGuidance("Depth of Deep Trench Isolation (DTI)");
    cmdDTIDepth->SetParameterName("depth", true);
    cmdDTIDepth->AvailableForStates(G4State_PreInit);
    cmdDTIDepth->SetDefaultValue(PixDetectorConstruction::DEFAULT_DTI_DEPTH);
    cmdDTIDepth->SetDefaultUnit("um");

    cmdBackDTI = new G4UIcmdWithABool("/sensor/readout/backDTI", this);
    cmdBackDTI->SetGuidance("Use B-DTI (true) or F-DTI (false)");
    cmdBackDTI->AvailableForStates(G4State_PreInit);
    cmdBackDTI->SetDefaultValue(PixDetectorConstruction::DEFAULT_BACK_DTI);

    cmdDiffModel = new G4UIcmdWithAString("/sensor/readout/diffusionModel", this);
    cmdDiffModel->SetGuidance("Model for distribution of electrons created outside the depletion region.");
    cmdDiffModel->SetCandidates("MC Isolation");
    cmdDiffModel->SetDefaultValue(PixDetectorConstruction::DEFAULT_DIFFUSION_MODEL);

    cmdDiffLen = new G4UIcmdWithADoubleAndUnit("/sensor/readout/diffusion", this);
    cmdDiffLen->SetGuidance("Electron diffusion length");
    cmdDiffLen->SetParameterName("L_diff", true);
    cmdDiffLen->AvailableForStates(G4State_PreInit);
    cmdDiffLen->SetDefaultValue(PixDetectorConstruction::DEFAULT_DIFFUSION_LENGTH);
    cmdDiffLen->SetDefaultUnit("um"); 

    cmdDiffStep = new G4UIcmdWithADoubleAndUnit("/sensor/readout/diffStep", this);
    cmdDiffStep->SetGuidance("Size of steps in Monte Carlo diffusion of charge");
    cmdDiffStep->SetParameterName("stepSize", true);
    cmdDiffStep->AvailableForStates(G4State_PreInit);
    cmdDiffStep->SetDefaultValue(PixDetectorConstruction::DEFAULT_DIFF_STEP);
    cmdDiffStep->SetDefaultUnit("nm");

    cmdIonizationEnergy = new G4UIcmdWithADoubleAndUnit("/sensor/readout/ionizationEnergy", this);
    cmdIonizationEnergy->SetGuidance("Approximate deposited energy per electron-hole pair created");
    cmdIonizationEnergy->AvailableForStates(G4State_PreInit, G4State_Idle);
    cmdIonizationEnergy->SetParameterName("eIon", true);
    cmdIonizationEnergy->SetDefaultValue(PixDetectorConstruction::DEFAULT_IONIZATION_ENERGY);
    cmdIonizationEnergy->SetDefaultUnit("eV");

    cmdIonizationModel = new G4UIcmdWithAString("/sensor/readout/ionizationModel", this);
    cmdIonizationModel->SetGuidance("Model for converting deposited energy to electron-hole pairs");
    cmdIonizationModel->AvailableForStates(G4State_PreInit, G4State_Idle);
    cmdIonizationModel->SetDefaultValue(PixDetectorConstruction::DEFAULT_IONIZATION_MODEL);
    cmdIonizationModel->SetCandidates("Scale Binom");

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
    
    cmdFromFile = new G4UIcmdWithAString("/sensor/fastMC/file", this);
    cmdFromFile->SetGuidance("Load diffusion distribution from a file");
    cmdFromFile->SetGuidance("(calculated from a previous run)");
    cmdFromFile->SetParameterName("filename", true);
    cmdFromFile->AvailableForStates(G4State_PreInit);
    cmdFromFile->SetDefaultValue("");

}

PixSensorMessenger::~PixSensorMessenger()
{    
    delete cmdResXY;
    delete cmdPixXY;
    delete cmdPixZ; 
    delete cmdGlassZ;
    delete cmdShieldingZ;
    delete cmdShieldingMat;
    delete cmdBackShieldingZ;
    delete cmdBackShieldingMat;
    delete cmdShieldingGap;
    delete cmdShieldingGapMat;
    delete cmdVoltage;

    delete fGeometryDir; 
    
    delete cmdPixDepl;
    delete cmdDTIDepth;
    delete cmdBackDTI;
    delete cmdDiffModel;
    delete cmdDiffLen; 
    delete cmdDiffStep;
    delete cmdIonizationEnergy;
    delete cmdIonizationModel;

    delete fReadoutDir;

    delete cmdInterpolation;
    delete cmdGridSpacing;
    delete cmdMaxSpread;
    delete cmdSampleSize;
    delete cmdFromFile;

    delete fFastMCDir;
    

}

void PixSensorMessenger::SetNewValue(G4UIcommand* cmd, G4String values)
{
    if (cmd == cmdResXY)
        fDC->SetResXY(cmdResXY->GetNewIntValue(values));
    else if (cmd == cmdPixXY)
        fDC->SetPixXY(cmdPixXY->GetNewDoubleValue(values));
    else if (cmd == cmdPixZ)
        fDC->SetPixZ(cmdPixZ->GetNewDoubleValue(values));
    else if (cmd == cmdFacingFront)
	fDC->SetFacingFront(cmdFacingFront->GetNewBoolValue(values));
    else if (cmd == cmdGlassZ)
        fDC->SetGlassZ(cmdGlassZ->GetNewDoubleValue(values));
    else if (cmd == cmdShieldingZ)
	fDC->SetShieldingZ(cmdShieldingZ->GetNewDoubleValue(values));
    else if (cmd == cmdShieldingMat)
	fDC->SetShieldingMat(values);
    else if (cmd == cmdBackShieldingZ)
	fDC->SetBackShieldingZ(cmdBackShieldingZ->GetNewDoubleValue(values));
    else if (cmd == cmdBackShieldingMat)
	fDC->SetBackShieldingMat(values);
    else if (cmd == cmdShieldingGap)
	fDC->SetShieldingGap(cmdShieldingGap->GetNewDoubleValue(values));
    else if (cmd == cmdShieldingGapMat)
	fDC->SetShieldingGapMat(values);
    
    else if (cmd == cmdVoltage)
	fDC->SetVoltage(cmdVoltage->GetNewDoubleValue(values));

    else if (cmd == cmdPixDepl)
        fDC->SetPixDepl(cmdPixDepl->GetNewDoubleValue(values));
    else if (cmd == cmdDTIDepth)
        fDC->SetDTIDepth(cmdDTIDepth->GetNewDoubleValue(values));
    else if (cmd == cmdBackDTI)
        fDC->SetBackDTI(cmdBackDTI->GetNewBoolValue(values));
    else if (cmd == cmdDiffModel)
        fDC->SetDiffusionModel(values);
    else if (cmd == cmdDiffLen)
        fDC->SetDiffusionLength(cmdDiffLen->GetNewDoubleValue(values));
    else if (cmd == cmdDiffStep)
        fDC->SetDiffStep(cmdDiffStep->GetNewDoubleValue(values));
    else if (cmd == cmdIonizationEnergy)
	fDC->SetIonizationEnergy(cmdIonizationEnergy->GetNewDoubleValue(values));
    else if (cmd == cmdIonizationModel)
	fDC->SetIonizationModel(values); 

    else if (cmd == cmdInterpolation)
        fDC->SetFastMCInterpolation(values);
    else if (cmd == cmdGridSpacing)
        fDC->SetFastMCGridSpacing(cmdGridSpacing->GetNewDoubleValue(values));
    else if (cmd == cmdMaxSpread)
        fDC->SetFastMCMaxSpread(cmdMaxSpread->GetNewIntValue(values));
    else if (cmd == cmdSampleSize)
        fDC->SetFastMCSampleSize(cmdSampleSize->GetNewIntValue(values));
    else if (cmd == cmdFromFile)
        fDC->SetFastMCFile(values);

}

G4String PixSensorMessenger::GetCurrentValue(G4UIcommand* cmd)
{

    if (cmd == cmdResXY)
        return cmd->ConvertToString(fDC->GetResXY());
    else if (cmd == cmdPixXY)
        return cmd->ConvertToString(fDC->GetPixXY());
    else if (cmd == cmdPixZ)
        return cmd->ConvertToString(fDC->GetPixZ());
    else if (cmd == cmdFacingFront)
	return cmd->ConvertToString(fDC->GetFacingFront());
    else if (cmd == cmdGlassZ)
        return cmd->ConvertToString(fDC->GetGlassZ());
    else if (cmd == cmdShieldingZ)
	return cmd->ConvertToString(fDC->GetShieldingZ());
    else if (cmd == cmdShieldingMat)
	return cmd->ConvertToString(fDC->GetShieldingMat());
    else if (cmd == cmdBackShieldingZ)
	return cmd->ConvertToString(fDC->GetBackShieldingZ());
    else if (cmd == cmdBackShieldingMat)
	return cmd->ConvertToString(fDC->GetBackShieldingMat());
    else if (cmd == cmdShieldingGap)
	return cmd->ConvertToString(fDC->GetShieldingGap());
    else if (cmd == cmdShieldingGapMat)
	return cmd->ConvertToString(fDC->GetShieldingGapMat());

    else if (cmd == cmdVoltage)
	return cmd->ConvertToString(fDC->GetVoltage());

    else if (cmd == cmdPixDepl)
        return cmd->ConvertToString(fDC->GetPixDepl());
    else if (cmd == cmdDTIDepth)
        return cmd->ConvertToString(fDC->GetDTIDepth());
    else if (cmd == cmdBackDTI)
        return cmd->ConvertToString(fDC->GetBackDTI());
    else if (cmd == cmdDiffModel)
        return fDC->GetDiffusionModel();
    else if (cmd == cmdDiffLen)
        return cmd->ConvertToString(fDC->GetDiffusionLength());
    else if (cmd == cmdDiffStep)
        return cmd->ConvertToString(fDC->GetDiffStep());
    else if (cmd == cmdIonizationEnergy)
	return cmd->ConvertToString(fDC->GetIonizationEnergy());
    else if (cmd == cmdIonizationModel)
	return fDC->GetIonizationModel();

    else if (cmd == cmdInterpolation)
        return fDC->GetFastMCInterpolation();
    else if (cmd == cmdGridSpacing)
        return cmd->ConvertToString(fDC->GetFastMCGridSpacing());
    else if (cmd == cmdMaxSpread)
        return cmd->ConvertToString(fDC->GetFastMCMaxSpread());
    else if (cmd == cmdSampleSize)
        return cmd->ConvertToString(fDC->GetFastMCSampleSize());
    else if (cmd == cmdFromFile)
        return cmd->ConvertToString(fDC->GetFastMCFile());

    else return "";
}

