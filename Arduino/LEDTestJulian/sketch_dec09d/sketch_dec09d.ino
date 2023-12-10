//// Libraries
#include <Adafruit_SCD30.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PID_v1.h>
#include <Wire.h>
#include <SparkFun_FS3000_Arduino_Library.h>  //Click here to get the library: http://librarymanager/All#SparkFun_FS3000
#include <LiquidCrystal.h>
#include "LEDCode.h"

const int RPIN = 13;
const int GPIN = 11;
const int BPIN = 12;
const int APIN = 10;
int numOfPpl = 0;

// Colors
uint8_t* currentColor;
uint8_t* newColor;
//uint8_t stage0[] = {0, 255, 0, 100};
//uint8_t stage1[] = {128, 128, 0, 100};
//uint8_t stage2[] = {255, 0, 0, 100};
//uint8_t stage3[] = {128, 0, 128, 100};
//uint8_t stage4[] = {0, 0, 255, 100};
//uint8_t stage5[] = {255, 255, 255, 100};

uint8_t stage0[] = {254, 240, 1, 0};
uint8_t stage1[] = {255, 206, 3, 0};
uint8_t stage2[] = {253, 154, 1, 0};
uint8_t stage3[] = {253, 97, 4, 0};
uint8_t stage4[] = {255, 44, 5, 0};
uint8_t stage5[] = {240, 5, 5, 0};

void updateLedStrip();

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  LEDSetup(RPIN, GPIN, BPIN, APIN, true); // uncomment if using waterproof strip
  updateLedStrip();

}

void loop() {
  updateLedStrip();
  delay(10000);
  numOfPpl = (numOfPpl + 1) % 6;
  Serial.println("Changing to... " + (String) numOfPpl);
}

void updateLedStrip(){
  int delayTime = 10;
  int steps = 200;
  switch (numOfPpl){
    case 0:
      newColor = stage0;
      break;
    case 1:
      newColor = stage1;
      break;
    case 2:
      newColor = stage2;
      break;
    case 3:
      newColor = stage3;
      break;
    case 4:
      newColor = stage4;
      break;
    case 5:
      newColor = stage5;
      break;
    default:
      newColor = stage0;
      break;
  }
  LEDTransition(currentColor, newColor, steps, delayTime);
  currentColor = newColor;
}
