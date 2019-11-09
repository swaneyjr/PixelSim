#ifndef PixDetectorConstruction_h
#define PixDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"

class PixSensorMessenger;

class PixDetectorConstruction: public G4VUserDetectorConstruction
{
    public:
        PixDetectorConstruction();
        virtual ~PixDetectorConstruction();

        virtual G4VPhysicalVolume* Construct();
        virtual void ConstructSDandField();

        // geometry
        inline void SetResXY(G4double val) { fResXY = val; }
        inline G4int GetResXY() const { return fResXY; }

        inline void SetPixXY(G4double val) { fPixXY = val; }
        inline G4double GetPixXY() const { return fPixXY; }

        inline void SetPixZ(G4double val) { fPixZ = val; }
        inline G4double GetPixZ() const { return fPixZ; }
 
        inline void SetGlassZ(G4double val) { fGlassZ = val; }
        inline G4double GetGlassZ() const { return fGlassZ; }


        // readout
        inline void SetPixDepl(G4double val) { fPixDepl = val; }
        inline G4double GetPixDepl() const { return fPixDepl; }

        inline void SetDiffusionModel(G4String val) { fDiffusionModel = val; };
        inline G4String GetDiffusionModel() const { return fDiffusionModel; };

        inline void SetDiffusionLength(G4double val) { fDiffusionLength = val; };
        inline G4double GetDiffusionLength() const { return fDiffusionLength; };

        inline void SetDTIDepth(G4double val) { fDTI = val; };
        inline G4double GetDTIDepth() const { return fDTI; }; 

        inline void SetMCStep(G4double val) { fMCStep = val; };
        inline G4double GetMCStep() const { return fMCStep; };

        void SetMaxTheta(G4double maxTheta);
        inline G4double GetMaxTheta() const { return fMaxTheta; };


    public:
        static constexpr G4double DEFAULT_WORLD_SIZE = 5*mm;
        static constexpr G4double DEFAULT_MAX_THETA = 85*deg;
        static constexpr G4int DEFAULT_RES_XY = 51;
        static constexpr G4double DEFAULT_PIX_XY = 1.12*um;
        static constexpr G4double DEFAULT_PIX_Z = 2.6*um;
        static constexpr G4double DEFAULT_GLASS_Z = 0.0;

        static constexpr G4double DEFAULT_PIX_DEPL = 0.8*um;
        inline static const G4String DEFAULT_DIFFUSION_MODEL = "Isolation";
        static constexpr G4double DEFAULT_DIFFUSION_LENGTH = 5.0*um;
        static constexpr G4double DEFAULT_DTI_DEPTH = 1.8*um; 
        static constexpr G4double DEFAULT_MC_STEP = 50*nm;
                
    private:
        G4double fWorldSize;
        G4double fMaxTheta;

        G4int fResXY;
        G4double fPixXY;
        G4double fPixZ;
        G4double fGlassZ;
        
        G4double fPixDepl;
        G4String fDiffusionModel;
        G4double fDiffusionLength;
        G4double fDTI;
        G4double fMCStep;
        
        PixSensorMessenger* fSensorMessenger;

};

#endif
