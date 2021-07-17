#include "PixEneDistribution.hh"
#include "Randomize.hh"
#include "G4SystemOfUnits.hh"

PixEneDistribution::PixEneDistribution()
 : G4SPSEneDistribution(),
   fDiscrete(false)
{ }

PixEneDistribution::~PixEneDistribution()
{ }

void PixEneDistribution::SetEnergyDisType(G4String aType)
{
    if (aType == "Discrete") {
        fDiscrete = true;
	fDiscreteEnergyCDF = ZeroPhysVector;
    } else {
	fDiscrete = false;
        G4SPSEneDistribution::SetEnergyDisType(aType);
    }
}

const G4String PixEneDistribution::GetEnergyDisType()
{
    if (fDiscrete) {
	return "Discrete";
    }
    return G4SPSEneDistribution::GetEnergyDisType();
}

void PixEneDistribution::DiscreteEnergyHistoFile(G4String filename)
{
    std::ifstream infile(filename, std::ios::in);
    if (!infile)
	G4Exception("PixEneDistribution::DiscreteEnergyHistoFile",
			"Event0301", FatalException,
			"Unable to open the histo ASCII file");
    fDiscreteEnergyCDF = ZeroPhysVector;
    fPhiTot = 0;
    G4double e, phi;
    while (infile >> e >> phi) { 
	fPhiTot += phi;
	fDiscreteEnergyCDF.InsertValues(e, fPhiTot);
    }
}

G4double PixEneDistribution::GenerateOne(G4ParticleDefinition* pdef)
{
    if (fDiscrete) {
	G4double emin = G4SPSEneDistribution::GetEmin();
	G4double emax = G4SPSEneDistribution::GetEmax();
	while (true)
	{
	    G4double particleEnergy = GenerateDiscreteEnergies();
	    //std::cout << particleEnergy/MeV << "MeV\n";
	    if (particleEnergy > emin && particleEnergy < emax) {
	        return particleEnergy;
	    }
	}
    }
    return G4SPSEneDistribution::GenerateOne(pdef);
}

G4double PixEneDistribution::GenerateDiscreteEnergies()
{
    G4double rndm = fPhiTot*G4UniformRand();
    // Binary search to find bin that rndm is in
    G4int nbelow = -1, middle;
    G4int nabove = fDiscreteEnergyCDF.GetVectorLength();
    while (nabove - nbelow > 1) {
	middle = (nabove + nbelow) / 2;
	if (rndm == fDiscreteEnergyCDF(size_t(middle))) break;
	if (rndm < fDiscreteEnergyCDF(size_t(middle)))
	    nabove = middle;
	else
	    nbelow = middle;
    }
    //std::cout << "rndm:" << rndm <<  ", nbelow: " << nbelow << "\n";

    return fDiscreteEnergyCDF.GetLowEdgeEnergy(size_t(nbelow+1));
}

G4double PixEneDistribution::GetProbability(G4double ene) 
{
    if (fDiscrete) {
	for (G4int bin=0; bin<fDiscreteEnergyCDF.GetVectorLength(); bin++) {
	    if (ene == fDiscreteEnergyCDF.GetLowEdgeEnergy(bin)) {
	        G4double pcdf = fDiscreteEnergyCDF(size_t(bin));
		if (bin == 0) return pcdf;
		return pcdf - fDiscreteEnergyCDF(size_t(bin-1));
	    }
	}
	return 0;
    } else {
	return G4SPSEneDistribution::GetProbability(ene);
    }
}
