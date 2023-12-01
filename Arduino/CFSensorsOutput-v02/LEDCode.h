#pragma once
#include <Arduino.h>
#define COLOR_MAX 255

static uint8_t REDPIN = 0;
static uint8_t GREENPIN = 0;
static uint8_t BLUEPIN = 0;
static bool lights = false;

// call this in setup
void LEDSetup(int rpin, int gpin, int bpin, bool LIGHTS_ON) {
  REDPIN = rpin;
  GREEPIN = gpin;
  BLUEPIN = bpin;
  lights = LIGHTS_ON;

  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);

  Serial.println("LED Strip setup complete.");
}

// write to each color channel
void LEDWrite(uint8_t rval, uint8_t gval, uint8_t bval) {
  analogWrite(REDPIN, rval & LIGHTS_ON);
  analogWrite(GREENPIN, gval & LIGHTS_ON);
  analogWrite(BLUEPIN, bval & LIGHTS_ON);
}