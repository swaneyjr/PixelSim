#include "PixMonteCarlo.hh"

#include "PixDetectorConstruction.hh"

#include "G4Exception.hh"
#include "G4RandomDirection.hh"
#include "Randomize.hh"

#include <iostream>
#include <fstream>

// null result for generating xy coords
PixCoords PixCoordsNull;

PixMonteCarlo::PixMonteCarlo(PixDetectorConstruction* dc)
{

    fLimXY = dc->GetPixXY() / 2;
    fLimZ = dc->GetPixZ() - dc->GetPixDepl();
    fBackDTI = dc->GetBackDTI();
    
    fIsoDepth = fBackDTI ? std::max(fLimZ - dc->GetDTIDepth(), 0.)
        : std::max(dc->GetDTIDepth() - dc->GetPixDepl(), 0.); 
    
    fDiffLen = dc->GetDiffusionLength();

    fInterpolation = dc->GetFastMCInterpolation();

    fStepSize = dc->GetDiffStep();
    fProbRecombination = (fStepSize * fStepSize) / (fDiffLen * fDiffLen);
  
    if (fInterpolation != "Off") 
    { 
        G4String gridFile = dc->GetFastMCFile();
        if (!gridFile.empty())
            LoadGrid(gridFile);
        else
        {
            fRes = 2 * dc->GetFastMCMaxSpread() + 1;
            fMCElectrons = dc->GetFastMCSampleSize();

            G4double nominalGridSpacing = dc->GetFastMCGridSpacing();
            fSpacesXY = (G4int) (fLimXY / nominalGridSpacing) + 1; // endpoints at edge and center
            fSpacesZ = (G4int) (fLimZ / nominalGridSpacing) + 1; // endpoints at edge and depletion

            GenerateGrid();
        }
    }
    else fMCGrid = nullptr;
}


PixMonteCarlo::~PixMonteCarlo()
{ 
    if ( fMCGrid ) delete [] fMCGrid;
}


const PixCoords PixMonteCarlo::GenerateHit(G4ThreeVector x0)
{  
    //G4cout << "(" << x0.x()/um << ", " << x0.y()/um << ", " << x0.z()/um << ")" << G4endl;

    // move to a convenient coordinate system
    G4ThreeVector xi = x0 + G4ThreeVector(0, 0, fLimZ/2);
    G4ThreeVector xf;

    // fast MC
    if (fMCGrid) return FastMC(xi);

    G4int dx = 0;
    G4int dy = 0; 

    // check each loop for recombination
    while (G4UniformRand() > fProbRecombination)
    {
                
        G4ThreeVector step = fStepSize * G4RandomDirection();
        xf = xi + step;

        //G4cout << "(" << xi.x()/nm << ", " << xi.y()/nm << ", " << xi.z()/nm << ") -> ";
        //G4cout << "(" << xf.x()/nm << ", " << xf.y()/nm << ", " << xf.z()/nm << ")\n";


        // create hit if electron has moved to depletion zone
        if (xf.z() < 0) 
	{
            //G4cout << "(" << dx << ", " << dy << ")\n";
	    return PixCoords(dx, dy);
	}
        // reflection over boundaries
        else if (xf.z() > fLimZ)
        {
            G4double znew = 2*fLimZ - xf.z();
            xf.setZ(znew);
        }

        if (fabs(xf.x()) > fLimXY)
        {
            G4int sgn = copysign(1, xf.x());
            G4double xnew = xf.x() - 2*sgn*fLimXY;
            if ((xi.z() < fIsoDepth) == fBackDTI) 
            {
                dx += sgn; 
                xf.setX(xnew);   
            }
            else
                xf.setX(-1*xnew);
        }
        
        if (fabs(xf.y()) > fLimXY)
        {
            G4int sgn = copysign(1, xf.y());
            G4double ynew = xf.y() - 2*sgn*fLimXY;
            if ((xi.z() < fIsoDepth) == fBackDTI) 
            {
                dy += sgn;
                xf.setY(ynew);
            }
            else 
                xf.setY(-1*ynew);
        }

        // setup for next step
        xi = xf;

    }

    return PixCoordsNull;

}

