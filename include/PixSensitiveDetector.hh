#ifndef PixSensitiveDetector_h
#define PixSensitiveDetector_h 1

#include "G4VSensitiveDetector.hh"
#include "G4SystemOfUnits.hh"

#include "PixHit.hh"

class PixSensitiveDetector : public G4VSensitiveDetector
{
    public:
        PixSensitiveDetector(const G4String& name, const G4String& hitsCollectionName);
        virtual ~PixSensitiveDetector();

        virtual void Initialize(G4HCofThisEvent* hitCollection);
        virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory*);
        virtual void EndOfEvent(G4HCofThisEvent* hitCollection);

        inline void SetChargeSpread(G4double cs) { fChargeSpread = cs; }
        inline G4double GetChargeSpread() const { return fChargeSpread; }

    public: 
        static constexpr G4double DEFAULT_CHARGE_SPREAD = 5.0*um;

    private:
        PixHitsCollection* fHitsCollection;
        G4double fChargeSpread;
        
        // depth in volume hierarchy, not physical depth
        static constexpr G4int DEPLETION_DEPTH = 5;
};

#endif
