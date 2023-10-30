

//// Libraries
#include <Adafruit_SCD30.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PID_v1.h>
#include <Wire.h>
#include <SparkFun_FS3000_Arduino_Library.h> //Click here to get the library: http://librarymanager/All#SparkFun_FS3000

/*
Mockup/wip of planned code for the museum
TODO:
on/off switch
array of functions

*/

/*
I'm asumming the the pins will change with the arduino mega.
*/

// functions for each state
boolean adsorbtion();
boolean desoportion();
boolean restingState();

// array of the states

// Define the pins for the electrical devices
const int V1;       // top valve
const int V2 = 3;   // bottom valve
const int V3 = 4;   // to algae
const int F1 = 5;   // top fan
const int F2 = 6;   // fan 2 (to algae feeder & vacuum pump)
const int P1 = 7;   // vacuum pump
const int Heat = 8; // heat
// int to track state of the machine. used to index into array of machine states
static int currentState = 0;

FS3000 fs;

// Heater PID Settings
double Setpoint, Input, Output;
PID myPID(&Input, &Output, &Setpoint, 2, 5, 1, DIRECT); // Kp, Ki, Kd values

//// Define Pins/Addresses
#define TCAADDR 0x70    // Multiplexer I2C address
#define ONE_WIRE_BUS 13 // Temperature probes data pin

OneWire oneWire(ONE_WIRE_BUS);       // initialize oneWire instance
DallasTemperature sensors(&oneWire); // pass oneWire reference to DallasTemperature

Adafruit_SCD30 scd30_1; // Define each CO2 sensor
Adafruit_SCD30 scd30_2;
uint8_t sensor1[8] = {0x28, 0x66, 0x99, 0x94, 0x97, 0xFF, 0x03, 0xF5}; // Addresses of 3 DS18B20s (temp sensor ?)
uint8_t sensor2[8] = {0x28, 0xF9, 0x29, 0x94, 0x97, 0x0F, 0x03, 0xB6};
uint8_t sensor3[8] = {0x28, 0x3A, 0x5F, 0x94, 0x97, 0x03, 0x03, 0xCC};

//// Function for selecing MUX output channel
void tcaselect(uint8_t channel)
{
    if (channel > 7)
        return;
    Wire.beginTransmission(TCAADDR);
    Wire.write(1 << channel);
    Wire.endTransmission();
}

//// Function for requesting and printing temperature
void printTemperature(DeviceAddress deviceAddress)
{
    float tempC = sensors.getTempC(deviceAddress);
    Serial.print(tempC);
    Serial.print(" ");
    Serial.print("C  |  ");
    Serial.print(DallasTemperature::toFahrenheit(tempC));
    Serial.print(" ");
    Serial.println("F");
}

// Define the time intervals for the sensor readings and case checking
const unsigned long SENSOR_INTERVAL = 60000; // 1 min
const unsigned long CASE_INTERVAL = 60000;   // 1 min

// Define the last time the sensors were read and the case was checked
unsigned long lastSensorTime = 0;
unsigned long lastCaseTime = 0;

void setup()
{
    // Set pins for sensors to output
    // pinMode(sensorName, OUTPUT);

    // Set the initial states of the electrical devices
    // low == close/off
    // high == open/on
    // digitalWrite(sensorName, state);

    // Heater Controls
}

void loop()
{
}

// functions for each state
boolean adsorbtion()
{
    return true;
}

boolean desoportion()
{
    return true;
}

boolean restingState()
{
    return true;
}