const PixCoords PixMonteCarlo::FastMC(G4ThreeVector xi)
{
    if (fInterpolation == "Nearest")
        return InterpolateNearest(xi);
    else if (fInterpolation == "Linear")
        return InterpolateLinear(xi);
    else return PixCoordsNull;
}

const PixCoords PixMonteCarlo::InterpolateNearest(G4ThreeVector xi)
{
    G4int xSpace = (G4int) (fabs(xi.x()) / fLimXY * (fSpacesXY-1) + 0.5);
    G4int ySpace = (G4int) (fabs(xi.y()) / fLimXY * (fSpacesXY-1) + 0.5);
    G4int zSpace = (G4int) ((xi.z() + fLimZ/2) / fLimZ * (fSpacesZ-1) + 0.5);

    G4int sgnX = copysign(1, xi.x());
    G4int sgnY = copysign(1, xi.y());

    G4double rand = fMCElectrons * G4UniformRand();
    G4int* baseAddr = fMCGrid + ((xSpace*fSpacesXY + ySpace)*fSpacesZ + zSpace) * fRes*fRes;

    G4int pixMin = 0;
    G4int pixMax = fRes*fRes-1;
    G4int pixCenter;
    G4int centerCount;

    if (rand > *(baseAddr + pixMax)) return PixCoordsNull; 

    // binary search tree
    while(pixMax - pixMin > 1) {
	pixCenter = (pixMax + pixMin) / 2;
	centerCount = *(baseAddr + pixCenter);
	if (centerCount > rand) pixMax = pixCenter;
	else pixMin = pixCenter;
    }

    G4int pixX = pixMin / fRes - fRes/2;
    G4int pixY = pixMin % fRes - fRes/2;    

    return PixCoords(pixX*sgnX, pixY*sgnY);
 
}

const PixCoords PixMonteCarlo::InterpolateLinear(G4ThreeVector xi)
{
    G4double xSpaceDouble = fabs(xi.x()) / fLimXY * (fSpacesXY-1);
    G4double ySpaceDouble = fabs(xi.y()) / fLimXY * (fSpacesXY-1);
    G4double zSpaceDouble = (xi.z()+fLimZ/2) / fLimZ * (fSpacesZ-1);

    G4int sgnX = copysign(1, xi.x());
    G4int sgnY = copysign(1, xi.y());

    G4int xSpace = (G4int) xSpaceDouble;
    G4int ySpace = (G4int) ySpaceDouble;
    G4int zSpace = (G4int) zSpaceDouble;

    G4double xExtra = xSpaceDouble - xSpace;
    G4double yExtra = ySpaceDouble - ySpace;
    G4double zExtra = zSpaceDouble - zSpace;

    G4double rand = fMCElectrons * G4UniformRand();
    G4int* baseAddr = fMCGrid + ((xSpace*fSpacesXY + ySpace)*fSpacesZ + zSpace) * fRes * fRes;

    G4int pixMin = 0;
    G4int pixMax = fRes*fRes-1;
    G4int pixC;
    G4int centerCount;
    
    G4int dz = fRes*fRes;
    G4int dy = dz*fSpacesZ;
    G4int dx = dy*fSpacesXY;

    G4int count = *(baseAddr + pixMax) * xExtra*yExtra*zExtra \
		+ *(baseAddr+dx+pixMax) * (1-xExtra)*yExtra*zExtra \
		+ *(baseAddr+dy+pixMax) * xExtra*(1-yExtra)*zExtra \
		+ *(baseAddr+dz+pixMax) * xExtra*yExtra*(1-zExtra) \
		+ *(baseAddr+dx+dy+pixMax) * (1-xExtra)*(1-yExtra)*zExtra \
		+ *(baseAddr+dx+dz+pixMax) * (1-xExtra)*yExtra*(1-zExtra) \
		+ *(baseAddr+dy+dz+pixMax) * xExtra*(1-yExtra)*(1-zExtra) \
		+ *(baseAddr+dx+dy+dz+pixMax) * (1-xExtra)*(1-yExtra)*(1-zExtra);
    if (rand > count) return PixCoordsNull; 

    // binary search tree
    while(pixMax - pixMin > 1) {
	pixC = (pixMax + pixMin) / 2;
	centerCount = *(baseAddr + pixC) * xExtra*yExtra*zExtra \
		    + *(baseAddr+dx+pixC) * (1-xExtra)*yExtra*zExtra \
		    + *(baseAddr+dy+pixC) * xExtra*(1-yExtra)*zExtra \
		    + *(baseAddr+dz+pixC) * xExtra*yExtra*(1-zExtra) \
		    + *(baseAddr+dx+dy+pixC) * (1-xExtra)*(1-yExtra)*zExtra \
		    + *(baseAddr+dx+dz+pixC) * (1-xExtra)*yExtra*(1-zExtra) \
		    + *(baseAddr+dy+dz+pixC) * xExtra*(1-yExtra)*(1-zExtra) \
		    + *(baseAddr+dx+dy+dz+pixC) * (1-xExtra)*(1-yExtra)*(1-zExtra);

	if (centerCount > rand) pixMax = pixC;
	else pixMin = pixC;
    }

    G4int pixX = pixMin / fRes - fRes/2;
    G4int pixY = pixMin % fRes - fRes/2;    

    return PixCoords(pixX*sgnX, pixY*sgnY);
 

}


