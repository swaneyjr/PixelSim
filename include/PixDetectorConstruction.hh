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

        inline void SetResXY(G4double resXY) { fResXY = resXY; }
        inline G4int GetResXY() const { return fResXY; }

        inline void SetPixXY(G4double pixXY) { fPixXY = pixXY; }
        inline G4double GetPixXY() const { return fPixXY; }

        inline void SetPixZ(G4double pixZ) { fPixZ = pixZ; }
        inline G4double GetPixZ() const { return fPixZ; }

        inline void SetPixDepl(G4double depl) { fPixDepl = depl; }
        inline G4double GetPixDepl() const { return fPixDepl; }

        inline void SetGlassZ(G4double glassZ) { fGlassZ = glassZ; }
        inline G4double GetGlassZ() const { return fGlassZ; }
 
        void SetMaxTheta(G4double maxTheta);
        inline G4double GetMaxTheta() const { return fMaxTheta; };


    public:
        static constexpr G4double DEFAULT_WORLD_SIZE = 5*mm;
        static constexpr G4double DEFAULT_MAX_THETA = 85*deg;
        static constexpr G4int DEFAULT_RES_XY = 51;
        static constexpr G4double DEFAULT_PIX_XY = 1.12*um;
        static constexpr G4double DEFAULT_PIX_Z = 2.6*um;
        static constexpr G4double DEFAULT_PIX_DEPL = 1.3*um;
        static constexpr G4double DEFAULT_GLASS_Z = 0.0;
                
    private:
        G4double fWorldSize;
        G4double fMaxTheta;

        G4int fResXY;
        G4double fPixXY;
        G4double fPixZ;
        G4double fPixDepl;
        G4double fGlassZ;
        
        PixSensorMessenger* fSensorMessenger;

};

#endif
