
#include "PrimaryGeneratorAction.h"
#include "RunAction.h"

#include <G4Event.hh>
#include <G4ParticleTable.hh>
#include <G4RunManager.hh>
#include <G4VUserPrimaryGeneratorAction.hh>
#include <TMath.h>

using namespace std;
using namespace CLHEP;

PrimaryGeneratorAction::PrimaryGeneratorAction() : G4VUserPrimaryGeneratorAction() {
    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition *particle = particleTable->FindParticle(RunAction::GetInputParticleName());

    gun.SetParticleDefinition(particle);
    gun.SetParticlePosition({0.0, 0.0, 0.0});
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event *event) {
    const auto [energy, theta] = RunAction::GetEnergyAndTheta();

    gun.SetParticleEnergy(energy * MeV);

    double phi = G4UniformRand() * TMath::TwoPi();
    double thetaRad = theta * TMath::DegToRad();

    gun.SetParticleMomentumDirection(
            {TMath::Sin(thetaRad) * TMath::Cos(phi), TMath::Sin(thetaRad) * TMath::Sin(phi), TMath::Cos(thetaRad)});

    gun.GeneratePrimaryVertex(event);

    RunAction::IncreaseLaunchedPrimaries();
}
