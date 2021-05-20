#include "PixDetectorConstruction.hh"

#include "PixVSensitiveDetector.hh"
#include "PixDepletionSD.hh"
#include "PixMonteCarloSD.hh"
#include "PixIsolationSD.hh"
#include "PixSensorMessenger.hh"
#include "PixPrimaryGeneratorAction.hh"
#include "PixPrimaryGenerator.hh"
#include "PixAngDistribution.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4SDManager.hh"
#include "G4Region.hh"
#include "G4VisAttributes.hh"
#include "G4SystemOfUnits.hh"

#include "G4UniformElectricField.hh"
#include "G4EqMagElectricField.hh"
#include "G4DormandPrince745.hh"
#include "G4ChordFinder.hh"
#include "G4IntegrationDriver.hh"

#include "globals.hh"

PixDetectorConstruction::PixDetectorConstruction()
    :   G4VUserDetectorConstruction(),
        fResXY(DEFAULT_RES_XY),
        fPixXY(DEFAULT_PIX_XY),
        fPixZ(DEFAULT_PIX_Z), 
	fFacingFront(true),
        fGlassZ(DEFAULT_GLASS_Z),
	fShieldingZ(DEFAULT_SHIELDING_Z),
	fShieldingMat(DEFAULT_SHIELDING_MAT),
	fBackShieldingZ(DEFAULT_BACK_Z),
	fBackShieldingMat(DEFAULT_BACK_MAT),
	fShieldingGap(DEFAULT_SHIELDING_GAP),
	fShieldingGapMat(DEFAULT_SHIELDING_GAP_MAT),
	fVoltage(DEFAULT_VOLTAGE),
        fPixDepl(DEFAULT_PIX_DEPL),
        fDTIDepth(DEFAULT_DTI_DEPTH),
        fBackDTI(DEFAULT_BACK_DTI),
        fDiffusionModel(DEFAULT_DIFFUSION_MODEL),
        fDiffusionLength(DEFAULT_DIFFUSION_LENGTH),
        fDiffStep(DEFAULT_DIFF_STEP),
	fIonizationEnergy(DEFAULT_IONIZATION_ENERGY),
        fIonizationModel(DEFAULT_IONIZATION_MODEL),
	fFastMCInterpolation(DEFAULT_FAST_MC_INTERPOLATION),
        fFastMCSampleSize(DEFAULT_FAST_MC_SAMPLE_SIZE),
        fFastMCMaxSpread(DEFAULT_FAST_MC_MAX_SPREAD),
        fFastMCGridSpacing(DEFAULT_FAST_MC_GRID_SPACING),
        fFastMCFile(""),
	fPixFieldManager(new G4FieldManager(0,0,true)),
        fSensorMessenger(new PixSensorMessenger(this))
{ }

PixDetectorConstruction::~PixDetectorConstruction()
{ 
    delete fSensorMessenger;
}

void PixDetectorConstruction::ConstructSDandField()
{
         
    // add depletion zone
    G4String depletionSDName = "/Pix/DepletionSD";
    G4String depletionHCName = "DepletionHC";
    
    PixVSensitiveDetector* depletionSD = 
        new PixDepletionSD(depletionSDName, depletionHCName, this);
    G4SDManager::GetSDMpointer()->AddNewDetector(depletionSD);
    SetSensitiveDetector("Depletion", depletionSD, true);


    // add diffusion for epitaxial region
    G4String epiSDName = "/Pix/EpiSD";
    G4String epiHCName = "EpiHC";

    PixVSensitiveDetector* epiSD;

    if (fDiffusionModel == "MC")
        epiSD = new PixMonteCarloSD(epiSDName, epiHCName, this);
    else if (fDiffusionModel == "Isolation")
        epiSD = new PixIsolationSD(epiSDName, epiHCName, this);
    else return;
    
    G4SDManager::GetSDMpointer()->AddNewDetector(epiSD); 
    SetSensitiveDetector("Epi", epiSD, true);

    // add electric field in pixels
    G4ThreeVector fieldVec = G4ThreeVector(0, 0, -fVoltage/fPixZ);
    G4UniformElectricField* eField = new G4UniformElectricField(fieldVec);
    fPixFieldManager->SetDetectorField(eField);

    G4int nvar = 8;
    G4double minStep = 0.1*um;
    G4EqMagElectricField* eqOfMotion = new G4EqMagElectricField(eField);
    auto stepper = new G4DormandPrince745(eqOfMotion, nvar);
    auto integrationDriver = new G4IntegrationDriver<G4DormandPrince745>
	    (minStep, stepper, nvar);

    G4ChordFinder* chordFinder = new G4ChordFinder(integrationDriver);
    fPixFieldManager->SetChordFinder(chordFinder);
}


