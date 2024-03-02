
#include "PrimaryGeneratorAction.h"
#include "RunAction.h"

#include <G4Event.hh>
#include <G4ParticleTable.hh>
#include <G4RunManager.hh>
#include <G4VUserPrimaryGeneratorAction.hh>
#include <TMath.h>

using namespace std;
using namespace CLHEP;

PrimaryGeneratorAction::PrimaryGeneratorAction() : G4VUserPrimaryGeneratorAction() {}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event *event) {
    auto runAction = dynamic_cast<RunAction *>(const_cast<G4UserRunAction *>(G4RunManager::GetRunManager()->GetUserRunAction()));
    const auto [energy, theta] = runAction->GetEnergyAndTheta();

    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition *particle = particleTable->FindParticle(runAction->GetInputParticleName());

    gun.SetParticleDefinition(particle);
    gun.SetParticleEnergy(energy * MeV);
    gun.SetParticlePosition({0.0, 0.0, 0.0});

    double phi = G4UniformRand() * TMath::TwoPi();

    gun.SetParticleMomentumDirection(
            {TMath::Sin(theta) * TMath::Cos(phi), TMath::Sin(theta) * TMath::Sin(phi), TMath::Cos(theta)});

    gun.GeneratePrimaryVertex(event);
}
