#pragma once
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <Arduino.h>

namespace farmer {
  typedef bool fan;
  typedef bool valve;
  typedef bool pump;

  const int V1 = 2; 
  const int V2 = 3;
  const int V3 = 4;
  const int F1 = 5;
  const int F2 = 6;
  const int P1 = 7;
  const int Heat = 8;

  enum outlet { CONTAIN, ATMOSPHERE, TO_ALGEA, ENUM_SIZE = 3 };
  
  struct fStates {
    float temp;
    fan fan1;
    fan fan2;
    valve V1;
    valve V2;
    valve V3;
    pump pump1;
    outlet out;
  };
  
  // set pin voltages based on current states in fStates f
  static void updateOutputPins(fStates* f) {
    digitalWrite(V1, f->V1); // valve 1
    digitalWrite(V2, f->V2); // valve 2
    digitalWrite(V3, f->V3);  // valve 3
    digitalWrite(F1, f->fan1); // fan 1
    digitalWrite(F2, f->fan2); // fan 2
    digitalWrite(P1, f->pump1); // fan 3
    digitalWrite(Heat, f->temp); // heat
  }

  // using a random function as a placeholder for a CO2 sensor check
  int checkCO2() {
    srand((unsigned)time(0));
    int i;
    i = (rand() % 6) + 1;
    return i;
  }
  
  // cooling state function
  bool cooling(fStates *f) {
    std::cout << "COOLING" << std::endl;
    f->V1 = LOW;
    f->V2 = LOW;
    f->V3 = LOW;
    f->fan1 = LOW;
    f->fan2 = LOW;
    f->temp = LOW;
    f->out = CONTAIN;
    f->pump1 = LOW;
    return true; // return checkSorbent();
  };
  
  // heating state function
  bool heating(fStates *f) {
    std::cout << "HEATING" << std::endl;
    f->fan1 = true;
    f->fan2 = true;
    f->temp = true;
    f->out = CONTAIN;
    f->pump1 = false;
    // regulate temperature ()
    return true; // return checkSorbent();
  };
  
  // desorption state function
  bool desorption(fStates *f) {
    std::cout << "DESORPTION" << std::endl;
    f->V1 = HIGH;
    f->V2 = LOW;
    f->V3 = HIGH;
    f->fan1 = HIGH;
    f->fan2 = HIGH;
    f->temp = LOW;
    f->pump1 = HIGH;
    f->out = TO_ALGEA;
    // regulate temperature ()
    return checkCO2(/* multiplier parameter, 1.1 */); // CO2_out <= 1.1 * CO2_in
  };
  
  // adsorption state function
  bool adsorption(fStates *f) {
    std::cout << "ADSORPTION" << std::endl;
    f->V1 = HIGH;
    f->V2 = HIGH;
    f->V3 = LOW;
    f->fan1 = HIGH;
    f->fan2 = LOW;
    f->temp = LOW;
    f->pump1 = LOW;
    f->out = ATMOSPHERE;
    return checkCO2(/* multiplier parameter, 0.9 */) <= 2; // CO2_out >= 0.9 * CO2_in
  };
  
  // a 'true' return is an indication
  // that the conditions for switching states was met
  bool (*stateFunc[4])(fStates *f);
} // namespace farmer