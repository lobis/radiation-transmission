
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

TH1F *RunAction::inputHistEnergy = nullptr;
TH1F *RunAction::inputHistTheta = nullptr;
TH2F *RunAction::inputHistEnergyTheta = nullptr;

TH1F *RunAction::muonsKe = nullptr;
TH1F *RunAction::electronsKe = nullptr;
TH1F *RunAction::gammasKe = nullptr;
TH1F *RunAction::protonsKe = nullptr;
TH1F *RunAction::neutronsKe = nullptr;

TH1F *RunAction::muonsTheta = nullptr;
TH1F *RunAction::electronsTheta = nullptr;
TH1F *RunAction::gammasTheta = nullptr;
TH1F *RunAction::protonsTheta = nullptr;
TH1F *RunAction::neutronsTheta = nullptr;

TH2F *RunAction::muonsKeTheta = nullptr;
TH2F *RunAction::electronsKeTheta = nullptr;
TH2F *RunAction::gammasKeTheta = nullptr;
TH2F *RunAction::protonsKeTheta = nullptr;
TH2F *RunAction::neutronsKeTheta = nullptr;

RunAction::RunAction() : G4UserRunAction() {}

void RunAction::BeginOfRunAction(const G4Run *) {
    lock_guard<std::mutex> lock(mutex);

    if (IsMaster()) {
        inputFile = TFile::Open(inputFilename.c_str(), "READ");

        inputHistEnergy = inputFile->Get<TH1F>(string(inputParticleName + "sKe").c_str());
        inputHistTheta = inputFile->Get<TH1F>(string(inputParticleName + "sTheta").c_str());
        inputHistEnergyTheta = inputFile->Get<TH2F>(string(inputParticleName + "sKeTheta").c_str());

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

        muonsKe = new TH1F("muonsKe", "Muons Kinetic Energy (MeV)", binsEnergyN, binsEnergy);
        electronsKe = new TH1F("electronsKe", "Electrons Kinetic Energy (MeV)", binsEnergyN, binsEnergy);
        gammasKe = new TH1F("gammasKe", "Gammas Kinetic Energy (MeV)", binsEnergyN, binsEnergy);
        protonsKe = new TH1F("protonsKe", "Protons Kinetic Energy (MeV)", binsEnergyN, binsEnergy);
        neutronsKe = new TH1F("neutronsKe", "Neutrons Kinetic Energy (MeV)", binsEnergyN, binsEnergy);

        muonsTheta = new TH1F("muonsTheta", "Muons Theta (degrees)", binsThetaN, binsThetaMin, binsThetaMax);
        electronsTheta = new TH1F("electronsTheta", "Electrons Theta (degrees)", binsThetaN, binsThetaMin,
                                  binsThetaMax);
        gammasTheta = new TH1F("gammasTheta", "Gammas Theta (degrees)", binsThetaN, binsThetaMin, binsThetaMax);
        protonsTheta = new TH1F("protonsTheta", "Protons Theta (degrees)", binsThetaN, binsThetaMin, binsThetaMax);
        neutronsTheta = new TH1F("neutronsTheta", "Neutrons Theta (degrees)", binsThetaN, binsThetaMin, binsThetaMax);

        muonsKeTheta = new TH2F("muonsKeTheta", "Muons Kinetic Energy (MeV) vs Theta (degrees)", binsEnergyN,
                                binsEnergy,
                                binsThetaN, binsThetaMin, binsThetaMax);
        electronsKeTheta = new TH2F("electronsKeTheta", "Electrons Kinetic Energy (MeV) vs Theta (degrees)",
                                    binsEnergyN,
                                    binsEnergy, binsThetaN, binsThetaMin, binsThetaMax);
        gammasKeTheta = new TH2F("gammasKeTheta", "Gammas Kinetic Energy (MeV) vs Theta (degrees)", binsEnergyN,
                                 binsEnergy,
                                 binsThetaN, binsThetaMin, binsThetaMax);
        protonsKeTheta = new TH2F("protonsKeTheta", "Protons Kinetic Energy (MeV) vs Theta (degrees)", binsEnergyN,
                                  binsEnergy, binsThetaN, binsThetaMin, binsThetaMax);
        neutronsKeTheta = new TH2F("neutronsKeTheta", "Neutrons Kinetic Energy (MeV) vs Theta (degrees)", binsEnergyN,
                                   binsEnergy, binsThetaN, binsThetaMin, binsThetaMax);
    }
}

void RunAction::EndOfRunAction(const G4Run *) {
    lock_guard<std::mutex> lockInput(inputMutex);
    lock_guard<std::mutex> lockOutput(outputMutex);

    if (!isMaster || !G4Threading::IsMultithreadedApplication()) {
        //
    }

    if (isMaster) {
        inputHistEnergy->Write("inputKe");
        inputHistTheta->Write("inputTheta");
        inputHistEnergyTheta->Write("inputKeTheta");
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
        return "muon";
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