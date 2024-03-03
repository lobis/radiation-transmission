
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

    static std::pair<double, double> GetEnergyAndZenith();

    static std::string GetInputParticleName();

    static void SetInputParticle(const std::string &inputParticleName);

    static void SetInputFilename(const std::string &inputFilename);

    static void SetOutputFilename(const std::string &outputFilename);

    static void SetRequestedPrimaries(int requestedPrimaries);

    static int GetRequestedPrimaries();

    static void IncreaseLaunchedPrimaries();

    static unsigned int GetLaunchedPrimaries();

private:
    static int requestedPrimaries;
    static unsigned int launchedPrimaries;

    static std::string inputFilename;
    static std::string outputFilename;

    static std::mutex inputMutex;
    static std::mutex outputMutex;

    static std::string inputParticleName;
    static TFile *inputFile;
    static TFile *outputFile;

    static TH1D *inputHistEnergy;
    static TH1D *inputHistZenith;
    static TH2D *inputHistEnergyZenith;

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


