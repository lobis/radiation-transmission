
#include "RunAction.h"

#include <iostream>
#include <TMath.h>
#include <TSystem.h>
#include <filesystem>
#include <numeric>

using namespace std;
using namespace CLHEP;

int RunAction::requestedPrimaries = 0;
int RunAction::requestedSecondaries = 0;

map<string, double> RunAction::launchedPrimariesMap = {};

mutex RunAction::inputMutex;
mutex RunAction::outputMutex;

set<string> RunAction::inputParticleNames = {};
map<string, double> RunAction::inputParticleWeights = {};
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
        double sum = 0;
        for (const auto &particle: inputParticleNames) {
            const auto weight = inputParticleWeights[particle];
            sum += weight;
        }
        for (auto &entry: inputParticleWeights) {
            entry.second /= sum;
        }

        cout << "Particle weights:" << endl;
        for (const auto &particleName: inputParticleNames) {
            cout << "    - " << particleName << " relative weight: " << inputParticleWeights[particleName] << endl;
        }

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
        muonsEnergy->GetXaxis()->SetTitle("Energy (MeV)");
        muonsEnergy->GetYaxis()->SetTitle("Counts / s / m2");

        electronsEnergy = new TH1D("electron_energy", "Electron Kinetic Energy (MeV)", binsEnergyN, binsEnergy);
        electronsEnergy->GetXaxis()->SetTitle("Energy (MeV)");
        electronsEnergy->GetYaxis()->SetTitle("Counts / s / m2");

        gammasEnergy = new TH1D("gamma_energy", "Gamma Kinetic Energy (MeV)", binsEnergyN, binsEnergy);
        gammasEnergy->GetXaxis()->SetTitle("Energy (MeV)");
        gammasEnergy->GetYaxis()->SetTitle("Counts / s / m2");

        protonsEnergy = new TH1D("proton_energy", "Proton Kinetic Energy (MeV)", binsEnergyN, binsEnergy);
        protonsEnergy->GetXaxis()->SetTitle("Energy (MeV)");
        protonsEnergy->GetYaxis()->SetTitle("Counts / s / m2");

        neutronsEnergy = new TH1D("neutron_energy", "Neutron Kinetic Energy (MeV)", binsEnergyN, binsEnergy);
        neutronsEnergy->GetXaxis()->SetTitle("Energy (MeV)");
        neutronsEnergy->GetYaxis()->SetTitle("Counts / s / m2");

        muonsZenith = new TH1D("muon_zenith", "Muon Zenith Angle (degrees)", binsZenithN, binsZenithMin, binsZenithMax);
        muonsZenith->GetXaxis()->SetTitle("Zenith Angle (degrees)");
        muonsZenith->GetYaxis()->SetTitle("Counts / s / m2");

        electronsZenith = new TH1D("electron_zenith", "Electron Zenith Angle (degrees)", binsZenithN, binsZenithMin,
                                   binsZenithMax);
        electronsZenith->GetXaxis()->SetTitle("Zenith Angle (degrees)");
        electronsZenith->GetYaxis()->SetTitle("Counts / s / m2");

        gammasZenith = new TH1D("gamma_zenith", "Gamma Zenith Angle (degrees)", binsZenithN, binsZenithMin,
                                binsZenithMax);
        gammasZenith->GetXaxis()->SetTitle("Zenith Angle (degrees)");
        gammasZenith->GetYaxis()->SetTitle("Counts / s / m2");

        protonsZenith = new TH1D("proton_zenith", "Proton Zenith Angle (degrees)", binsZenithN, binsZenithMin,
                                 binsZenithMax);
        protonsZenith->GetXaxis()->SetTitle("Zenith Angle (degrees)");
        protonsZenith->GetYaxis()->SetTitle("Counts / s / m2");

        neutronsZenith = new TH1D("neutron_zenith", "Neutron Zenith Angle (degrees)", binsZenithN, binsZenithMin,
                                  binsZenithMax);
        neutronsZenith->GetXaxis()->SetTitle("Zenith Angle (degrees)");
        neutronsZenith->GetYaxis()->SetTitle("Counts / s / m2");

        muonsEnergyZenith = new TH2D("muon_energy_zenith", "Muon Kinetic Energy (MeV) vs Zenith Angle (degrees)",
                                     binsEnergyN,
                                     binsEnergy,
                                     binsZenithN, binsZenithMin, binsZenithMax);
        muonsEnergyZenith->GetXaxis()->SetTitle("Energy (MeV)");
        muonsEnergyZenith->GetYaxis()->SetTitle("Zenith Angle (degrees)");
        muonsEnergyZenith->GetZaxis()->SetTitle("Counts / s / m2");

        electronsEnergyZenith = new TH2D("electron_energy_zenith",
                                         "Electron Kinetic Energy (MeV) vs Zenith Angle (degrees)",
                                         binsEnergyN,
                                         binsEnergy, binsZenithN, binsZenithMin, binsZenithMax);
        electronsEnergyZenith->GetXaxis()->SetTitle("Energy (MeV)");
        electronsEnergyZenith->GetYaxis()->SetTitle("Zenith Angle (degrees)");
        electronsEnergyZenith->GetZaxis()->SetTitle("Counts / s / m2");

        gammasEnergyZenith = new TH2D("gamma_energy_zenith", "Gamma Kinetic Energy (MeV) vs Zenith Angle (degrees)",
                                      binsEnergyN,
                                      binsEnergy,
                                      binsZenithN, binsZenithMin, binsZenithMax);
        gammasEnergyZenith->GetXaxis()->SetTitle("Energy (MeV)");
        gammasEnergyZenith->GetYaxis()->SetTitle("Zenith Angle (degrees)");
        gammasEnergyZenith->GetZaxis()->SetTitle("Counts / s / m2");

        protonsEnergyZenith = new TH2D("proton_energy_zenith", "Proton Kinetic Energy (MeV) vs Zenith Angle (degrees)",
                                       binsEnergyN,
                                       binsEnergy, binsZenithN, binsZenithMin, binsZenithMax);
        protonsEnergyZenith->GetXaxis()->SetTitle("Energy (MeV)");
        protonsEnergyZenith->GetYaxis()->SetTitle("Zenith Angle (degrees)");
        protonsEnergyZenith->GetZaxis()->SetTitle("Counts / s / m2");

        neutronsEnergyZenith = new TH2D("neutron_energy_zenith",
                                        "Neutron Kinetic Energy (MeV) vs Zenith Angle (degrees)", binsEnergyN,
                                        binsEnergy, binsZenithN, binsZenithMin, binsZenithMax);
        neutronsEnergyZenith->GetXaxis()->SetTitle("Energy (MeV)");
        neutronsEnergyZenith->GetYaxis()->SetTitle("Zenith Angle (degrees)");
        neutronsEnergyZenith->GetZaxis()->SetTitle("Counts / s / m2");
    }
}

