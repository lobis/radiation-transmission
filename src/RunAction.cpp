
#include "RunAction.h"

#include <iostream>
#include <TMath.h>

using namespace std;
using namespace CLHEP;

int RunAction::requestedPrimaries = 0;
unsigned int RunAction::launchedPrimaries = 0;

mutex RunAction::inputMutex;
mutex RunAction::outputMutex;

string RunAction::inputParticleName;
string RunAction::inputFilename;
string RunAction::outputFilename;

TFile *RunAction::inputFile = nullptr;
TFile *RunAction::outputFile = nullptr;

TH1D *RunAction::inputHistEnergy = nullptr;
TH1D *RunAction::inputHistZenith = nullptr;
TH2D *RunAction::inputHistEnergyZenith = nullptr;

TH1D *RunAction::muonsEnergy = nullptr;
TH1D *RunAction::electronsEnergy = nullptr;
TH1D *RunAction::gammasEnergy = nullptr;
TH1D *RunAction::protonsEnergy = nullptr;
TH1D *RunAction::neutronsEnergy = nullptr;

TH1D *RunAction::muonsZenith = nullptr;
TH1D *RunAction::electronsZenith = nullptr;
TH1D *RunAction::gammasZenith = nullptr;
TH1D *RunAction::protonsZenith = nullptr;
TH1D *RunAction::neutronsZenith = nullptr;

TH2D *RunAction::muonsEnergyZenith = nullptr;
TH2D *RunAction::electronsEnergyZenith = nullptr;
TH2D *RunAction::gammasEnergyZenith = nullptr;
TH2D *RunAction::protonsEnergyZenith = nullptr;
TH2D *RunAction::neutronsEnergyZenith = nullptr;

RunAction::RunAction() : G4UserRunAction() {}

void RunAction::BeginOfRunAction(const G4Run *) {
    lock_guard<std::mutex> lock(mutex);

    if (IsMaster()) {
        inputFile = TFile::Open(inputFilename.c_str(), "READ");

        inputHistEnergy = inputFile->Get<TH1D>(string(inputParticleName + "_energy").c_str());
        inputHistZenith = inputFile->Get<TH1D>(string(inputParticleName + "_zenith").c_str());
        inputHistEnergyZenith = inputFile->Get<TH2D>(string(inputParticleName + "_energy_zenith").c_str());

        // rename to avoid name conflicts
        inputHistEnergy->SetName("inputEnergy");
        inputHistZenith->SetName("inputZenith");
        inputHistEnergyZenith->SetName("inputEnergyZenith");

        outputFile = TFile::Open(outputFilename.c_str(), "RECREATE");

        const unsigned int binsEnergyN = 200;
        const double binsEnergyMin = 1E-9;
        const double binsEnergyMax = 1E8;
        double binsEnergy[binsEnergyN + 1];
        for (int i = 0; i <= binsEnergyN; ++i) {
            binsEnergy[i] = TMath::Power(10, (TMath::Log10(binsEnergyMin) +
                                              i * (TMath::Log10(binsEnergyMax) - TMath::Log10(binsEnergyMin)) /
                                              binsEnergyN));
        }
        const unsigned int binsZenithN = 200;
        const double binsZenithMin = 0;
        const double binsZenithMax = 90;

        muonsEnergy = new TH1D("muon_energy", "Muon Kinetic Energy (MeV)", binsEnergyN, binsEnergy);
        electronsEnergy = new TH1D("electron_energy", "Electron Kinetic Energy (MeV)", binsEnergyN, binsEnergy);
        gammasEnergy = new TH1D("gamma_energy", "Gamma Kinetic Energy (MeV)", binsEnergyN, binsEnergy);
        protonsEnergy = new TH1D("proton_energy", "Proton Kinetic Energy (MeV)", binsEnergyN, binsEnergy);
        neutronsEnergy = new TH1D("neutron_energy", "Neutron Kinetic Energy (MeV)", binsEnergyN, binsEnergy);

        muonsZenith = new TH1D("muon_zenith", "Muon Zenith Angle (degrees)", binsZenithN, binsZenithMin, binsZenithMax);
        electronsZenith = new TH1D("electron_zenith", "Electron Zenith Angle (degrees)", binsZenithN, binsZenithMin,
                                   binsZenithMax);
        gammasZenith = new TH1D("gamma_zenith", "Gamma Zenith Angle (degrees)", binsZenithN, binsZenithMin,
                                binsZenithMax);
        protonsZenith = new TH1D("proton_zenith", "Proton Zenith Angle (degrees)", binsZenithN, binsZenithMin,
                                 binsZenithMax);
        neutronsZenith = new TH1D("neutron_zenith", "Neutron Zenith Angle (degrees)", binsZenithN, binsZenithMin,
                                  binsZenithMax);

        muonsEnergyZenith = new TH2D("muon_energy_zenith", "Muon Kinetic Energy (MeV) vs Zenith Angle (degrees)",
                                     binsEnergyN,
                                     binsEnergy,
                                     binsZenithN, binsZenithMin, binsZenithMax);
        electronsEnergyZenith = new TH2D("electron_energy_zenith",
                                         "Electron Kinetic Energy (MeV) vs Zenith Angle (degrees)",
                                         binsEnergyN,
                                         binsEnergy, binsZenithN, binsZenithMin, binsZenithMax);
        gammasEnergyZenith = new TH2D("gamma_energy_zenith", "Gamma Kinetic Energy (MeV) vs Zenith Angle (degrees)",
                                      binsEnergyN,
                                      binsEnergy,
                                      binsZenithN, binsZenithMin, binsZenithMax);
        protonsEnergyZenith = new TH2D("proton_energy_zenith", "Proton Kinetic Energy (MeV) vs Zenith Angle (degrees)",
                                       binsEnergyN,
                                       binsEnergy, binsZenithN, binsZenithMin, binsZenithMax);
        neutronsEnergyZenith = new TH2D("neutron_energy_zenith",
                                        "Neutron Kinetic Energy (MeV) vs Zenith Angle (degrees)", binsEnergyN,
                                        binsEnergy, binsZenithN, binsZenithMin, binsZenithMax);
    }
}

