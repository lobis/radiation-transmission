
#include "EventAction.h"

#include "RunAction.h"

#include <iostream>

using namespace std;


EventAction::EventAction() : G4UserEventAction() {}

void EventAction::BeginOfEventAction(const G4Event *event) {
    if (event->GetEventID() % 100 == 0) {
        G4cout << "EventAction::BeginOfEventAction: "
               << "ID: " << event->GetEventID() << G4endl;
    }
}

void EventAction::EndOfEventAction(const G4Event *event) {}
