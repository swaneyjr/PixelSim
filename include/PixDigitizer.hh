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

    private:
        PixDigiCollection* fDigiCollection;
        G4double* fPixEnergies;

        G4int fResXY;
        
        const G4double fBandGap = 1.1*eV;
};

#endif
