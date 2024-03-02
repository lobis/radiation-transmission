
#include "EventAction.h"

#include "RunAction.h"

#include <iostream>

using namespace std;


EventAction::EventAction() : G4UserEventAction() {}

void EventAction::BeginOfEventAction(const G4Event *event) {}

void EventAction::EndOfEventAction(const G4Event *event) {}
