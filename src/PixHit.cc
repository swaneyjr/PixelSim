#include "PixHit.hh"

G4ThreadLocal G4Allocator<PixHit>* PixHitAllocator = 0;

PixHit::PixHit()
    :   G4VHit(),
        fX(-1),
        fY(-1),
        fN(0.0),
        fParticleName("")
{ }

PixHit::PixHit(const PixHit& hit)
    :   G4VHit()
{
    fX = hit.fX;
    fY = hit.fY;
    fN = hit.fN;
    fParticleName = hit.fParticleName;
}

PixHit::~PixHit()
{ }

const PixHit& PixHit::operator=(const PixHit& hit)
{
    fX = hit.fX;
    fY = hit.fY;
    fN = hit.fN;
    fParticleName = hit.fParticleName;

    return *this;
}

G4int PixHit::operator==(const PixHit& hit)
{
    return ( this == &hit ) ? 1: 0;
}
