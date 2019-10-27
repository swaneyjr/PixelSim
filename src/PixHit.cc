#include "PixHit.hh"

G4ThreadLocal G4Allocator<PixHit>* PixHitAllocator = 0;

PixHit::PixHit()
    :   G4VHit(),
        fX(-1),
        fY(-1),
        fE(0.0)
{ }

PixHit::PixHit(const PixHit& hit)
    :   G4VHit()
{
    fX = hit.fX;
    fY = hit.fY;
    fE = hit.fE;
}

PixHit::~PixHit()
{ }

const PixHit& PixHit::operator=(const PixHit& hit)
{
    fX = hit.fX;
    fY = hit.fY;
    fE = hit.fE;

    return *this;
}

G4int PixHit::operator==(const PixHit& hit)
{
    return ( this == &hit ) ? 1: 0;
}
