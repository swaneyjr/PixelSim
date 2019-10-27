#include "PixAngDistribution.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

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
    G4double normalization = maxTheta + sin(2*maxTheta) / 2;


    G4double rand = normalization * G4UniformRand();

    // initial guess
    G4double theta = rand / (pi / 2);

    while (true) 
    { 
        // function to find root
        G4double f0 = theta + sin(2*theta) / 2 - rand;

        if (fabs(f0) < THETA_THRESH) break;
    
        // derivatives
        G4double f1 = 1 + cos(2*theta);
        G4double f2 = -2 * sin(2*theta);

        // halley's method
        theta -= 2*f0*f1 / (2*f1*f1 - f0*f2);
    } 


    G4double sinTheta = sin(theta);
    G4double cosTheta = cos(theta);
    G4double sinPhi = sin(phi);
    G4double cosPhi = cos(phi);

    momentum.setX(sinTheta*cosPhi);
    momentum.setY(sinTheta*sinPhi);
    momentum.setZ(-cosTheta);

}
