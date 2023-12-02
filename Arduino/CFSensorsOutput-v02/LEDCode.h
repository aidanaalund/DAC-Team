#pragma once
#include <Arduino.h>
#define COLOR_MAX 255

// NOTE: ALL OF THESE PINS NEED TO HAVE PWM CAPABILITIES SINCE WE ARE USING DIGITAL OUTS
static uint8_t REDPIN = 0;
static uint8_t GREENPIN = 0;
static uint8_t BLUEPIN = 0;
static uint8_t ALPHAPIN = 0;

// should I use a macro to just automatically switch between the waterproof & normal led strips?

static bool LIGHTS_ON = false;

// call this in setup - preferably choose PWM capable pins so we don't burn things up
void LEDSetup(int rpin, int gpin, int bpin /*, int apin*/, bool LIGHTS_ON) { // uncomment on waterproof led use
  REDPIN = rpin;
  GREEPIN = gpin;
  BLUEPIN = bpin;
  //ALPHAPIN = apin;
  LIGHTS_ON = lights

  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
  //pinMode(ALPHAPIN, OUTPUT);

  Serial.println("LED Strip setup complete.");
}

// write to each color channel: red, green, blue, alpha
void LEDWrite(uint8_t rval, uint8_t gval, uint8_t bval, uint8_t aval) {
  analogWrite(REDPIN, rval & LIGHTS_ON);
  analogWrite(GREENPIN, gval & LIGHTS_ON);
  analogWrite(BLUEPIN, bval & LIGHTS_ON);
  //analogWrite(ALPHAPIN, aval & LIGHTS_ON); // uncomment on waterproof led use
}

// transition from one RGB color to a new RGB color (not super sure what alpha pin does so that may be implemented wrong here)
void LEDTransition(uint8_t prevColor[], uint8_t newColor[], int steps, int delayTime) {
  float delta[4];
  for (int i = 0; i < 4; i++) {
    delta[i] = (newColor[i] - prevColor[i]) / (float)steps;
  }

  for (int step = 0; step <= steps; step++) {
    uint8_t currentColor[4];
    for (int i = 0; i < 4; i++) {
      currentColor[i] = prevColor[i] + (delta[i] * step);
    }

    LEDWrite(currentColor[0], currentColor[1], currentColor[2], currentColor[3]);
    delay(delayTime);
  }
}

void LEDTransition