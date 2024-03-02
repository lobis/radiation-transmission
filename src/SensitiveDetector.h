
#pragma once

#include <G4VSensitiveDetector.hh>



class SensitiveDetector : public G4VSensitiveDetector {
public:
    explicit SensitiveDetector(const std::string& name);

    void Initialize(G4HCofThisEvent*) override;
    G4bool ProcessHits(G4Step*, G4TouchableHistory*) override;
};