void RunAction::EndOfRunAction(const G4Run *) {
    lock_guard<std::mutex> lockInput(inputMutex);
    lock_guard<std::mutex> lockOutput(outputMutex);

    if (isMaster) {
        if (inputHistEnergy) {
            inputHistEnergy->Write("inputEnergy");
        }
        if (inputHistZenith) {
            inputHistZenith->Write("inputZenith");
        }
        if (inputHistEnergyZenith) {
            inputHistEnergyZenith->Write("inputEnergyZenith");
        }
        inputFile->Close();

        outputFile->Write();
        outputFile->Close();
    }
}

void RunAction::InsertTrack(const G4Track *track) {
    lock_guard<std::mutex> lock(outputMutex);

    auto *particle = const_cast<G4ParticleDefinition *>(track->GetParticleDefinition());
    G4String particleName = particle->GetParticleName();
    // Energy in MeV
    G4double kineticEnergy = track->GetKineticEnergy() / MeV;
    G4double zenith =
            TMath::ACos(track->GetMomentumDirection().z()) * TMath::RadToDeg();

    if (particleName == "mu-") {
        muonsEnergy->Fill(kineticEnergy);
        muonsZenith->Fill(zenith);
        muonsEnergyZenith->Fill(kineticEnergy, zenith);
    } else if (particleName == "e-") {
        electronsEnergy->Fill(kineticEnergy);
        electronsZenith->Fill(zenith);
        electronsEnergyZenith->Fill(kineticEnergy, zenith);
    } else if (particleName == "gamma") {
        gammasEnergy->Fill(kineticEnergy);
        gammasZenith->Fill(zenith);
        gammasEnergyZenith->Fill(kineticEnergy, zenith);
    } else if (particleName == "proton") {
        protonsEnergy->Fill(kineticEnergy);
        protonsZenith->Fill(zenith);
        protonsEnergyZenith->Fill(kineticEnergy, zenith);
    } else if (particleName == "neutron") {
        neutronsEnergy->Fill(kineticEnergy);
        neutronsZenith->Fill(zenith);
        neutronsEnergyZenith->Fill(kineticEnergy, zenith);
    }
}

std::pair<double, double> RunAction::GetEnergyAndZenith() {
    lock_guard<std::mutex> lock(inputMutex);

    if (!inputHistEnergy || !inputHistZenith || !inputHistEnergyZenith) {
        throw runtime_error("RunAction::GetEnergyAndZenith: input histograms not set");
    }
    double energy, zenith;
    inputHistEnergyZenith->GetRandom2(energy, zenith);
    // energy = inputHistEnergy->GetRandom();
    // zenith = inputHistZenith->GetRandom();

    return {energy * MeV, zenith};
}

std::string RunAction::GetInputParticleName() {
    if (inputParticleName == "neutron") {
        return "neutron";
    } else if (inputParticleName == "proton") {
        return "proton";
    } else if (inputParticleName == "muon") {
        return "mu-";
    } else if (inputParticleName == "electron") {
        return "e-";
    } else if (inputParticleName == "gamma") {
        return "gamma";
    } else {
        throw runtime_error("RunAction::GetInputParticleName: unknown input particle name: " + inputParticleName);
    }
}

void RunAction::SetInputParticle(const string &name) {
    inputParticleName = name;
}

void RunAction::SetInputFilename(const string &name) {
    inputFilename = name;
}

void RunAction::SetOutputFilename(const string &name) {
    outputFilename = name;
}

void RunAction::SetRequestedPrimaries(int newValue) {
    RunAction::requestedPrimaries = newValue;
}

int RunAction::GetRequestedPrimaries() {
    return RunAction::requestedPrimaries;
}

void RunAction::IncreaseLaunchedPrimaries() {
    RunAction::launchedPrimaries++;
}

unsigned int RunAction::GetLaunchedPrimaries() {
    lock_guard<std::mutex> lock(inputMutex);
    return RunAction::launchedPrimaries;
}