void PixMonteCarlo::GenerateGrid()
{
    // make this null so that GenerateHit() will do a random walk
    fMCGrid = nullptr;

    // this will be copied to the instance variable later
    G4int* grid = new G4int[fSpacesXY * fSpacesXY * fSpacesZ * fRes * fRes];
    for (G4int i=0; i<fSpacesXY * fSpacesXY * fSpacesZ * fRes * fRes; i++) {
	*(grid + i) = 0;
    }
    
    G4cout << "..........Generating grid.........." << G4endl;

    for (G4int ix=0; ix<fSpacesXY; ix++)
    {
        // only do half, by symmetry
        for (G4int iy=0; iy<=ix; iy++)
        {
            for (G4int iz=0; iz<fSpacesZ; iz++)
            {
                G4cout << (ix*(ix+1)/2 + iy)*fSpacesZ + iz + 1 << "/" << fSpacesXY*(fSpacesXY+1)/2 * fSpacesZ << "\r" << std::flush;

                G4ThreeVector xlocal = G4ThreeVector(
                            fLimXY * ix/(fSpacesXY-1), 
                            fLimXY * iy/(fSpacesXY-1), 
                            fLimZ * iz/(fSpacesZ-1) - fLimZ/2);

                for (G4int iElectron=0; iElectron<fMCElectrons; iElectron++)
                {
                    PixCoords dxy = GenerateHit(xlocal);
                    if (&dxy == &PixCoordsNull || std::max(abs(dxy.first), abs(dxy.second)) > fRes/2)
                        continue;
            
                    G4int pixX = dxy.first + fRes/2;
                    G4int pixY = dxy.second + fRes/2;

                    (*(grid + (((ix*fSpacesXY + iy) * fSpacesZ + iz) * fRes + pixX) * fRes + pixY))++;

                    if (ix == iy) continue;
                    (*(grid + (((iy*fSpacesXY + ix) * fSpacesZ + iz) * fRes + pixX) * fRes + pixY))++;
                }
            }
        }
    }

    SaveGrid(grid);
    AccumulateGrid(grid);
}


