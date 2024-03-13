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
#include <filesystem>

using namespace std;

void printProgress() {
    const auto start = chrono::steady_clock::now();
    // lambda to check if the condition has been met (RunAction::GetLaunchedPrimaries() < RunAction::GetRequestedPrimaries()) or (RunAction::GetSecondariesCount() < RunAction::GetRequestedSecondaries())

    auto checkCondition = []() {
        if (RunAction::GetRequestedPrimaries() > 0) {
            return RunAction::GetLaunchedPrimaries() < RunAction::GetRequestedPrimaries();
        } else {
            return RunAction::GetSecondariesCount() < RunAction::GetRequestedSecondaries();
        }
    };

    cout << "Requested primaries: " << RunAction::GetRequestedPrimaries() << endl;
    cout << "Requested secondaries: " << RunAction::GetRequestedSecondaries() << endl;

    do {
        const auto elapsed = chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count();
        if (RunAction::GetRequestedPrimaries() > 0) {
            const auto count = RunAction::GetLaunchedPrimaries();
            cout << "Progress (primaries): " << count << " / " << RunAction::GetRequestedPrimaries()
                 << " (" << 100.0 * double(count) / RunAction::GetRequestedPrimaries() << "%)"
                 << " Elapsed time: " << elapsed << " s" << endl;
        } else {
            const auto count = RunAction::GetSecondariesCount();
            cout << "Progress (secondaries): " << count << " / " << RunAction::GetRequestedSecondaries()
                 << " (" << 100.0 * double(count) / RunAction::GetRequestedSecondaries() << "%)"
                 << " Elapsed time: " << elapsed << " s" << endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    } while (checkCondition());
}

int main(int argc, char **argv) {
    const auto timeStart = chrono::steady_clock::now();

    int nEvents = 0;
    int nSecondariesLimit = 0;
    int nThreads = 0;
    string inputFilename;
    string outputFilename;
    set<string> inputParticleNames = RunAction::GetInputParticlesAllowed();
    vector<pair<string, double>> detectorConfiguration;

    CLI::App app{"radiation-transmission"};

    app.add_option("-n,--primaries", nEvents, "Number of primary particles to launch")->check(
            CLI::PositiveNumber);
    app.add_option("-s,--secondaries", nSecondariesLimit, "Number of secondaries to limit the simulation to")->check(
            CLI::PositiveNumber);
    app.add_option("-t,--threads", nThreads, "Number of threads. t=0 means no multithreading (default)")->check(
            CLI::NonNegativeNumber);
    app.add_option("-p,--particle", inputParticleNames, "Input particle type")->check(
            CLI::IsMember(RunAction::GetInputParticlesAllowed()));
    app.add_option("-i,--input", inputFilename,
                   "Input root filename with particle energy / angle information")->required();
    app.add_option("-o,--output", outputFilename, "Output root filename")->required();
    app.add_option("-d,--detector", detectorConfiguration,
                   "Detector configuration: material and thickness (in mm) in the following format: '-d G4_Pb 100'. If called multiple times they will be stacked")->required();

    // primaries or secondaries must be defined, but not both

    CLI11_PARSE(app, argc, argv)

    if ((nEvents == 0 && nSecondariesLimit == 0) || (nEvents > 0 && nSecondariesLimit > 0)) {
        throw runtime_error("Either primaries or secondaries must be defined, but not both");
    }

    RunAction::SetInputParticles(inputParticleNames);
    RunAction::SetInputFilename(inputFilename);
    RunAction::SetOutputFilename(outputFilename);

    RunAction::SetRequestedPrimaries(nEvents);
    RunAction::SetRequestedSecondaries(nSecondariesLimit);

    if (!filesystem::exists(inputFilename)) {
        cerr << "Input file " << inputFilename << " does not exist" << endl;
        return 1;
    }

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

    cout << "nEvents: " << nEvents << endl;
    if (nEvents > 0) {
        runManager->BeamOn(nEvents);
    } else {
        runManager->BeamOn(numeric_limits<int>::max());
    }

    const auto elapsed = chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - timeStart).count();

    cout << "Total runtime: " << elapsed << " s" << endl;

    return 0;
}
