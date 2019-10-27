#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "PixDetectorConstruction.hh"
#include "PixSpectrumMessenger.hh"
#include "RootIO.hh"
#include "QGSP_BERT.hh"
#include "PixActionInitialization.hh"

#include "G4SystemOfUnits.hh"
#include "globals.hh"


void printHelp()
{
    G4cout << "Usage: pixelSim [MACFILE [OUTFILE] --alias_1 VALUE_1 ... --alias_n VALUE_N] --help\n\n";
    G4cout << "NO ARGS:     Run in interactive mode\n";
    G4cout << "MACFILE:     Execute macro in batch mode\n";
    G4cout << "OUTFILE:     ROOT output filename\n";
    G4cout << "--alias:     Substitute {alias_1} for VALUE_1, etc. in MACFILE\n";
    G4cout << "--help:      Show this message\n";
}

int main(int argc, char** argv) 
{
 
    G4UImanager* UImanager = G4UImanager::GetUIpointer();
     
    PixSpectrumMessenger* specMessenger = new PixSpectrumMessenger;
 
    // initialize run
    G4RunManager* runManager = new G4RunManager;

    runManager->SetUserInitialization(new PixDetectorConstruction);
    runManager->SetUserInitialization(new QGSP_BERT);
    runManager->SetUserInitialization(new PixActionInitialization);

    RootIO* root = RootIO::GetInstance();

    // Detect interactive mode (if no arguments) and define UI session
    if(argc == 1) 
    {
        // no args: interactive mode
        //
        // first set pre_init values
        UImanager->ExecuteMacroFile("macros/interactive/cfg.mac");


        G4String fname = "pix_interactive.root";
        root->BeginFile(fname);

        G4UIExecutive* ui = new G4UIExecutive(argc, argv);
        G4VisManager* visManager = new G4VisExecutive; 
        
        visManager->Initialize();
        UImanager->ExecuteMacroFile("macros/interactive/init.mac");

        ui->SessionStart();

        delete ui;
        delete visManager;
    } 
    else 
    {
        // batch mode
        
        // parse flags and args
        G4String prefix = "--";
        G4String macFile;
        G4String outFile;
        for (G4int argi=1; argi < argc; argi++) 
        {
            G4String arg = G4String(argv[argi]);    
            if (!arg.compare(0, prefix.length(), prefix)) 
            {
                
                if (arg == "--help") 
                {
                    printHelp();
                    return 0;
                }

                // set flag args as aliases
                G4String var = arg.substr(prefix.length());
	            G4String val = G4String(argv[++argi]);
	            UImanager->SetAlias((var+" "+val).c_str());
            }
            else if ( macFile.empty() )
                macFile = arg;
            else if ( outFile.empty() )
                outFile = arg;
            else
            {
                G4cout << "Too many arguments. " << macFile << "\n\n";
                printHelp();
                return 0;
            }
            
        }

        // handle missing args
        if ( macFile.empty() ) 
        {
            G4cout << "Invalid syntax.\n\n";
            printHelp();
            return 0;
        }
        if ( outFile.empty() ) 
            outFile = "pix_batch_default.root";

        root->BeginFile(outFile);

        UImanager->ExecuteMacroFile(macFile); 
    }

    root->Close();
 
    delete specMessenger;
    delete runManager;

    return 0;
}
