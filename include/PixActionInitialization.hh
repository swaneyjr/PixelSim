#ifndef PixActionInitialization_h
#define PixActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

class PixActionInitialization: public G4VUserActionInitialization
{
    public:
        PixActionInitialization();
        virtual ~PixActionInitialization();

        virtual void Build() const;
};

#endif