void PixMonteCarlo::SaveGrid(G4int* grid)
{
    G4String dtiType = fBackDTI ? "b" : "f";
    std::stringstream fileName;
    fileName << "diffusion/diff"
        << "_xy" << (G4int)(2*fLimXY / nm + 0.5)
        << "_z" << (G4int)(fLimZ / nm + 0.5)
        << "_" << dtiType << "dti" 
	<< "_iso" << (G4int)(fIsoDepth / nm + 0.5)
        << "_len" << (G4int)(fDiffLen / nm + 0.5)
        << ".dat";
    
    std::ofstream outfile;
    outfile.open(fileName.str(), std::ios::binary);

    if (outfile.is_open())
    {
        // metadata
        outfile.put(fBackDTI?1:0);

        outfile.write((char*) &fLimXY, 8);
        outfile.write((char*) &fLimZ, 8);
        outfile.write((char*) &fIsoDepth, 8);
        outfile.write((char*) &fDiffLen, 8);
        outfile.write((char*) &fStepSize, 8);
        
        outfile.write((char*) &fSpacesXY, 4);
        outfile.write((char*) &fSpacesZ, 4);
        outfile.write((char*) &fRes, 4);
        outfile.write((char*) &fMCElectrons, 4);

        // now the actual data
        outfile.write((char*) grid, 4*fSpacesXY*fSpacesXY*fSpacesZ*fRes*fRes);

    }
    outfile.close();
}


void PixMonteCarlo::LoadGrid(G4String& fileName) 
{
    std::ifstream infile;
    infile.open(fileName, std::ios::binary);

    if (!infile.is_open())
        G4Exception(
                "PixMonteCarlo::LoadGrid()",
                "DataError",
                FatalException,
                "Could not open file.");

    infile.seekg(0, std::ios::end);
    G4int size = infile.tellg();
    infile.seekg(0, std::ios::beg);

    char boolBuf;
    infile.get(boolBuf);
    G4bool backDTI = (boolBuf == 1);

    char* doubleBuf = new char[40];
    infile.read(doubleBuf, 40);
    G4double* metadataDouble = (G4double*) doubleBuf;
    
    char* intBuf = new char[16];
    infile.read(intBuf, 16);
    G4int* metadataInt = (G4int*) intBuf;

    G4bool validGeometry = 
        backDTI == fBackDTI
        && fLimXY == metadataDouble[0]
        && fLimZ == metadataDouble[1]
        && fIsoDepth == metadataDouble[2]
        && fDiffLen == metadataDouble[3];

    G4cout << "XY:   " << fLimXY/um << " um <-> " << metadataDouble[0]/um << " um" << G4endl;
    G4cout << "Z:    " << fLimZ/um << " um <-> " << metadataDouble[1]/um << " um" << G4endl;
    G4cout << "Iso:  " << fIsoDepth/um << " um <-> " << metadataDouble[2]/um << " um" << G4endl; 
    G4cout << "L:    " << fDiffLen/um << " um <-> " << metadataDouble[3]/um << " um" << G4endl;
    G4cout << "bDTI: " << fBackDTI << " <-> " << backDTI << G4endl;

    fSpacesXY = metadataInt[0];
    fSpacesZ = metadataInt[1];
    fRes = metadataInt[2];
    fMCElectrons = metadataInt[3];

    delete [] doubleBuf;
    delete [] intBuf;

    if (!validGeometry) 
        G4Exception(
                "PixMonteCarlo::LoadGrid()",
                "DataError",
                FatalException,
                "Diffusion histogram does not agree with detector geometry");

    G4cout << "Size: " << size << " <-> " << 4*fSpacesXY*fSpacesXY*fSpacesZ*fRes*fRes +57 << G4endl;
    if (size - 57 != 4*fSpacesXY*fSpacesXY*fSpacesZ*fRes*fRes)
        G4Exception(
                "PixMonteCarlo::LoadGrid()",
                "DataError",
                FatalException,
                "Could not interpret file.");

    char* dataBuf = new char[size-57];
    infile.read(dataBuf, size-57);
    AccumulateGrid((G4int*) dataBuf);

    infile.close();
}

void PixMonteCarlo::AccumulateGrid(G4int* grid) {
    fMCGrid = grid;
    
    for (G4int idx = 0; idx < fSpacesXY*fSpacesXY*fSpacesZ; idx++) {

    	G4int* baseAddr = fMCGrid + idx * fRes * fRes;
	
        for (G4int pixXY=1; pixXY<fRes*fRes; pixXY++) 
            *(baseAddr + pixXY) += *(baseAddr + pixXY - 1);
        
    }

}
