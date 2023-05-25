/***************************************************
  This is an example for the Adafruit Thermocouple Sensor w/MAX31855K

  Designed specifically to work with the Adafruit Thermocouple Sensor
  ----> https://www.adafruit.com/products/269

  These displays use SPI to communicate, 3 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution

  Modified for the NET Project Automation Team at UT Austin
 ****************************************************/
#include <SPI.h>
#include "Adafruit_MAX31855.h"

// Default connection is using software SPI, but comment and uncomment one of
// the two examples below to switch between software SPI and hardware SPI:

// Example creating a thermocouple instance with software SPI on any three
// digital IO pins.
// change here when running on MEGA
#define MAXDO1   3
#define MAXCS1   4
#define MAXCLK1  5
#define MAXDO2   6
#define MAXCS2   7
#define MAXCLK2  8
#define MAXDO3   9
#define MAXCS3   10
#define MAXCLK3  11
unsigned long startMillis;
unsigned long currentMillis;

// initialize the Thermocouple
Adafruit_MAX31855 thermocouple1(MAXCLK1, MAXCS1, MAXDO1);
Adafruit_MAX31855 thermocouple2(MAXCLK2, MAXCS2, MAXDO2);
Adafruit_MAX31855 thermocouple3(MAXCLK3, MAXCS3, MAXDO3);

// Example creating a thermocouple instance with hardware SPI
// on a given CS pin.
//#define MAXCS   10
//Adafruit_MAX31855 thermocouple(MAXCS);

// Example creating a thermocouple instance with hardware SPI
// on SPI1 using specified CS pin.
//#define MAXCS   10
//Adafruit_MAX31855 thermocouple(MAXCS, SPI1);

void setup() {
  startMillis = millis();
  Serial.begin(9600);

  while (!Serial) delay(1); // wait for Serial on Leonardo/Zero, etc

  // Serial.println("MAX31855 test");
  // wait for MAX chip to stabilize
  delay(500);
  // Serial.print("Initializing sensor...");
  if (!thermocouple1.begin() && !thermocouple2.begin() && !thermocouple3.begin()) {
    Serial.println("ERROR.");
    while (1) delay(10);
  }

  // OPTIONAL: Can configure fault checks as desired (default is ALL)
  // Multiple checks can be logically OR'd together.
  // thermocouple.setFaultChecks(MAX31855_FAULT_OPEN | MAX31855_FAULT_SHORT_VCC);  
  // short to GND fault is ignored

  // Serial.println("DONE.");
  Serial.println("Time(ms) Temperature1(C) Temperature2(C) Temperature3(C)");
}

void loop() {
  // basic readout test, just print the current temp
  //  Serial.print("Internal Temp = ");
  //  Serial.println(thermocouple.readInternal());
   currentMillis = millis();

   double c1 = thermocouple1.readCelsius();
   double c2 = thermocouple2.readCelsius();
   double c3 = thermocouple3.readCelsius();
   if (isnan(c1) && isnan(c2) && isnan(c3)) {
     Serial.println("Thermocouple fault(s) detected!");
     uint8_t e1 = thermocouple1.readError();
     uint8_t e2 = thermocouple2.readError();
     uint8_t e3 = thermocouple3.readError();
    //  if (e & MAX31855_FAULT_OPEN) Serial.println("FAULT: Thermocouple is open - no connections.");
    //  if (e & MAX31855_FAULT_SHORT_GND) Serial.println("FAULT: Thermocouple is short-circuited to GND.");
    //  if (e & MAX31855_FAULT_SHORT_VCC) Serial.println("FAULT: Thermocouple is short-circuited to VCC.");
   } else {
     Serial.print(currentMillis, 6);
     Serial.print(" ");
     Serial.print(c1);
     Serial.print(c2);
     Serial.println(c3);

   }
   //Serial.print("F = ");
   //Serial.println(thermocouple.readFahrenheit());

   delay(2000);
}