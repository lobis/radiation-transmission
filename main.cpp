#include <G4RunManager.hh>
#include <G4VisExecutive.hh>
#include <G4UIExecutive.hh>
#include <G4RunManagerFactory.hh>

#include "DetectorConstruction.h"
#include "PhysicsList.h"
#include "ActionInitialization.h"
#include "RunAction.h"

#include "CLI/CLI.hpp"

using namespace std;

int main(int argc, char **argv) {

    int nEvents;
    int nThreads = 0;
    string inputFilename;
    string outputFilename;
    string inputParticle;
    vector<pair<string, double>> detectorConfiguration;

    CLI::App app{"radiation-transmission"};

    app.add_option("-n,--primaries", nEvents, "Number of primary particles to launch")->check(
            CLI::PositiveNumber)->required();
    app.add_option("-t,--threads", nThreads, "Number of threads")->check(CLI::PositiveNumber);
    app.add_option("-p,--particle", inputParticle, "Input particle type")->check(
            CLI::IsMember({"neutron", "gamma", "proton", "electron", "muon"}))->required();
    app.add_option("-i,--input", inputFilename,
                   "Input root filename with particle energy / angle information")->required();
    app.add_option("-o,--output", outputFilename, "Output root filename")->required();

    app.add_option("-d,--detector", detectorConfiguration,
                   "Detector configuration: material and thickness (in mm) in the following format: '-d G4_Pb 100'. If called multiple times they will be stacked")->required();

    CLI11_PARSE(app, argc, argv);

    RunAction::SetInputParticle(inputParticle);
    RunAction::SetInputFilename(inputFilename);
    RunAction::SetOutputFilename(outputFilename);

    const auto runManagerType = nThreads > 0 ? G4RunManagerType::MTOnly : G4RunManagerType::SerialOnly;
    auto runManager = unique_ptr<G4RunManager>(G4RunManagerFactory::CreateRunManager(runManagerType));

    if (nThreads > 0) {
        runManager->SetNumberOfThreads((G4int) nThreads);
    }

    runManager->SetUserInitialization(new DetectorConstruction(detectorConfiguration));
    runManager->SetUserInitialization(new PhysicsList);

    runManager->SetUserInitialization(new ActionInitialization);

    runManager->Initialize();

    G4VisExecutive visManager;
    visManager.Initialize();

    runManager->BeamOn(nEvents);

    return 0;
}
