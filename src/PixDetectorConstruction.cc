#include "PixDetectorConstruction.hh"

#include "PixVSensitiveDetector.hh"
#include "PixDepletionSD.hh"
#include "PixMonteCarloSD.hh"
#include "PixIsolationSD.hh"
#include "PixSensorMessenger.hh"

#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4SDManager.hh"
#include "G4Region.hh"
#include "G4VisAttributes.hh"
#include "G4SystemOfUnits.hh"

#include "globals.hh"

PixDetectorConstruction::PixDetectorConstruction()
    :   G4VUserDetectorConstruction(),
        fWorldSize(DEFAULT_WORLD_SIZE),
        fMaxTheta(DEFAULT_MAX_THETA),
        fResXY(DEFAULT_RES_XY),
        fPixXY(DEFAULT_PIX_XY),
        fPixZ(DEFAULT_PIX_Z), 
        fGlassZ(DEFAULT_GLASS_Z),
        fPixDepl(DEFAULT_PIX_DEPL),
        fDiffusionModel(DEFAULT_DIFFUSION_MODEL),
        fDiffusionLength(DEFAULT_DIFFUSION_LENGTH),
        fDiffStep(DEFAULT_DIFF_STEP),
        fFastMCInterpolation(DEFAULT_FAST_MC_INTERPOLATION),
        fFastMCSampleSize(DEFAULT_FAST_MC_SAMPLE_SIZE),
        fFastMCMaxSpread(DEFAULT_FAST_MC_MAX_SPREAD),
        fFastMCGridSpacing(DEFAULT_FAST_MC_GRID_SPACING),
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
        new PixDepletionSD(depletionSDName, depletionHCName);
    G4SDManager::GetSDMpointer()->AddNewDetector(depletionSD);
    SetSensitiveDetector("Depletion", depletionSD, true);


    // add diffusion for substrate
    G4String substrateSDName = "/Pix/SubstrateSD";
    G4String substrateHCName = "SubstrateHC";

    PixVSensitiveDetector* substrateSD;

    if (fDiffusionModel == "MC")
        substrateSD = new PixMonteCarloSD(substrateSDName, substrateHCName, this);
    else if (fDiffusionModel == "Isolation")
        substrateSD = new PixIsolationSD(substrateSDName, substrateHCName, this);
    else return;
    
    G4SDManager::GetSDMpointer()->AddNewDetector(substrateSD); 
    SetSensitiveDetector("Substrate", substrateSD, true);

}


G4VPhysicalVolume* PixDetectorConstruction::Construct()
{
    G4bool checkOverlaps = true;
    G4NistManager* nist = G4NistManager::Instance();

    
    G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");
    G4Material* sensor_mat = nist->FindOrBuildMaterial("G4_Si");
    G4Material* glass_mat = nist->FindOrBuildMaterial("G4_GLASS_PLATE");

    G4Box* solidPix = new G4Box("Pix", 
            0.5*fPixXY, 
            0.5*fPixXY, 
            0.5*fPixZ); 

    G4LogicalVolume* logicPix = 
        new G4LogicalVolume(solidPix, sensor_mat, "Pix");

    G4Box* solidDepl = new G4Box("Depletion",
            0.5*fPixXY,
            0.5*fPixXY,
            0.5*fPixDepl);

    G4LogicalVolume* logicDepl = 
        new G4LogicalVolume(solidDepl, sensor_mat, "Depletion");
    
    
    G4Box* solidSubstrate = new G4Box("Substrate",
            0.5*fPixXY,
            0.5*fPixXY,
            0.5*(fPixZ - fPixDepl));
    
    G4LogicalVolume* logicSubstrate =
        new G4LogicalVolume(solidSubstrate, sensor_mat, "Substrate");

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

    // create world
    G4Box* solidWorld = 
        new G4Box("world",
                0.5*fWorldSize, 0.5*fWorldSize, 0.5*fWorldSize);

    G4LogicalVolume* logicWorld = 
        new G4LogicalVolume(solidWorld,
                world_mat,
                "World");

    G4Box* solidEnvelope = 
        new G4Box("Envelope",
                0.5*fWorldSize, 0.5*fWorldSize, fPixZ + fGlassZ);

    G4LogicalVolume* logicEnvelope = 
        new G4LogicalVolume(solidEnvelope,
                world_mat,
                "Envelope");

    // now make physical placements
    
    // put depletion and substrate in each pixel
    new G4PVPlacement(0,
            G4ThreeVector(0, 0, 0.5*(fPixZ - fPixDepl)),
            logicDepl,
            "DeplPV",
            logicPix,
            false,
            0,
            checkOverlaps);

    new G4PVPlacement(0,
            G4ThreeVector(0, 0, -0.5*fPixDepl),
            logicSubstrate,
            "SubstratePV",
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

    // place glass inside envelope
    if (fGlassZ > 0.0)
    {
        G4Box* solidGlass = new G4Box("Glass",
                0.5*fWorldSize,
                0.5*fWorldSize,
                0.5*fGlassZ);

        G4LogicalVolume* logicGlass =
            new G4LogicalVolume(solidGlass, glass_mat, "Glass");

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


void PixDetectorConstruction::SetMaxTheta(G4double maxTheta)
{
    if (maxTheta > 90*deg) 
    {
        G4cerr << "Angle greater than 90 degrees set as maxTheta.  Ignoring...\n";
        return;
    }
    
    fMaxTheta = maxTheta;
    G4double worldSize = 2 * fGlassZ * tan(maxTheta);
    fWorldSize = std::max(DEFAULT_WORLD_SIZE, worldSize);
}

