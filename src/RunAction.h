
#pragma once

#include <G4RunManager.hh>
#include <G4UserRunAction.hh>

#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>

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

private:
    static std::string inputFilename;
    static std::string outputFilename;

    static std::mutex inputMutex;
    static std::mutex outputMutex;

    static std::string inputParticleName;
    static TFile *inputFile;
    static TFile *outputFile;

    static TH1F *inputHistEnergy;
    static TH1F *inputHistTheta;
    static TH2F *inputHistEnergyTheta;

    static TH1F *muonsKe;
    static TH1F *electronsKe;
    static TH1F *gammasKe;
    static TH1F *protonsKe;
    static TH1F *neutronsKe;

    static TH1F *muonsTheta;
    static TH1F *electronsTheta;
    static TH1F *gammasTheta;
    static TH1F *protonsTheta;
    static TH1F *neutronsTheta;

    static TH2F *muonsKeTheta;
    static TH2F *electronsKeTheta;
    static TH2F *gammasKeTheta;
    static TH2F *protonsKeTheta;
    static TH2F *neutronsKeTheta;
};


