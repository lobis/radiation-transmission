
#include "DetectorConstruction.h"
#include "SensitiveDetector.h"

#include <G4LogicalVolumeStore.hh>
#include <G4NistManager.hh>
#include <G4PhysicalVolumeStore.hh>

#include <random>
#include <G4PVPlacement.hh>

using namespace std;
using namespace CLHEP;

DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction() {}


G4VPhysicalVolume *DetectorConstruction::Construct() {
    // build a basic detector
    auto nist = G4NistManager::Instance();
    const auto vacuum = nist->FindOrBuildMaterial("G4_Galactic");
    const auto concrete = nist->FindOrBuildMaterial("G4_CONCRETE");

    const auto targetMaterial = concrete;
    constexpr double width = 300.0 * m; // infinite in x and y

    constexpr double thickness = 100.0 * cm;
    constexpr double detectorThickness = 1.0 * mm;

    auto worldSolid = new G4Box("World", width / 2, width / 2, width / 2);
    auto worldLogical = new G4LogicalVolume(worldSolid, vacuum, "World");
    world = new G4PVPlacement(nullptr, {}, worldLogical, "World", nullptr, false, 0);

    auto targetSolid = new G4Box("Target", width / 2, width / 2, thickness / 2);
    auto targetLogical = new G4LogicalVolume(targetSolid, targetMaterial, "Target");
    new G4PVPlacement(nullptr, {0, 0, thickness / 2}, targetLogical, "Target", worldLogical, false, 0);

    auto detectorSolid = new G4Box("Detector", width / 2, width / 2, detectorThickness / 2);
    auto detectorLogical = new G4LogicalVolume(detectorSolid, vacuum, "Detector");
    new G4PVPlacement(nullptr, {0, 0, thickness + detectorThickness / 2}, detectorLogical, "Detector",
                      worldLogical, false, 0);

    // check for overlaps
    if (world->CheckOverlaps(1000, 0, true)) {
        throw runtime_error("Overlaps found in geometry");
    }

    return world;
}

void DetectorConstruction::ConstructSDandField() {
    auto detectorLogical = G4LogicalVolumeStore::GetInstance()->GetVolume("Detector");
    auto detector = new SensitiveDetector("Detector");
    SetSensitiveDetector(detectorLogical, detector);
}


