#ifndef PixIonization_h
#define PixIonization_h

#include "PixDetectorConstruction.hh"

class PixIonization {

    public:
	PixIonization(PixDetectorConstruction* dc);
	~PixIonization();

	G4int Ionize(G4double edep, G4double* eLeftover);

    private:
	G4double fIonizationEnergy;
	G4String fIonizationModel;

	static constexpr G4double SI_BAND_GAP = 1.12*eV;
};

#endif
