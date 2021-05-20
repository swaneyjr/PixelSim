#ifndef RootIO_h
#define RootIO_h 1

#include "PixDigi.hh"

#include "g4root.hh"
#include "G4PrimaryParticle.hh"

#include <unordered_set>

class PixAnalysisMessenger;

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
        PixAnalysisMessenger* fAnalysisMessenger;

        // pixel hits
        std::vector<G4int> fX;
        std::vector<G4int> fY;
        std::vector<G4int> fN;

        // total electron deposit by particle type
        std::vector<G4int> fNProton;
        std::vector<G4int> fNElectron;
        std::vector<G4int> fNAlpha;
        std::vector<G4int> fNPion;
        std::vector<G4int> fNDeuteron;
        std::vector<G4int> fNOther;

        std::unordered_set<G4int> fParticles;

        G4int fMinPix;

    public:
        inline void SetMinPix(G4int val) { fMinPix = val; };
        inline G4int GetMinPix() const { return fMinPix; };

};


#endif
