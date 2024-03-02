
#pragma once

#include <G4VSteppingVerbose.hh>
#include <G4VUserActionInitialization.hh>


class ActionInitialization : public G4VUserActionInitialization {
public:
    ActionInitialization();

    void BuildForMaster() const override;

    void Build() const override;
};
