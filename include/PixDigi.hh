#ifndef PixDigi_h
#define PixDigi_h 1

#include "G4VDigi.hh"
#include "G4TDigiCollection.hh"
#include "G4Allocator.hh"

class PixDigi: public G4VDigi
{
    public:
        PixDigi();
        virtual ~PixDigi();

        PixDigi(const PixDigi&);
        const PixDigi& operator=(const PixDigi&);
        int operator==(const PixDigi&) const;

        inline void* operator new(size_t);
        inline void operator delete(void*);

    private:

        G4int fX;
        G4int fY;
        G4int fN;

    public:
        
        inline void SetX(G4int x) { fX = x; }
        inline void SetY(G4int y) { fY = y; }
        inline void SetN(G4int n) { fN = n; }

        inline G4int GetX() { return fX; }
        inline G4int GetY() { return fY; }
        inline G4int GetN() { return fN; }
};

typedef G4TDigiCollection<PixDigi> PixDigiCollection;

extern G4ThreadLocal G4Allocator<PixDigi>* pixDigiAllocator;

inline void* PixDigi::operator new(size_t)
{
    if (!pixDigiAllocator)
        pixDigiAllocator = new G4Allocator<PixDigi>;
    return (void*) pixDigiAllocator->MallocSingle();
}

inline void PixDigi::operator delete(void* aDigi)
{
    pixDigiAllocator->FreeSingle((PixDigi*) aDigi);
}


#endif