void RunAction::EndOfRunAction(const G4Run *) {
    if (isMaster) {
        lock_guard<std::mutex> lockInput(inputMutex);
        lock_guard<std::mutex> lockOutput(outputMutex);

        if (launchedPrimariesMap["muon"] > 0) {
            muonsEnergy->Scale(get<1>(inputParticleHists["muon"])->Integral() / launchedPrimariesMap.at("muon"));
            muonsZenith->Scale(get<2>(inputParticleHists["muon"])->Integral() / launchedPrimariesMap.at("muon"));
            muonsEnergyZenith->Scale(get<0>(inputParticleHists["muon"])->Integral() / launchedPrimariesMap.at("muon"));
        }
        if (launchedPrimariesMap["electron"] > 0) {
            electronsEnergy->Scale(
                    get<1>(inputParticleHists["electron"])->Integral() / launchedPrimariesMap.at("electron"));
            electronsZenith->Scale(
                    get<2>(inputParticleHists["electron"])->Integral() / launchedPrimariesMap.at("electron"));
            electronsEnergyZenith->Scale(
                    get<0>(inputParticleHists["electron"])->Integral() / launchedPrimariesMap.at("electron"));
        }
        if (launchedPrimariesMap["gamma"] > 0) {
            gammasEnergy->Scale(get<1>(inputParticleHists["gamma"])->Integral() / launchedPrimariesMap.at("gamma"));
            gammasZenith->Scale(get<2>(inputParticleHists["gamma"])->Integral() / launchedPrimariesMap.at("gamma"));
            gammasEnergyZenith->Scale(
                    get<0>(inputParticleHists["gamma"])->Integral() / launchedPrimariesMap.at("gamma"));
        }
        if (launchedPrimariesMap["proton"] > 0) {
            protonsEnergy->Scale(get<1>(inputParticleHists["proton"])->Integral() / launchedPrimariesMap.at("proton"));
            protonsZenith->Scale(get<2>(inputParticleHists["proton"])->Integral() / launchedPrimariesMap.at("proton"));
            protonsEnergyZenith->Scale(
                    get<0>(inputParticleHists["proton"])->Integral() / launchedPrimariesMap.at("proton"));
        }
        if (launchedPrimariesMap["neutron"] > 0) {
            neutronsEnergy->Scale(
                    get<1>(inputParticleHists["neutron"])->Integral() / launchedPrimariesMap.at("neutron"));
            neutronsZenith->Scale(
                    get<2>(inputParticleHists["neutron"])->Integral() / launchedPrimariesMap.at("neutron"));
            neutronsEnergyZenith->Scale(
                    get<0>(inputParticleHists["neutron"])->Integral() / launchedPrimariesMap.at("neutron"));
        }

        cout << "Total launched primaries: " << GetLaunchedPrimaries(false) << endl;
        cout << "Total secondaries: " << GetSecondariesCount(false) << endl;

        cout << "Secondaries flux (counts / s / m2): "
             << muonsEnergyZenith->Integral() + electronsEnergyZenith->Integral() +
                gammasEnergyZenith->Integral() + protonsEnergyZenith->Integral() +
                neutronsEnergyZenith->Integral() << endl;
        cout << "    - muons: " << muonsEnergyZenith->Integral() << endl;
        cout << "    - electrons: " << electronsEnergyZenith->Integral() << endl;
        cout << "    - gammas: " << gammasEnergyZenith->Integral() << endl;
        cout << "    - protons: " << protonsEnergyZenith->Integral() << endl;
        cout << "    - neutrons: " << neutronsEnergyZenith->Integral() << endl;


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

    if (requestedSecondaries > 0 && GetSecondariesCount(false) >= requestedSecondaries) {
        G4RunManager::GetRunManager()->AbortRun(true);
    }
}

std::pair<double, double> RunAction::GenerateEnergyAndZenith(const string &particle) {
    double energy, zenith;

    lock_guard<std::mutex> lock(inputMutex);

    const auto hist = get<0>(inputParticleHists[particle]);

    hist->GetRandom2(energy, zenith);

    return {energy, zenith};
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

unsigned long long RunAction::GetSecondariesCount(bool lock) {
    if (muonsEnergyZenith == nullptr || electronsEnergyZenith == nullptr || gammasEnergyZenith == nullptr ||
        protonsEnergyZenith == nullptr || neutronsEnergyZenith == nullptr) {
        return 0;
    }

    if (lock) {
        outputMutex.lock();
    }
    auto count = muonsEnergyZenith->GetEntries() + electronsEnergyZenith->GetEntries() +
                 gammasEnergyZenith->GetEntries() + protonsEnergyZenith->GetEntries() +
                 neutronsEnergyZenith->GetEntries();
    if (lock) {
        outputMutex.unlock();
    }
    return count;
}

void RunAction::IncreaseLaunchedPrimaries(const string &particleName) {
    lock_guard<std::mutex> lock(inputMutex);
    RunAction::launchedPrimariesMap[particleName]++;
}

unsigned int RunAction::GetLaunchedPrimaries(bool lock) {
    if (lock) {
        inputMutex.lock();
    }
    auto count = std::accumulate(launchedPrimariesMap.begin(), launchedPrimariesMap.end(), 0,
                                 [](unsigned int sum, const auto &entry) { return sum + entry.second; });
    if (lock) {
        inputMutex.unlock();
    }
    return count;
}

std::string RunAction::ChooseParticle() {
    lock_guard<std::mutex> lock(inputMutex);

    if (inputParticleNames.size() == 1) {
        return *inputParticleNames.begin();
    } else {
        const auto random = G4UniformRand();
        // choose a random particle based on the weights
        double sum = 0;
        for (const auto &particle: inputParticleNames) {
            sum += inputParticleWeights[particle];
            if (random <= sum) {
                return particle;
            }
        }
    }

    throw runtime_error("RunAction::ChooseParticle: could not choose a particle");
}
