
#include <G4RunManager.hh>

#include "SensitiveDetector.h"
#include "RunAction.h"

using namespace std;


SensitiveDetector::SensitiveDetector(const string &name) : G4VSensitiveDetector(name) {}

G4bool SensitiveDetector::ProcessHits(G4Step *step, G4TouchableHistory *) {

    auto track = step->GetTrack();

    RunAction::InsertTrack(track);

    track->SetTrackStatus(fStopAndKill);

    return true;
}

void SensitiveDetector::Initialize(G4HCofThisEvent *) {}
