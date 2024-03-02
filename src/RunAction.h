
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

    void InsertTrack(const G4Track *track);

    std::pair<double, double> GetEnergyAndTheta();

private:
    std::mutex inputMutex;
    std::string inputParticleName;
    TFile *inputFile = nullptr;
    TH1F *inputHistEnergy = nullptr;
    TH1F *inputHistTheta = nullptr;
    TH2F *inputHistEnergyTheta = nullptr;

    std::mutex outputMutex;
    TFile *outputFile = nullptr;

    TH1F *muonsKe = nullptr;
    TH1F *electronsKe = nullptr;
    TH1F *gammasKe = nullptr;
    TH1F *protonsKe = nullptr;
    TH1F *neutronsKe = nullptr;

    TH1F *muonsTheta = nullptr;
    TH1F *electronsTheta = nullptr;
    TH1F *gammasTheta = nullptr;
    TH1F *protonsTheta = nullptr;
    TH1F *neutronsTheta = nullptr;

    TH2F *muonsKeTheta = nullptr;
    TH2F *electronsKeTheta = nullptr;
    TH2F *gammasKeTheta = nullptr;
    TH2F *protonsKeTheta = nullptr;
    TH2F *neutronsKeTheta = nullptr;
};


