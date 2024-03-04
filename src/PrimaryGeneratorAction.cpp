
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
    gun.SetParticlePosition({0.0, 0.0, 0.0});
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event *event) {
    const auto particleName = RunAction::ChooseParticle();

    G4ParticleDefinition *particle = G4ParticleTable::GetParticleTable()->FindParticle(
            RunAction::GetGeant4ParticleName(particleName));
    gun.SetParticleDefinition(particle);

    const auto [energy, zenith] = RunAction::GenerateEnergyAndZenith(particleName);

    gun.SetParticleEnergy(energy);

    double phi = G4UniformRand() * TMath::TwoPi();
    double zenithRad = zenith * TMath::DegToRad();

    const G4ThreeVector direction = {TMath::Sin(zenithRad) * TMath::Cos(phi), TMath::Sin(zenithRad) * TMath::Sin(phi),
                                     TMath::Cos(zenithRad)};

    gun.SetParticleMomentumDirection(direction);

    gun.GeneratePrimaryVertex(event);

    RunAction::IncreaseLaunchedPrimaries();
}
