
#include "SensitiveDetector.h"

using namespace std;


SensitiveDetector::SensitiveDetector(const string &name) : G4VSensitiveDetector(name) {}

G4bool SensitiveDetector::ProcessHits(G4Step *step, G4TouchableHistory *) {

    auto track = step->GetTrack();

    // print step info
    G4StepPoint *preStepPoint = step->GetPreStepPoint();
    auto particle = const_cast<G4ParticleDefinition *>(track->GetParticleDefinition());
    G4String particleName = particle->GetParticleName();
    G4String volumeName = preStepPoint->GetPhysicalVolume()->GetName();
    G4double stepLength = step->GetStepLength();
    G4double energyDeposit = step->GetTotalEnergyDeposit();
    G4double globalTime = track->GetGlobalTime();
    G4double kineticEnergy = track->GetKineticEnergy();
    G4ThreeVector position = preStepPoint->GetPosition();
    G4ThreeVector momentum = track->GetMomentumDirection();
    cout << "SensitiveDetector::ProcessHits: "
         << "particle=" << particleName << " "
         << "volume=" << volumeName << " "
         << "stepLength=" << stepLength << " "
         << "energyDeposit=" << energyDeposit << " "
         << "globalTime=" << globalTime << " "
         << "kineticEnergy=" << kineticEnergy << " "
         << "position=" << position << " "
         << "momentum=" << momentum << endl;


    track->SetTrackStatus(fStopAndKill);

    return true;
}

void SensitiveDetector::Initialize(G4HCofThisEvent *) {}
