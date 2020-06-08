#ifndef PixHit_h
#define PixHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"

class PixHit : public G4VHit
{
    public:
        PixHit();
        PixHit(const PixHit&);
        virtual ~PixHit();

        const PixHit& operator=(const PixHit&);
        G4int operator==(const PixHit&);

        inline void* operator new(size_t);
        inline void operator delete(void*);

        void SetX(G4int val) { fX = val; };
        G4int GetX() const { return fX; };

        void SetY(G4int val) { fY = val; };
        G4int GetY() const { return fY; };

        void SetN(G4int val) { fN = val; };
        G4int GetN() const { return fN; };

        void SetParticleName(G4String pname) { fParticleName = pname; }
        G4String GetParticleName() const { return fParticleName; }

    private:
        G4int fX;
        G4int fY;
        G4int fN;
        G4String fParticleName;
};

typedef G4THitsCollection<PixHit> PixHitsCollection;

extern G4ThreadLocal G4Allocator<PixHit>* PixHitAllocator;

inline void* PixHit::operator new(size_t)
{
    if (!PixHitAllocator)
        PixHitAllocator = new G4Allocator<PixHit>;
    return (void*) PixHitAllocator->MallocSingle();
}

inline void PixHit::operator delete(void* hit)
{
    PixHitAllocator->FreeSingle((PixHit*) hit);
}

#endif
