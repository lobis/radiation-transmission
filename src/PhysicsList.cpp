
#include "PhysicsList.h"

#include <G4DecayPhysics.hh>
#include <G4EmExtraPhysics.hh>
#include <G4EmStandardPhysics.hh>
#include <G4EmStandardPhysics_option4.hh>
#include <G4HadronElasticPhysics.hh>
#include <G4HadronPhysicsFTFP_BERT.hh>
#include <G4IonPhysics.hh>
#include <G4NeutronTrackingCut.hh>
#include <G4RadioactiveDecayPhysics.hh>
#include <G4StoppingPhysics.hh>
#include <G4VModularPhysicsList.hh>
#include <G4EmLivermorePhysics.hh>
#include <G4EmLivermorePolarizedPhysics.hh>
#include <G4EmPenelopePhysics.hh>
#include <G4HadronElasticPhysicsHP.hh>
#include <G4IonBinaryCascadePhysics.hh>
#include <G4HadronPhysicsQGSP_BIC_HP.hh>

PhysicsList::PhysicsList() : G4VModularPhysicsList() {
    SetVerboseLevel(1);

    RegisterPhysics(new G4DecayPhysics());
    RegisterPhysics(new G4RadioactiveDecayPhysics());
    RegisterPhysics(new G4EmExtraPhysics());
    RegisterPhysics(new G4EmLivermorePhysics());
    RegisterPhysics(new G4IonBinaryCascadePhysics());
    RegisterPhysics(new G4HadronPhysicsQGSP_BIC_HP());
    RegisterPhysics(new G4HadronElasticPhysicsHP());
    // RegisterPhysics(new G4StoppingPhysics());
    RegisterPhysics(new G4IonPhysics());

    // Neutron tracking cut
    RegisterPhysics(new G4NeutronTrackingCut());
}
