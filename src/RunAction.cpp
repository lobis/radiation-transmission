
#include "RunAction.h"

#include <iostream>
#include <TMath.h>

using namespace std;
using namespace CLHEP;

int RunAction::requestedPrimaries = 0;
int RunAction::requestedSecondaries = 0;
unsigned int RunAction::launchedPrimaries = 0;
unsigned long long RunAction::secondariesCount = 0;

mutex RunAction::inputMutex;
mutex RunAction::outputMutex;

set<string> RunAction::inputParticleNames = {};
map<string, double> RunAction::inputParticleWeights = {};
double RunAction::secondariesTotalWeight;
set<string> RunAction::inputParticleNamesAllowed = {"neutron", "gamma", "proton", "electron", "muon"};

string RunAction::inputFilename;
string RunAction::outputFilename;

TFile *RunAction::inputFile = nullptr;
TFile *RunAction::outputFile = nullptr;

map<string, tuple<TH2D *, TH1D *, TH1D *>> RunAction::inputParticleHists = {};

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

        for (const auto &particleName: inputParticleNamesAllowed) {
            inputParticleHists[particleName] = {
                    inputFile->Get<TH2D>(string(particleName + "_energy_zenith").c_str()),
                    inputFile->Get<TH1D>(string(particleName + "_energy").c_str()),
                    inputFile->Get<TH1D>(string(particleName + "_zenith").c_str())
            };

            get<0>(inputParticleHists[particleName])->SetName(
                    string("input_" + particleName + "_energy_zenith").c_str());
            get<1>(inputParticleHists[particleName])->SetName(
                    string("input_" + particleName + "_energy").c_str());
            get<2>(inputParticleHists[particleName])->SetName(
                    string("input_" + particleName + "_zenith").c_str());

            inputParticleWeights[particleName] = get<0>(inputParticleHists[particleName])->GetEntries();
        }

        // normalize inputParticleWeights
        double sumTotal = 0;
        double sumPartial = 0;
        for (const auto &particleName: inputParticleNamesAllowed) {
            const auto weight = inputParticleWeights[particleName];
            sumTotal += weight;
            if (inputParticleNames.find(particleName) != inputParticleNames.end()) {
                sumPartial += weight;
            }
        }
        secondariesTotalWeight = sumPartial / sumTotal;
        for (auto &entry: inputParticleWeights) {
            entry.second /= sumPartial;
        }

        cout << "Particle weights:" << endl;
        for (const auto &particleName: inputParticleNames) {
            cout << "    - " << particleName << " relative weight: " << inputParticleWeights[particleName] << endl;
        }
        cout << "Total weight: " << secondariesTotalWeight << endl;

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
    if (isMaster) {
        lock_guard<std::mutex> lockInput(inputMutex);
        lock_guard<std::mutex> lockOutput(outputMutex);

        // write metadata
        const auto countPerSecondPerSquareMeterInput = inputFile->Get<TNamed>(
                "secondaries_per_second_per_square_meter");
        // parse into double
        const auto countPerSecondPerSquareMeterInputDouble = stod(countPerSecondPerSquareMeterInput->GetTitle());
        const auto countPerSecondPerSquareMeterOutputDouble =
                countPerSecondPerSquareMeterInputDouble * (double) secondariesCount /
                (double) launchedPrimaries * secondariesTotalWeight;

        TNamed secondariesPerSecondPerSquareMeterNamed("secondaries_per_second_per_square_meter",
                                                       Form("%.6E", countPerSecondPerSquareMeterOutputDouble));
        secondariesPerSecondPerSquareMeterNamed.Write();

        TNamed secondariesCountNamed("secondaries_count", to_string(secondariesCount).c_str());
        secondariesCountNamed.Write();

        auto latitudeNamed = inputFile->Get<TNamed>("latitude");
        latitudeNamed->Write();

        // write input hists
        for (const auto &entry: inputParticleHists) {
            get<1>(entry.second)->Write();
            get<2>(entry.second)->Write();
            get<0>(entry.second)->Write(); // write this last to keep consistent style
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

    // TODO: split mu- and e-
    if (particleName == "mu-" || particleName == "mu+") {
        muonsEnergy->Fill(kineticEnergy);
        muonsZenith->Fill(zenith);
        muonsEnergyZenith->Fill(kineticEnergy, zenith);
    } else if (particleName == "e-" || particleName == "e+") {
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
    } else {
        return;
    }

    secondariesCount += 1;
    if (requestedSecondaries > 0 && secondariesCount >= requestedSecondaries) {
        G4RunManager::GetRunManager()->AbortRun(true);
    }
}

std::pair<double, double> RunAction::GenerateEnergyAndZenith(const string &particle) {
    double energy, zenith;

    const auto hist = get<0>(inputParticleHists[particle]);

    lock_guard<std::mutex> lock(inputMutex);

    hist->GetRandom2(energy, zenith);

    return {energy * MeV, zenith};
}

std::string RunAction::GetGeant4ParticleName(const std::string &particleName) {
    if (particleName == "neutron") {
        return "neutron";
    } else if (particleName == "proton") {
        return "proton";
    } else if (particleName == "muon") {
        return "mu-";
    } else if (particleName == "electron") {
        return "e-";
    } else if (particleName == "gamma") {
        return "gamma";
    } else {
        throw runtime_error("RunAction::GetInputParticleName: unknown input particle name: " + particleName);
    }
}

void RunAction::SetInputParticles(const std::set<std::string> &particleNames) {
    inputParticleNames = particleNames;
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

void RunAction::SetRequestedSecondaries(int newValue) {
    RunAction::requestedSecondaries = newValue;
}

int RunAction::GetRequestedPrimaries() {
    return RunAction::requestedPrimaries;
}

int RunAction::GetRequestedSecondaries() {
    return RunAction::requestedSecondaries;
}

unsigned long long RunAction::GetSecondariesCount() {
    lock_guard<std::mutex> lock(outputMutex);
    return RunAction::secondariesCount;
}

void RunAction::IncreaseLaunchedPrimaries() {
    RunAction::launchedPrimaries++;
}

unsigned int RunAction::GetLaunchedPrimaries() {
    lock_guard<std::mutex> lock(inputMutex);
    return RunAction::launchedPrimaries;
}

std::string RunAction::ChooseParticle() {
    lock_guard<std::mutex> lock(inputMutex);

    if (inputParticleNames.size() == 1) {
        return *inputParticleNames.begin();
    } else {
        const auto random = G4UniformRand();
        // choose a random particle based on the weights
        double sum = 0;
        for (const auto &entry: inputParticleWeights) {
            sum += entry.second;
            if (random <= sum) {
                return entry.first;
            }
        }
    }

    throw runtime_error("RunAction::ChooseParticle: could not choose a particle");
}