G4VPhysicalVolume* PixDetectorConstruction::Construct()
{
    G4bool checkOverlaps = true;
    G4NistManager* nist = G4NistManager::Instance();
    
    G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");
    G4Material* sensor_mat = nist->FindOrBuildMaterial("G4_Si");
    G4Material* glass_mat = nist->FindOrBuildMaterial("G4_GLASS_PLATE");
    G4Material* shielding_mat = GetMaterial(fShieldingMat);
    G4Material* backshielding_mat = GetMaterial(fBackShieldingMat);
    G4Material* gap_mat = GetMaterial(fShieldingGapMat);

    G4Box* solidPix = new G4Box("Pix", 
            0.5*fPixXY, 
            0.5*fPixXY, 
            0.5*fPixZ); 

    G4LogicalVolume* logicPix = 
        new G4LogicalVolume(solidPix, sensor_mat, "Pix");

    // add field inside each pixel
    logicPix->SetFieldManager(fPixFieldManager, true);

    G4Box* solidDepl = new G4Box("Depletion",
            0.5*fPixXY,
            0.5*fPixXY,
            0.5*fPixDepl);

    G4LogicalVolume* logicDepl = 
        new G4LogicalVolume(solidDepl, sensor_mat, "Depletion");
    
    
    G4Box* solidEpi = new G4Box("Epi",
            0.5*fPixXY,
            0.5*fPixXY,
            0.5*(fPixZ - fPixDepl));
    
    G4LogicalVolume* logicEpi =
        new G4LogicalVolume(solidEpi, sensor_mat, "Epi");

    G4Box* solidRow = new G4Box("Row", 
            0.5*fPixXY*fResXY, 
            0.5*fPixXY, 
            0.5*fPixZ);

    G4LogicalVolume* logicRow = 
        new G4LogicalVolume(solidRow, sensor_mat, "Row"); 

    G4Box* solidSensor = new G4Box("Sensor", 
            0.5*fPixXY*fResXY, 
            0.5*fPixXY*fResXY, 
            0.5*fPixZ);
    
    G4LogicalVolume* logicSensor = 
        new G4LogicalVolume(solidSensor, sensor_mat, "Sensor"); 

    // add sensor as new region for low-energy propagation
    G4Region* sensorRegion = new G4Region("sensorRegion");
    logicSensor->SetRegion(sensorRegion);
    sensorRegion->AddRootLogicalVolume(logicSensor);

    // create world with necessary dimensions
    G4double worldXY = GetWorldXY();
    G4double thickness = std::max(fGlassZ + fShieldingZ + fShieldingGap, fPixZ) + fBackShieldingZ;
    G4double worldZ = std::max(DEFAULT_WORLD_SIZE, thickness);

    G4Box* solidWorld = 
        new G4Box("world",
                0.5*worldXY, 0.5*worldXY, 0.5*worldZ);

    G4LogicalVolume* logicWorld = 
        new G4LogicalVolume(solidWorld,
                world_mat,
                "World");

    G4Box* solidEnvelope = 
        new G4Box("Envelope",
                0.5*worldXY, 0.5*worldXY, thickness);

    G4LogicalVolume* logicEnvelope = 
        new G4LogicalVolume(solidEnvelope,
                world_mat,
                "Envelope");

    // now make physical placements
    
    // put depletion and epitaxial regions in each pixel
    G4int pixSign = -1; 
    if (fFacingFront) pixSign = 1;
    new G4PVPlacement(0,
            G4ThreeVector(0, 0, -pixSign*0.5*(fPixZ - fPixDepl)),
            logicDepl,
            "DeplPV",
            logicPix,
            false,
            0,
            checkOverlaps);

    new G4PVPlacement(0,
            G4ThreeVector(0, 0, pixSign*0.5*fPixDepl),
            logicEpi,
            "EpiPV",
            logicPix,
            false,
            0,
            checkOverlaps);
    

    // make 2D grid of pixels
    new G4PVReplica("PixPV",
            logicPix,
            logicRow,
            kXAxis,
            fResXY,
            fPixXY,
            0);

    new G4PVReplica("RowPV",
            logicRow,
            logicSensor,
            kYAxis,
            fResXY,
            fPixXY,
            0); 

    // place sensor inside envelope
    new G4PVPlacement(0,
            G4ThreeVector(0, 0, -0.5*fPixZ),
            logicSensor,
            "SensorPV",
            logicEnvelope,
            false,
            0,
            checkOverlaps);

    // add new region with lens glass and additional shielding
    G4Region* glassShieldingRegion = new G4Region("glassShieldingRegion");
    
    // place glass inside envelope
    if (fGlassZ > 0.0)
    {
        G4Box* solidGlass = new G4Box("Glass",
                0.5*worldXY,
                0.5*worldXY,
                0.5*fGlassZ);

        G4LogicalVolume* logicGlass =
            new G4LogicalVolume(solidGlass, glass_mat, "Glass");

	logicGlass->SetRegion(glassShieldingRegion);
    	glassShieldingRegion->AddRootLogicalVolume(logicGlass);

        // make transparent
        G4VisAttributes glassAttr = G4VisAttributes();
        glassAttr.SetColor(0.8, 0.8, 0.8, 0.3);

        logicGlass->SetVisAttributes(glassAttr);

        new G4PVPlacement(0,
                G4ThreeVector(0, 0, 0.5*fGlassZ),
                logicGlass,
                "GlassPV",
                logicEnvelope,
                false,
                0,
                checkOverlaps);
    }

    if (fShieldingZ > 0.0) {
	G4Box* solidShielding = new G4Box("Shielding",
                0.5*worldXY,
                0.5*worldXY,
                0.5*fShieldingZ);

        G4LogicalVolume* logicShielding =
            new G4LogicalVolume(solidShielding, shielding_mat, "Shielding");

        logicShielding->SetRegion(glassShieldingRegion);
        glassShieldingRegion->AddRootLogicalVolume(logicShielding);

        // make transparent
        G4VisAttributes shieldingAttr = G4VisAttributes();
        shieldingAttr.SetColor(0.0, 0.8, 0.0, 0.3);

        logicShielding->SetVisAttributes(shieldingAttr);

        new G4PVPlacement(0,
                G4ThreeVector(0, 0, 0.5*fShieldingZ + fShieldingGap + fGlassZ),
                logicShielding,
                "ShieldingPV",
                logicEnvelope,
                false,
                0,
                checkOverlaps);

    }
    
    if (fBackShieldingZ > 0.0) {
	G4Box* solidBackShielding = new G4Box("BackShielding",
                0.5*worldXY,
                0.5*worldXY,
                0.5*fBackShieldingZ);

        G4LogicalVolume* logicBackShielding =
            new G4LogicalVolume(solidBackShielding, backshielding_mat, "BackShielding");

        logicBackShielding->SetRegion(glassShieldingRegion);
        glassShieldingRegion->AddRootLogicalVolume(logicBackShielding);

        // make transparent
        G4VisAttributes shieldingAttr = G4VisAttributes();
        shieldingAttr.SetColor(0.8, 0.0, 0.0, 0.3);

        logicBackShielding->SetVisAttributes(shieldingAttr);

	// put one behind the sensor for more backscattering
	new G4PVPlacement(0,
                G4ThreeVector(0, 0, -0.5*fBackShieldingZ - fPixZ),
                logicBackShielding,
                "BackShieldingPV1",
                logicEnvelope,
                false,
                0,
                checkOverlaps);

	new G4PVPlacement(0,
		G4ThreeVector(0, 0, 0.5*fBackShieldingZ + fShieldingZ + fShieldingGap + fGlassZ),
		logicBackShielding,
		"BackShieldingPV2",
		logicEnvelope,
		false,
		0,
		checkOverlaps);
    }

    if (fShieldingGap > 0.0 && fShieldingGapMat != "G4_AIR") {
	G4Box* solidGap = new G4Box("Gap",
                0.5*worldXY,
                0.5*worldXY,
                0.5*fShieldingGap);

        G4LogicalVolume* logicGap =
            new G4LogicalVolume(solidGap, gap_mat, "Gap");

        logicGap->SetRegion(glassShieldingRegion);
        glassShieldingRegion->AddRootLogicalVolume(logicGap);

        // make transparent
        G4VisAttributes shieldingAttr = G4VisAttributes();
        shieldingAttr.SetColor(0.8, 0.8, 0.8, 0.3);

        logicGap->SetVisAttributes(shieldingAttr);

        new G4PVPlacement(0,
                G4ThreeVector(0, 0, 0.5*fShieldingGap + fGlassZ),
                logicGap,
                "GapPV",
                logicEnvelope,
                false,
                0,
                checkOverlaps);

    }

    // place envelope inside world
    new G4PVPlacement(0,
            G4ThreeVector(),
            logicEnvelope,
            "Envelope",
            logicWorld,
            false,
            0,
            checkOverlaps); 

    
    G4VPhysicalVolume* physWorld =
        new G4PVPlacement(0,
                G4ThreeVector(),
                logicWorld,
                "World",
                0,
                false,
                0,
                checkOverlaps);

    return physWorld;

}

