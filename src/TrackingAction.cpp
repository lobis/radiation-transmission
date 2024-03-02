
#include "TrackingAction.h"
#include "RunAction.h"

#include <G4ParticleDefinition.hh>
#include <G4SystemOfUnits.hh>
#include <G4Track.hh>
#include <G4UnitsTable.hh>
#include <iostream>

using namespace std;
using namespace CLHEP;

TrackingAction::TrackingAction() : G4UserTrackingAction() {}

void TrackingAction::PreUserTrackingAction(const G4Track *track) {
    return;
    // print track info
    G4ParticleDefinition *particle = const_cast<G4ParticleDefinition *>(track->GetParticleDefinition());
    G4String particleName = particle->GetParticleName();
    G4String volumeName = track->GetVolume()->GetName();
    G4double stepLength = track->GetStepLength();
    G4double energyDeposit = track->GetTotalEnergy();
    G4double globalTime = track->GetGlobalTime();
    G4double kineticEnergy = track->GetKineticEnergy();
    // position in mm
    G4ThreeVector position = track->GetPosition() / mm;
    G4ThreeVector momentum = track->GetMomentumDirection();

    cout << "TrackingAction::PreUserTrackingAction: "
         << "particle=" << particleName << " "
         << "volume=" << volumeName << " "
         << "stepLength=" << G4BestUnit(stepLength, "Length") << " "
         << "energyDeposit=" << G4BestUnit(energyDeposit, "Energy") << " "
         << "globalTime=" << G4BestUnit(globalTime, "Time") << " "
         << "kineticEnergy=" << G4BestUnit(kineticEnergy, "Energy") << " "
         << "position=" << position << " "
         << "momentum=" << momentum << endl;
}

void TrackingAction::PostUserTrackingAction(const G4Track *track) {}
