
#include "SteppingAction.h"

#include "RunAction.h"

#include <G4Step.hh>

using namespace std;


SteppingAction::SteppingAction() : G4UserSteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step *step) {
    return;
    // print step info
    G4StepPoint *preStepPoint = step->GetPreStepPoint();
    G4StepPoint *postStepPoint = step->GetPostStepPoint();
    G4Track *track = step->GetTrack();
    G4ParticleDefinition *particle = const_cast<G4ParticleDefinition *>(track->GetParticleDefinition());
    G4String particleName = particle->GetParticleName();
    G4String volumeName = preStepPoint->GetPhysicalVolume()->GetName();
    G4double stepLength = step->GetStepLength();
    G4double energyDeposit = step->GetTotalEnergyDeposit();
    G4double globalTime = track->GetGlobalTime();
    G4double kineticEnergy = track->GetKineticEnergy();
    G4ThreeVector position = preStepPoint->GetPosition();
    G4ThreeVector momentum = track->GetMomentumDirection();
    cout << "SteppingAction::UserSteppingAction: "
         << "particle=" << particleName << " "
         << "volume=" << volumeName << " "
         << "stepLength=" << stepLength << " "
         << "energyDeposit=" << energyDeposit << " "
         << "globalTime=" << globalTime << " "
         << "kineticEnergy=" << kineticEnergy << " "
         << "position=" << position << " "
         << "momentum=" << momentum << endl;
}
