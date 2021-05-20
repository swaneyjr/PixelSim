#include "PixAngDistribution.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include <math.h>

PixAngDistribution::PixAngDistribution()
    :   G4SPSAngDistribution(),
        fCosmic(false)
{ 
    G4SPSAngDistribution::SetAngDistType("beam1d");
}

PixAngDistribution::~PixAngDistribution()
{ }

void PixAngDistribution::SetAngDistType(G4String aType)
{
    if (aType == "cosmic")
    {
        fCosmic = true;
    }
    else
    {
        fCosmic = false;
        G4SPSAngDistribution::SetAngDistType(aType);
    }
}

G4String PixAngDistribution::GetDistType()
{
    return fCosmic ? "cosmic" : G4SPSAngDistribution::GetDistType();
}

G4ParticleMomentum PixAngDistribution::GenerateOne()
{
    if (fCosmic)
    {
        G4ParticleMomentum localM = G4ParticleMomentum();
        GenerateCosmicFlux(localM);
        return localM;
    } 
    else return G4SPSAngDistribution::GenerateOne();

}

void PixAngDistribution::GenerateCosmicFlux(G4ParticleMomentum& momentum)
{
    // uniform phi
    G4double phi = twopi*G4UniformRand()*radian;
    
    // get normalized theta pdf
    G4double maxTheta = GetMaxTheta();
    G4double normalization = pow(cos(maxTheta), 4);

    G4double rand = (1-normalization) * G4UniformRand();
    G4double theta = acos(pow(rand + normalization, 0.25));

    G4double sinTheta = sin(theta);
    G4double cosTheta = cos(theta);
    G4double sinPhi = sin(phi);
    G4double cosPhi = cos(phi);

    momentum.setX(sinTheta*cosPhi);
    momentum.setY(sinTheta*sinPhi);
    momentum.setZ(-cosTheta);

}