G4double PixDetectorConstruction::GetWorldXY()
{
    G4RunManager* runMan = G4RunManager::GetRunManager();
    PixPrimaryGeneratorAction* pga = (PixPrimaryGeneratorAction*)
            runMan->GetUserPrimaryGeneratorAction();
    PixPrimaryGenerator* gun = pga->GetPrimaryGenerator();
    G4String sourceType = gun->GetSourceType();

    if (sourceType == "point") return DEFAULT_WORLD_SIZE;

    G4double maxTheta = gun->GetAngDist()->GetMaxTheta();
    G4double worldSize = 2 * (fGlassZ + fShieldingZ + fShieldingGap) * tan(maxTheta);
    return std::max(DEFAULT_WORLD_SIZE, worldSize);
}

G4Material* PixDetectorConstruction::GetMaterial(G4String mat)
{

    G4Element* H = new G4Element("Hydrogen", "H", 1., 1.00794*g/mole);
    G4Element* C  = new G4Element("Carbon", "C", 6., 12.011*g/mole);
    G4Element* O  = new G4Element("Oxygen", "O", 8., 16.00*g/mole);

    if (mat.rfind("G4", 0) == 0) {
        G4NistManager* nist = G4NistManager::Instance();
	return nist->FindOrBuildMaterial(mat);
    } else if (mat == "wood") {
        G4Material* wood = new G4Material("wood", 0.9*g/cm3, 3);
        wood->AddElement(H, 4);
        wood->AddElement(C, 2);
	wood->AddElement(O, 1);
        return wood;
    } else if (mat == "PLA") {
        G4Material* pla = new G4Material("PLA", 1.3*g/cm3, 3);
	pla->AddElement(H, 4);
	pla->AddElement(C, 3);
	pla->AddElement(O, 2);
	return pla;
    }
    
    return 0;

}
