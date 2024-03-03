#include <G4RunManager.hh>
#include <G4VisExecutive.hh>
#include <G4UIExecutive.hh>
#include <G4RunManagerFactory.hh>

#include "DetectorConstruction.h"
#include "PhysicsList.h"
#include "ActionInitialization.h"

#include "CLI/CLI.hpp"

using namespace std;

int main(int argc, char **argv) {
    int nEvents = 1000;
    int nThreads = 0;

    CLI::App app{"radiation-transmission"};

    app.add_option("-n,--primaries", nEvents, "Number of primary particles to launch")->check(CLI::PositiveNumber);
    app.add_option("-t,--threads", nThreads, "Number of threads")->check(CLI::PositiveNumber);

    CLI11_PARSE(app, argc, argv);

    const auto runManagerType = nThreads > 0 ? G4RunManagerType::MTOnly : G4RunManagerType::SerialOnly;
    auto runManager = unique_ptr<G4RunManager>(G4RunManagerFactory::CreateRunManager(runManagerType));

    if (nThreads > 0) {
        runManager->SetNumberOfThreads((G4int) nThreads);
    }

    runManager->SetUserInitialization(new DetectorConstruction);
    runManager->SetUserInitialization(new PhysicsList);

    runManager->SetUserInitialization(new ActionInitialization);

    runManager->Initialize();

    G4VisExecutive visManager;
    visManager.Initialize();

    runManager->BeamOn(nEvents);

    return 0;
}
