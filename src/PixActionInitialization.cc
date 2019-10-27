#include "PixActionInitialization.hh"

#include "PixPrimaryGeneratorAction.hh"
#include "PixEventAction.hh"

PixActionInitialization::PixActionInitialization()
{ }

PixActionInitialization::~PixActionInitialization()
{ }

void PixActionInitialization::Build() const
{
    SetUserAction(new PixPrimaryGeneratorAction());
    SetUserAction(new PixEventAction());
}
