#ifndef RootIO_h
#define RootIO_h 1

#include "PixDigi.hh"

#include "g4root.hh"
#include "G4PrimaryParticle.hh"

class RootIO
{
    public:
        virtual ~RootIO();

        static RootIO* GetInstance();

        void BeginFile(G4String& filename);
        void AddDigits(PixDigiCollection* dc);
        void AddPrimary(G4PrimaryParticle* primary);
        void WriteEvent();
        void Close();


    protected:
        RootIO();

    private:
        G4AnalysisManager* fAnalysisManager;

        std::vector<G4int> fX;
        std::vector<G4int> fY;
        std::vector<G4int> fN;
};


#endif
