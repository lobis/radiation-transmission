
#pragma once

#include <G4GDMLParser.hh>
#include <G4LogicalVolume.hh>
#include <G4LogicalVolumeStore.hh>
#include <G4NistManager.hh>
#include <G4PhysicalVolumeStore.hh>
#include <G4VUserDetectorConstruction.hh>
#include <G4VisAttributes.hh>
#include <globals.hh>


class DetectorConstruction : public G4VUserDetectorConstruction {

public:
    DetectorConstruction();

    G4VPhysicalVolume *Construct() override;

    G4VPhysicalVolume *GetWorld() const { return world; }

    bool CheckOverlaps() const;

    void ConstructSDandField() override;


private:
    G4VPhysicalVolume *world = nullptr;
};


