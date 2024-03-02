
#pragma once

#include <G4GeneralParticleSource.hh>
#include <G4ParticleGun.hh>
#include <G4VUserPrimaryGeneratorAction.hh>

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
    PrimaryGeneratorAction();

    void GeneratePrimaries(G4Event *) override;


private:
    G4ParticleGun gun;
};


