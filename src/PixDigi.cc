#include "PixDigi.hh"

G4ThreadLocal G4Allocator<PixDigi>* pixDigiAllocator = 0;

PixDigi::PixDigi()
    :   G4VDigi(),
        fX(-1),
        fY(-1),
        fN(0),
        fNProton(0),
        fNElectron(0),
        fNAlpha(0),
        fNPion(0),
        fNDeuteron(0),
        fNOther(0)
{ }

PixDigi::~PixDigi()
{ }

PixDigi::PixDigi(const PixDigi& aDigi)
{
    fX = aDigi.fX;
    fY = aDigi.fY;
    fN = aDigi.fN;

    fNProton    = aDigi.fNProton;
    fNElectron  = aDigi.fNElectron;
    fNAlpha     = aDigi.fNAlpha;
    fNPion      = aDigi.fNPion;
    fNDeuteron  = aDigi.fNDeuteron;
    fNOther     = aDigi.fNOther;
}

const PixDigi& PixDigi::operator=(const PixDigi& aDigi)
{
    fX = aDigi.fX;
    fY = aDigi.fY;
    fN = aDigi.fN;

    fNProton    = aDigi.fNProton;
    fNElectron  = aDigi.fNElectron;
    fNAlpha     = aDigi.fNAlpha;
    fNPion      = aDigi.fNPion;
    fNDeuteron  = aDigi.fNDeuteron;
    fNOther     = aDigi.fNOther;

    return *this;
}

int PixDigi::operator==(const PixDigi& aDigi) const
{
    return ( fX == aDigi.fX 
            && fY == aDigi.fY 
            && fN == aDigi.fN 
            && fNProton    == aDigi.fNProton
            && fNElectron  == aDigi.fNElectron
            && fNAlpha     == aDigi.fNAlpha
            && fNPion      == aDigi.fNPion
            && fNDeuteron  == aDigi.fNDeuteron
            && fNOther     == aDigi.fNOther )
        ? 1 : 0;
}


