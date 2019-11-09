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
        G4int* fPixElectrons;

        G4int fResXY; 
};

#endif
