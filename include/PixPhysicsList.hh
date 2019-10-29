#ifndef PixPhysicsList_h
#define PixPhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "G4UserLimits.hh"

#include "G4SystemOfUnits.hh"

class PixPhysMessenger;

class PixPhysicsList : public G4VModularPhysicsList
{
    public:
        PixPhysicsList();
        virtual ~PixPhysicsList();

        virtual void SetCuts();

        inline void SetMaxStepSensor(G4double val) { fMaxStepSensor = val; };
        inline G4double GetMaxStepSensor() const { return fMaxStepSensor; }

        inline void SetRangeCutWorld(G4double val) { defaultCutValue = val; };
        inline G4double GetRangeCutWorld() const { return defaultCutValue; };

        inline void SetRangeCutSensor(G4double val) { fRangeCutSensor = val; };
        inline G4double GetRangeCutSensor() const { return fRangeCutSensor; };

        inline void SetMinProdThresh(G4double val) { fRangeCutSensor = val; };
        inline G4double GetMinProdThresh() const { return fRangeCutSensor; };

        static constexpr G4double DEFAULT_MAX_STEP_SENSOR = -1;
        static constexpr G4double DEFAULT_RANGE_CUT_WORLD = 0.7*mm;
        static constexpr G4double DEFAULT_RANGE_CUT_SENSOR = 0.1*um;
        static constexpr G4double DEFAULT_MIN_PROD_THRESH = 50*eV;

    private: 
        G4double fMaxStepSensor;
        G4double fRangeCutWorld;
        G4double fRangeCutSensor;
        G4double fMinProdThresh;

        G4UserLimits* fStepLimit;
        
        PixPhysMessenger* fPhysMessenger;

};

#endif
