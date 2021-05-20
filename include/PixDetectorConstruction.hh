#ifndef PixDetectorConstruction_h
#define PixDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4FieldManager.hh"
#include "G4Material.hh"
#include "G4SystemOfUnits.hh"

class PixSensorMessenger;

class PixDetectorConstruction: public G4VUserDetectorConstruction
{
    public:
        PixDetectorConstruction();
        virtual ~PixDetectorConstruction();

        virtual G4VPhysicalVolume* Construct();
        virtual void ConstructSDandField();

	G4double GetWorldXY();

    public:

        // geometry
        inline void SetResXY(G4double val) { fResXY = val; }
        inline G4int GetResXY() const { return fResXY; }

        inline void SetPixXY(G4double val) { fPixXY = val; }
        inline G4double GetPixXY() const { return fPixXY; }

        inline void SetPixZ(G4double val) { fPixZ = val; }
        inline G4double GetPixZ() const { return fPixZ; }

	inline void SetFacingFront(G4bool val) { fFacingFront = val; }
	inline G4bool GetFacingFront() const { return fFacingFront; }

        inline void SetGlassZ(G4double val) { fGlassZ = val; }
        inline G4double GetGlassZ() const { return fGlassZ; }

	inline void SetShieldingZ(G4double val) { fShieldingZ = val; }
	inline G4double GetShieldingZ() const { return fShieldingZ; }

	inline void SetShieldingMat(G4String val) { fShieldingMat = val; }
	inline G4String GetShieldingMat() const { return fShieldingMat; }
	
	inline void SetBackShieldingZ(G4double val) { fBackShieldingZ = val; }
	inline G4double GetBackShieldingZ() const { return fBackShieldingZ; }

	inline void SetBackShieldingMat(G4String val) { fBackShieldingMat = val; }
	inline G4String GetBackShieldingMat() const { return fBackShieldingMat; }
	
	inline void SetShieldingGap(G4double val) { fShieldingGap = val; }
	inline G4double GetShieldingGap() const { return fShieldingGap; }

	inline void SetShieldingGapMat(G4String val) { fShieldingGapMat = val; }
	inline G4String GetShieldingGapMat() const { return fShieldingGapMat; }


	inline void SetVoltage(G4double val) { fVoltage = val; }
	inline G4double GetVoltage() const { return fVoltage; }

        // readout
        inline void SetPixDepl(G4double val) { fPixDepl = val; }
        inline G4double GetPixDepl() const { return fPixDepl; }

        inline void SetDTIDepth(G4double val) { fDTIDepth = val; };
        inline G4double GetDTIDepth() const { return fDTIDepth; }; 

        inline void SetBackDTI(G4bool val) { fBackDTI = val; };
        inline G4bool GetBackDTI() const { return fBackDTI; };

        inline void SetDiffusionModel(G4String val) { fDiffusionModel = val; };
        inline G4String GetDiffusionModel() const { return fDiffusionModel; };

        inline void SetDiffusionLength(G4double val) { fDiffusionLength = val; };
        inline G4double GetDiffusionLength() const { return fDiffusionLength; }; 
        inline void SetDiffStep(G4double val) { fDiffStep = val; };
        inline G4double GetDiffStep() const { return fDiffStep; };

	inline void SetIonizationEnergy(G4double val) { fIonizationEnergy = val; };
	inline G4double GetIonizationEnergy() const { return fIonizationEnergy; };
	inline void SetIonizationModel(G4String val) { fIonizationModel = val; };
	inline G4String GetIonizationModel() const { return fIonizationModel; };


        // Monte Carlo interpolation
        inline void SetFastMCInterpolation(G4String val) { fFastMCInterpolation = val; };
        inline G4String GetFastMCInterpolation() const { return fFastMCInterpolation; };

        inline void SetFastMCSampleSize(G4int val) { fFastMCSampleSize = val; };
        inline G4int GetFastMCSampleSize() const { return fFastMCSampleSize; };

        inline void SetFastMCMaxSpread(G4int val) { fFastMCMaxSpread = val; };
        inline G4int GetFastMCMaxSpread() const { return fFastMCMaxSpread; };

        inline void SetFastMCGridSpacing(G4double val) { fFastMCGridSpacing = val; };
        inline G4double GetFastMCGridSpacing() const { return fFastMCGridSpacing; };

        inline void SetFastMCFile(G4String val) { fFastMCFile = val; };
        inline G4String GetFastMCFile() const { return fFastMCFile; };

    private:
	G4Material* GetMaterial(G4String mat);

    public:
        static constexpr G4double DEFAULT_WORLD_SIZE = 20*cm;

        static constexpr G4int DEFAULT_RES_XY = 501;
        static constexpr G4double DEFAULT_PIX_XY = 1.12*um;
        static constexpr G4double DEFAULT_PIX_Z = 2.6*um;
        static constexpr G4double DEFAULT_GLASS_Z = 0.0;
	static constexpr G4double DEFAULT_SHIELDING_Z = 0.0;
	static constexpr const char* DEFAULT_SHIELDING_MAT = "G4_AIR";
	static constexpr G4double DEFAULT_BACK_Z = 0.0;
	static constexpr const char* DEFAULT_BACK_MAT = "G4_AIR";
	static constexpr G4double DEFAULT_SHIELDING_GAP = 0.0;
	static constexpr const char* DEFAULT_SHIELDING_GAP_MAT = "G4_AIR";

	static constexpr G4double DEFAULT_VOLTAGE = 2*volt;

        static constexpr G4double DEFAULT_PIX_DEPL = 0.8*um;
        static constexpr G4double DEFAULT_DTI_DEPTH = 1.8*um; 
        static constexpr G4bool DEFAULT_BACK_DTI = true;
        static constexpr const char* DEFAULT_DIFFUSION_MODEL = "Isolation";
        static constexpr G4double DEFAULT_DIFFUSION_LENGTH = 5.0*um;
        static constexpr G4double DEFAULT_DIFF_STEP = 50*nm;
	static constexpr G4double DEFAULT_IONIZATION_ENERGY=3.6*eV;
	static constexpr const char* DEFAULT_IONIZATION_MODEL = "Scale";

        static constexpr const char* DEFAULT_FAST_MC_INTERPOLATION = "Off";
        static constexpr G4int DEFAULT_FAST_MC_SAMPLE_SIZE = 100000;
        static constexpr G4int DEFAULT_FAST_MC_MAX_SPREAD = 7;
        static constexpr G4double DEFAULT_FAST_MC_GRID_SPACING = 100*nm;

                
    private:
        G4int fResXY;
        G4double fPixXY;
        G4double fPixZ;
	G4bool fFacingFront;
        G4double fGlassZ;
	G4double fShieldingZ;
	G4String fShieldingMat;
	G4double fBackShieldingZ;
	G4String fBackShieldingMat;
	G4double fShieldingGap;
	G4String fShieldingGapMat;

	G4double fVoltage;
        
        G4double fPixDepl;
        G4double fDTIDepth;
        G4double fBackDTI;
        G4String fDiffusionModel;
        G4double fDiffusionLength; 
        G4double fDiffStep;
	G4double fIonizationEnergy;
	G4String fIonizationModel;

        G4String fFastMCInterpolation;
        G4int fFastMCSampleSize;
        G4int fFastMCMaxSpread;
        G4double fFastMCGridSpacing;
        G4String fFastMCFile;
        
        G4FieldManager* fPixFieldManager;
	PixSensorMessenger* fSensorMessenger;

};

#endif
