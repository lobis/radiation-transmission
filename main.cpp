#include <G4RunManager.hh>
#include <G4VisExecutive.hh>
#include <G4UIExecutive.hh>
#include <G4UImanager.hh>

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

int main() {
    G4RunManager runManager;

    runManager.SetUserInitialization(new DetectorConstruction);
    runManager.SetUserInitialization(new PhysicsList);

    runManager.SetUserInitialization(new ActionInitialization);

    runManager.Initialize();

    G4VisExecutive visManager;
    visManager.Initialize();

    auto uiManager = G4UImanager::GetUIpointer();
    // insert the lines from the visualizationMacro string
    // uiManager->ApplyCommand(visualizationMacro);

    runManager.BeamOn(10000);

    return 0;
}
