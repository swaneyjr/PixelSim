#ifndef PixEventAction_h
#define PixEventAction_h 1

#include "G4UserEventAction.hh"
#include "G4Event.hh"

class PixDigitizer;

class PixEventAction : public G4UserEventAction
{
    public:
        PixEventAction();
        virtual ~PixEventAction();

        virtual void BeginOfEventAction(const G4Event* event);
        virtual void EndOfEventAction(const G4Event* event);

    private:
        PixDigitizer* fDigitizer;
};

#endif
