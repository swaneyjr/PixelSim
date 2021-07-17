#include "PixIonization.hh"

#include <CLHEP/Random/Randomize.h>

PixIonization::PixIonization(PixDetectorConstruction* dc)
{
    fIonizationEnergy = dc->GetIonizationEnergy();
    fIonizationModel = dc->GetIonizationModel();
}

PixIonization::~PixIonization()
{ }

G4int PixIonization::Ionize(G4double edep, G4double* eLeftover)
{
    if ( fIonizationModel == "Scale" ) 
    {
	G4int nElectrons = (G4int) (edep / fIonizationEnergy);
	*eLeftover = edep - nElectrons*fIonizationEnergy;
	return nElectrons;
    }
    else if ( fIonizationModel == "Binom" )
    {
	G4int nPossibleEHPairs = (G4int)(edep / SI_BAND_GAP);
	*eLeftover = edep - nPossibleEHPairs * SI_BAND_GAP;
	G4double pEHPair = SI_BAND_GAP / fIonizationEnergy;
	return (G4int) CLHEP::RandBinomial::shoot(nPossibleEHPairs, pEHPair);
    }
    else { return 0; }
}
