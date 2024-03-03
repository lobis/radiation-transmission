
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
TH1D *RunAction::inputHistTheta = nullptr;
TH2D *RunAction::inputHistEnergyTheta = nullptr;

TH1D *RunAction::muonsKe = nullptr;
TH1D *RunAction::electronsKe = nullptr;
TH1D *RunAction::gammasKe = nullptr;
TH1D *RunAction::protonsKe = nullptr;
TH1D *RunAction::neutronsKe = nullptr;

TH1D *RunAction::muonsTheta = nullptr;
TH1D *RunAction::electronsTheta = nullptr;
TH1D *RunAction::gammasTheta = nullptr;
TH1D *RunAction::protonsTheta = nullptr;
TH1D *RunAction::neutronsTheta = nullptr;

TH2D *RunAction::muonsKeTheta = nullptr;
TH2D *RunAction::electronsKeTheta = nullptr;
TH2D *RunAction::gammasKeTheta = nullptr;
TH2D *RunAction::protonsKeTheta = nullptr;
TH2D *RunAction::neutronsKeTheta = nullptr;

RunAction::RunAction() : G4UserRunAction() {}

void RunAction::BeginOfRunAction(const G4Run *) {
    lock_guard<std::mutex> lock(mutex);

    if (IsMaster()) {
        inputFile = TFile::Open(inputFilename.c_str(), "READ");

        inputHistEnergy = inputFile->Get<TH1D>(string(inputParticleName + "_energy").c_str());
        inputHistTheta = inputFile->Get<TH1D>(string(inputParticleName + "_theta").c_str());
        inputHistEnergyTheta = inputFile->Get<TH2D>(string(inputParticleName + "_energy_theta").c_str());

        // rename to avoid name conflicts
        inputHistEnergy->SetName("inputKe");
        inputHistTheta->SetName("inputTheta");
        inputHistEnergyTheta->SetName("inputKeTheta");

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
        const unsigned int binsThetaN = 200;
        const double binsThetaMin = 0;
        const double binsThetaMax = 90;

        muonsKe = new TH1D("muon_energy", "Muon Kinetic Energy (MeV)", binsEnergyN, binsEnergy);
        electronsKe = new TH1D("electron_energy", "Electron Kinetic Energy (MeV)", binsEnergyN, binsEnergy);
        gammasKe = new TH1D("gamma_energy", "Gamma Kinetic Energy (MeV)", binsEnergyN, binsEnergy);
        protonsKe = new TH1D("proton_energy", "Proton Kinetic Energy (MeV)", binsEnergyN, binsEnergy);
        neutronsKe = new TH1D("neutron_energy", "Neutron Kinetic Energy (MeV)", binsEnergyN, binsEnergy);

        muonsTheta = new TH1D("muon_theta", "Muon Theta (degrees)", binsThetaN, binsThetaMin, binsThetaMax);
        electronsTheta = new TH1D("electron_theta", "Electron Theta (degrees)", binsThetaN, binsThetaMin,
                                  binsThetaMax);
        gammasTheta = new TH1D("gamma_theta", "Gamma Theta (degrees)", binsThetaN, binsThetaMin, binsThetaMax);
        protonsTheta = new TH1D("proton_theta", "Proton Theta (degrees)", binsThetaN, binsThetaMin, binsThetaMax);
        neutronsTheta = new TH1D("neutron_theta", "Neutron Theta (degrees)", binsThetaN, binsThetaMin, binsThetaMax);

        muonsKeTheta = new TH2D("muon_energy_theta", "Muon Kinetic Energy (MeV) vs Theta (degrees)", binsEnergyN,
                                binsEnergy,
                                binsThetaN, binsThetaMin, binsThetaMax);
        electronsKeTheta = new TH2D("electron_energy_theta", "Electron Kinetic Energy (MeV) vs Theta (degrees)",
                                    binsEnergyN,
                                    binsEnergy, binsThetaN, binsThetaMin, binsThetaMax);
        gammasKeTheta = new TH2D("gamma_energy_theta", "Gamma Kinetic Energy (MeV) vs Theta (degrees)", binsEnergyN,
                                 binsEnergy,
                                 binsThetaN, binsThetaMin, binsThetaMax);
        protonsKeTheta = new TH2D("proton_energy_theta", "Proton Kinetic Energy (MeV) vs Theta (degrees)", binsEnergyN,
                                  binsEnergy, binsThetaN, binsThetaMin, binsThetaMax);
        neutronsKeTheta = new TH2D("neutron_energy_theta", "Neutron Kinetic Energy (MeV) vs Theta (degrees)", binsEnergyN,
                                   binsEnergy, binsThetaN, binsThetaMin, binsThetaMax);
    }
}

void RunAction::EndOfRunAction(const G4Run *) {
    lock_guard<std::mutex> lockInput(inputMutex);
    lock_guard<std::mutex> lockOutput(outputMutex);

    if (isMaster) {
        if (inputHistEnergy) {
            inputHistEnergy->Write("inputKe");
        }
        if (inputHistTheta) {
            inputHistTheta->Write("inputTheta");
        }
        if (inputHistEnergyTheta) {
            inputHistEnergyTheta->Write("inputKeTheta");
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
    G4double theta =
            TMath::ACos(track->GetMomentumDirection().z()) * TMath::RadToDeg();

    if (particleName == "mu-") {
        muonsKe->Fill(kineticEnergy);
        muonsTheta->Fill(theta);
        muonsKeTheta->Fill(kineticEnergy, theta);
    } else if (particleName == "e-") {
        electronsKe->Fill(kineticEnergy);
        electronsTheta->Fill(theta);
        electronsKeTheta->Fill(kineticEnergy, theta);
    } else if (particleName == "gamma") {
        gammasKe->Fill(kineticEnergy);
        gammasTheta->Fill(theta);
        gammasKeTheta->Fill(kineticEnergy, theta);
    } else if (particleName == "proton") {
        protonsKe->Fill(kineticEnergy);
        protonsTheta->Fill(theta);
        protonsKeTheta->Fill(kineticEnergy, theta);
    } else if (particleName == "neutron") {
        neutronsKe->Fill(kineticEnergy);
        neutronsTheta->Fill(theta);
        neutronsKeTheta->Fill(kineticEnergy, theta);
    }
}

std::pair<double, double> RunAction::GetEnergyAndTheta() {
    lock_guard<std::mutex> lock(inputMutex);

    if (!inputHistEnergy || !inputHistTheta || !inputHistEnergyTheta) {
        throw runtime_error("RunAction::GetEnergyAndTheta: input histograms not set");
    }
    double energy, theta;
    inputHistEnergyTheta->GetRandom2(energy, theta);
    // energy = inputHistEnergy->GetRandom();
    // theta = inputHistTheta->GetRandom();

    return {energy * MeV, theta};
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