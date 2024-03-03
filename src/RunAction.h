
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

    static std::pair<double, double> GetEnergyAndTheta();

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
    static TH1D *inputHistTheta;
    static TH2D *inputHistEnergyTheta;

    static TH1D *muonsKe;
    static TH1D *electronsKe;
    static TH1D *gammasKe;
    static TH1D *protonsKe;
    static TH1D *neutronsKe;

    static TH1D *muonsTheta;
    static TH1D *electronsTheta;
    static TH1D *gammasTheta;
    static TH1D *protonsTheta;
    static TH1D *neutronsTheta;

    static TH2D *muonsKeTheta;
    static TH2D *electronsKeTheta;
    static TH2D *gammasKeTheta;
    static TH2D *protonsKeTheta;
    static TH2D *neutronsKeTheta;
};


