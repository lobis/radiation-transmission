
#include "DetectorConstruction.h"
#include "SensitiveDetector.h"

#include <G4LogicalVolumeStore.hh>
#include <G4NistManager.hh>
#include <G4PhysicalVolumeStore.hh>

#include <random>
#include <G4PVPlacement.hh>

using namespace std;
using namespace CLHEP;

DetectorConstruction::DetectorConstruction(const std::vector<std::pair<std::string, double>> &configuration)
        : G4VUserDetectorConstruction(), configuration(configuration) {}


G4VPhysicalVolume *DetectorConstruction::Construct() {
    // build a basic detector
    auto nist = G4NistManager::Instance();
    const auto vacuum = nist->FindOrBuildMaterial("G4_Galactic");

    // check all the materials are valid
    for (const auto &[material, thickness]: configuration) {
        if (nist->FindOrBuildMaterial(material) == nullptr) {
            throw runtime_error("Material " + material + " not found");
        }
        if (thickness < 0) {
            throw runtime_error("Thickness cannot be negative");
        }
    }

    constexpr double width = 100.0 * km; // infinite in x and y

    constexpr double detectorThickness = 1.0 * nm;

    auto worldSolid = new G4Box("World", width / 2, width / 2, width / 2);
    auto worldLogical = new G4LogicalVolume(worldSolid, vacuum, "World");
    world = new G4PVPlacement(nullptr, {}, worldLogical, "World", nullptr, false, 0);

    double totalThickness = 0;
    for (int i = 0; i < configuration.size(); i++) {
        const auto &config = configuration[i];
        const auto material = nist->FindOrBuildMaterial(config.first);
        const auto thickness = config.second * mm;
        cout << "Layer " << i << " is " << thickness << " mm of " << material->GetName() << endl;
        if (thickness == 0) {
            cout << "Warning: Layer " << i << " has zero thickness, skipping it" << endl;
            continue;
        }
        auto solid = new G4Box("Layer" + to_string(i), width / 2, width / 2, thickness / 2);
        auto logical = new G4LogicalVolume(solid, material, "Layer" + to_string(i));
        new G4PVPlacement(nullptr, {0, 0, totalThickness + thickness / 2}, logical, "Layer" + to_string(i),
                          worldLogical, false, 0);
        totalThickness += thickness;
    }

    auto detectorSolid = new G4Box("Detector", width / 2, width / 2, detectorThickness / 2);
    auto detectorLogical = new G4LogicalVolume(detectorSolid, vacuum, "Detector");
    new G4PVPlacement(nullptr, {0, 0, totalThickness + detectorThickness / 2}, detectorLogical, "Detector",
                      worldLogical, false, 0);

    // check for overlaps (not sure if this actually works)
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


