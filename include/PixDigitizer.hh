#ifndef PixDigitizer_h
#define PixDigitizer_h 1

#include "PixDigi.hh"

#include "G4VDigitizerModule.hh"
#include "G4SystemOfUnits.hh"

class PixDigitizer : public G4VDigitizerModule
{
    public:
        PixDigitizer(G4String name);
        virtual ~PixDigitizer();

        void Digitize();
        void Configure(G4int resXY);

    private:
        std::vector<G4int> fPixTot;

        std::vector<G4int> fPixProton;
        std::vector<G4int> fPixElectron;
        std::vector<G4int> fPixAlpha;
        std::vector<G4int> fPixPion;
        std::vector<G4int> fPixDeuteron;
        std::vector<G4int> fPixOther;
        
        PixDigiCollection* fDigiCollection; 
};

#endif
