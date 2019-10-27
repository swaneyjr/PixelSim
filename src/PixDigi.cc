#include "PixDigi.hh"

G4ThreadLocal G4Allocator<PixDigi>* pixDigiAllocator = 0;

PixDigi::PixDigi()
    :   G4VDigi(),
        fX(-1),
        fY(-1),
        fN(0)
{ }

PixDigi::~PixDigi()
{ }

PixDigi::PixDigi(const PixDigi& aDigi)
{
    fX = aDigi.fX;
    fY = aDigi.fY;
    fN = aDigi.fN;
}

const PixDigi& PixDigi::operator=(const PixDigi& aDigi)
{
    fX = aDigi.fX;
    fY = aDigi.fY;
    fN = aDigi.fN;

    return *this;
}

int PixDigi::operator==(const PixDigi& aDigi) const
{
    return ( fX == aDigi.fX && fY == aDigi.fY && fN == aDigi.fN )
        ? 1 : 0;
}


