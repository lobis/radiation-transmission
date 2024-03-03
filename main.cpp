#include <G4RunManager.hh>
#include <G4RunManagerFactory.hh>

#include "DetectorConstruction.h"
#include "PhysicsList.h"
#include "ActionInitialization.h"
#include "RunAction.h"

#include "CLI/CLI.hpp"

#include <chrono>
#include <iostream>
#include <thread>

using namespace std;

void printProgress() {
    const auto start = chrono::steady_clock::now();
    do {
        const auto elapsed = chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count();
        cout << "Progress: " << RunAction::GetLaunchedPrimaries() << " / " << RunAction::GetRequestedPrimaries()
             << " (" << 100.0 * RunAction::GetLaunchedPrimaries() / RunAction::GetRequestedPrimaries() << "%)"
             << " Elapsed time: " << elapsed << " s" << endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    } while (RunAction::GetLaunchedPrimaries() < RunAction::GetRequestedPrimaries());
}

int main(int argc, char **argv) {
    const auto timeStart = chrono::steady_clock::now();

    int nEvents;
    int nThreads = 0;
    string inputFilename;
    string outputFilename;
    string inputParticle;
    vector<pair<string, double>> detectorConfiguration;

    CLI::App app{"radiation-transmission"};

    app.add_option("-n,--primaries", nEvents, "Number of primary particles to launch")->check(
            CLI::PositiveNumber)->required();
    app.add_option("-t,--threads", nThreads, "Number of threads. t=0 means no multithreading (default)")->check(
            CLI::NonNegativeNumber);
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

    RunAction::SetRequestedPrimaries(nEvents);

    const auto runManagerType = nThreads > 0 ? G4RunManagerType::MTOnly : G4RunManagerType::SerialOnly;
    auto runManager = unique_ptr<G4RunManager>(G4RunManagerFactory::CreateRunManager(runManagerType));

    if (nThreads > 0) {
        runManager->SetNumberOfThreads((G4int) nThreads);
    }

    runManager->SetUserInitialization(new DetectorConstruction(detectorConfiguration));
    runManager->SetUserInitialization(new PhysicsList);

    runManager->SetUserInitialization(new ActionInitialization);

    runManager->Initialize();

    std::thread t(printProgress);
    t.detach();

    runManager->BeamOn(RunAction::GetRequestedPrimaries());

    const auto elapsed = chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - timeStart).count();

    cout << "Total runtime: " << elapsed << " s" << endl;

    return 0;
}
