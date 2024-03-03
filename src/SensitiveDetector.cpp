
#include <G4RunManager.hh>

#include "SensitiveDetector.h"
#include "RunAction.h"

using namespace std;


SensitiveDetector::SensitiveDetector(const string &name) : G4VSensitiveDetector(name) {}

G4bool SensitiveDetector::ProcessHits(G4Step *step, G4TouchableHistory *) {

    auto track = step->GetTrack();

    auto runAction = dynamic_cast<RunAction *>(const_cast<G4UserRunAction *>(G4RunManager::GetRunManager()->GetUserRunAction()));
    runAction->InsertTrack(track);

    track->SetTrackStatus(fStopAndKill);

    return true;
}

void SensitiveDetector::Initialize(G4HCofThisEvent *) {}
