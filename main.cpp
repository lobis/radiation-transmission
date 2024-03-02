#include <G4RunManager.hh>
#include <G4VisExecutive.hh>
#include <G4UIExecutive.hh>
#include <G4UImanager.hh>
#include <G4RunManagerFactory.hh>

#include "DetectorConstruction.h"
#include "PhysicsList.h"
#include "ActionInitialization.h"

using namespace std;

const string visualizationMacro = R"(
/vis/open OGL 600x600-0+0
/vis/viewer/set/viewpointThetaPhi 90 0
/vis/viewer/set/lightsThetaPhi 90 0
/vis/viewer/set/style wireframe

/vis/drawVolume
/vis/viewer/flush
)";

int main(int argc, char **argv) {
    int nEvents = 1000;
    int nThreads = 0;

    if (argc > 1) {
        nEvents = stoi(argv[1]);
    }
    if (argc > 2) {
        nThreads = stoi(argv[2]);
    }

    const auto runManagerType = nThreads > 0 ? G4RunManagerType::MTOnly : G4RunManagerType::SerialOnly;
    auto runManager = unique_ptr<G4RunManager>(G4RunManagerFactory::CreateRunManager(runManagerType));

    if (nThreads > 0) {
        throw runtime_error("Multithreading is buggy atm");
        runManager->SetNumberOfThreads((G4int) nThreads);
    }

    runManager->SetUserInitialization(new DetectorConstruction);
    runManager->SetUserInitialization(new PhysicsList);

    runManager->SetUserInitialization(new ActionInitialization);

    runManager->Initialize();

    G4VisExecutive visManager;
    visManager.Initialize();

    auto uiManager = G4UImanager::GetUIpointer();
    // insert the lines from the visualizationMacro string
    // uiManager->ApplyCommand(visualizationMacro);

    runManager->BeamOn(nEvents);

    return 0;
}
