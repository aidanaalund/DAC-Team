#pragma once
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <Arduino.h>

namespace vector4 {
  struct vec4 { float r, g, b, a;};
  struct ivec4 { int r, g, b, a; };

  // type conversion
  vec4 to_vec4(const ivec4& v) return {(float)v.r, (float)v.g, (float)v.b, (float)v.a};
  ivec4 to_ivec4(const vec4& v) return {(int)v.r, (int)v.g, (int)v.b, (int)v.a};

  // vector operations
  vec4 operator+(const vec4& v1, const vec4& v2) return {v1.r + v2.r, v1.g + v2.g, v1.b + v2.b, v1.a + v2.a};
  vec4 operator-(const vec4& v1, const vec4& v2) return {v1.r - v2.r, v1.g - v2.g, v1.b - v2.b, v1.a - v2.a};
  vec4 operator*(const float scalar, const vec4& v) return {scalar * v.r, scalar * v.g, scalar * v.b, scalar * v.a};
  vec4 operator*(const vec4& v, const float scalar) return scalar * v;
  vec4 operator/(const vec4& v, const float scalar) return {v.r / scalar, v.g / scalar, v.b / scalar, v.a / scalar};
  vec4 operator/(const float scalar, const vec4& v) return scalar * v;

  // vector functions
  vec4 cross(const vec4& v1, const vec4& v2) return {v1.g * v2.b - v1.b * v2.g, v1.b * v2.r - v1.r * v2.b, v1.r * v2.g - v1.g * v2.r, 0};
  vec4 normalize(const vec4& v) return v / magnitude(v);
  float dot(const vec4& v1, const vec4& v2) return v1.r * v2.r + v1.g * v2.g + v1.b * v2.b + v1.a * v2.a;
  float magnitude(const vec4& v) return sqrt(v.r * v.r + v.g * v.g + v.b * v.b + v.a * v.a);
}

// LED CODE
namespace led {
  // PINS
  const int RED_PIN = 6;
  const int GREEN_PIN = 5;
  const int BLUE_PIN = 3;
  const int ALPHA_PIN = 9;

  // SWITCHES
  bool LIGHTS_ON = false;

  // CONSTANTS
  const int MAX_BRIGHTNESS = 255;
  const int FADE_SPEED = 5;

  // pin initalization
  void ledPinSetup() {
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
    pinMode(ALPHA_PIN, OUTPUT);
  }

  void toggleWrite(int pin, uint8_t val) {
    digitalWrite(pin, val & LIGHTS_ON);
  }

  void ledWrite(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    toggleWrite(RED_PIN, r);
    toggleWrite(GREEN_PIN, g);
    toggleWrite(BLUE_PIN, b);
    toggleWrite(ALPHA_PIN, a);
  }
}

namespace interaction {
  int numPeople = 0;
  const int MAX_PEOPLE = 5;
}

namespace farmer {
  // definition changes for readability
  typedef bool fan;
  typedef bool valve;
  typedef bool pump;

  // PINS
  const int V1 = 2; 
  const int V2 = 3;
  const int V3 = 4;
  const int F1 = 5;
  const int F2 = 6;
  const int P1 = 7;
  const int Heat = 8;

  // pin initalization
  void farmerPinSetup() {
    pinMode(V1, OUTPUT);
    pinMode(V2, OUTPUT);
    pinMode(V3, OUTPUT);
    pinMode(F1, OUTPUT);
    pinMode(F2, OUTPUT);
    pinMode(P1, OUTPUT);
  }

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
    digitalWrite(V1, f->V1);      // valve 1
    digitalWrite(V2, f->V2);      // valve 2
    digitalWrite(V3, f->V3);      // valve 3
    digitalWrite(F1, f->fan1);    // fan 1
    digitalWrite(F2, f->fan2);    // fan 2
    digitalWrite(P1, f->pump1);   // fan 3
    digitalWrite(Heat, f->temp);  // heat
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

  // min state, wait for number of interactors
  bool awaitMinPeople(fStates *f) {
    // same states as cooling
    f->V1 = LOW;
    f->V2 = LOW;
    f->V3 = LOW;
    f->fan1 = LOW;
    f->fan2 = LOW;
    f->temp = LOW;
    f->out = CONTAIN;
    f->pump1 = LOW;

    // LED code & interaction checking
    led::ledWrite(0, led::MAX_BRIGHTNESS * (interaction::numPeople / interaction::MAX_PEOPLE), 0, 0);
    bool can_move_on = interaction::numPeople >= interaction::MAX_PEOPLE;
    interaction::numPeople = interaction::numPeople * !can_move_on;
    return can_move_on;
  }
  
  // a 'true' return is an indication
  // that the conditions for switching states was met
  bool (*stateFunc[5])(fStates *f);
  const int MAX_STATES = 5;
}
