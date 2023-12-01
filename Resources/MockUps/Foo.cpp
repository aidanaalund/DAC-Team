#include <iostream>
#include <stdlib.h>
#include <unistd.h>   
#include "Foo.h"

// directly related to the farmer/farmer pipeline
static farmer::fStates farmer_state;
static int CUR_STATE = 0;

// defined for the sake of simulating runtime
namespace simulation {
  bool RUNNING = true;
  const char* LINUX_CLEAR = "clear";
  const int SLEEP_DELAY = 90000;
}

void setup() {
  //farmer setup & defaults
  farmer::farmerPinSetup();
  farmer::cooling(&farmer_state); // initial state in cooling
  farmer::updateOutputPins(&farmer_state); // setting defaults (everything to low)

  // storing state functions in array, cooling -> adsorption -> heating -> desorption
  farmer::stateFunc[0] = farmer::waitMinPeople;
  farmer::stateFunc[1] = farmer::cooling;
  farmer::stateFunc[2] = farmer::adsorption;
  farmer::stateFunc[3] = farmer::heating;
  farmer::stateFunc[4] = farmer::desorption;

  // LED setup
  led::ledPinSetup();
}

void loop() {
  // run state code
  bool can_move_on = farmer::stateFunc[CUR_STATE](&farmer_state);
  farmer::updateOutputPins(&farmer_state); // pass in states & update booleans

  CUR_STATE = (CUR_STATE + can_move_on) * !(CUR_STATE == farmer::MAX_STATES - 1);
  usleep(simulation::SLEEP_DELAY * can_move_on); // delay between state changes
  if (CUR_STATE == 0) system(simulation::LINUX_CLEAR);
}

int main() {
  setup();
  while (simulation::RUNNING)
    loop();
}