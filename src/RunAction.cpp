
#include "RunAction.h"

#include <iostream>

using namespace std;


RunAction::RunAction() : G4UserRunAction() {}

void RunAction::BeginOfRunAction(const G4Run *) {
    lock_guard<std::mutex> lock(mutex);

    if (IsMaster()) {
        outputFile = TFile::Open("output.root", "RECREATE");
    }

    cout << "RunAction::BeginOfRunAction" << endl;
}

void RunAction::EndOfRunAction(const G4Run *) {
    lock_guard<std::mutex> lock(mutex);

    if (!isMaster || !G4Threading::IsMultithreadedApplication()) {
        //
    }

    if (isMaster) {
        outputFile->Write();
        outputFile->Close();
    }

    cout << "RunAction::EndOfRunAction" << endl;
}
