
#pragma once

#include <G4RunManager.hh>
#include <G4UserRunAction.hh>

#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>

class RunAction : public G4UserRunAction {
public:
    RunAction();

    void BeginOfRunAction(const G4Run *) override;

    void EndOfRunAction(const G4Run *) override;

    static void InsertTrack(const G4Track *track);

    static std::pair<double, double> GenerateEnergyAndZenith(const std::string &particle);

    static std::string ChooseParticle();

    static std::string GetGeant4ParticleName(const std::string &particleName); // electron -> e-, etc.

    static void SetInputParticles(const std::set<std::string> &particleNames);

    static void SetInputFilename(const std::string &inputFilename);

    static void SetOutputFilename(const std::string &outputFilename);

    static void SetRequestedPrimaries(int);

    static int GetRequestedPrimaries();

    static void SetRequestedSecondaries(int);

    static int GetRequestedSecondaries();

    static void IncreaseLaunchedPrimaries();

    static unsigned int GetLaunchedPrimaries();

    static unsigned long long GetSecondariesCount();

private:
    static int requestedPrimaries;
    static int requestedSecondaries;

    static unsigned int launchedPrimaries;
    static unsigned long long secondariesCount;

    static std::string inputFilename;
    static std::string outputFilename;

    static std::mutex inputMutex;
    static std::mutex outputMutex;

    static std::set<std::string> inputParticleNames;
    static TFile *inputFile;
    static TFile *outputFile;

    static std::map<std::string, std::tuple<TH2D *, TH1D *, TH1D *>> inputParticleHists;
    static std::map<std::string, double> inputParticleWeights; // based on the counts in the input histograms

    static TH1D *muonsEnergy;
    static TH1D *electronsEnergy;
    static TH1D *gammasEnergy;
    static TH1D *protonsEnergy;
    static TH1D *neutronsEnergy;

    static TH1D *muonsZenith;
    static TH1D *electronsZenith;
    static TH1D *gammasZenith;
    static TH1D *protonsZenith;
    static TH1D *neutronsZenith;

    static TH2D *muonsEnergyZenith;
    static TH2D *electronsEnergyZenith;
    static TH2D *gammasEnergyZenith;
    static TH2D *protonsEnergyZenith;
    static TH2D *neutronsEnergyZenith;
};


