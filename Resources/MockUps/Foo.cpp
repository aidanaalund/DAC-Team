#include <iostream>
#include <stdlib.h>
#include <unistd.h>   
#include "Foo.h"

// directly related to the farmer/farmer pipeline
static farmer::fStates farmer_state;
static bool RUNNING = true;
static int CUR_STATE = 0;

// stuff for simulation
const char* LINUX_CLEAR = "clear";
const int SLEEP_DELAY = 1000;

void setup() {
  //setting default states
  pinMode(V1, OUTPUT);
  pinMode(V2, OUTPUT);
  pinMode(V3, OUTPUT);
  pinMode(F1, OUTPUT);
  pinMode(F2, OUTPUT);
  pinMode(P1, OUTPUT);
  
  farmer::cooling(&farmer_state);
  updateOutputPins(&farmer_state); // setting defaults (everything to low)

  // storing state functions in array, cooling -> adsorption -> heating -> desorption
  farmer::stateFunc[0] = farmer::cooling;
  farmer::stateFunc[1] = farmer::adsorption;
  farmer::stateFunc[2] = farmer::heating;
  farmer::stateFunc[3] = farmer::desorption;
}

void loop() {
  // run state code
  bool can_move_on = farmer::stateFunc[CUR_STATE](&farmer_state);
  farmer::updateOutputPins(&farmer_state); // pass in states & update booleans

  CUR_STATE = (CUR_STATE + can_move_on) * !(CUR_STATE == 3);
  usleep(90000 * can_move_on);
  if (CUR_STATE == 0) system(LINUX_CLEAR);
}

int main() {
  setup();
  while (RUNNING)
    loop();
}