
#include "PrimaryGeneratorAction.h"
#include "RunAction.h"

#include <G4Event.hh>
#include <G4ParticleTable.hh>
#include <G4RunManager.hh>
#include <G4VUserPrimaryGeneratorAction.hh>

using namespace std;
using namespace CLHEP;

PrimaryGeneratorAction::PrimaryGeneratorAction() : G4VUserPrimaryGeneratorAction() {}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event *event) {
    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition *particle = particleTable->FindParticle("neutron");

    gun.SetParticleDefinition(particle);
    gun.SetParticleEnergy(1.0 * GeV);
    gun.SetParticlePosition({0.0, 0.0, -10 * cm});
    gun.SetParticleMomentumDirection({0.0, 0.0, 1.0});

    gun.GeneratePrimaryVertex(event);
}
