//// Libraries
#include <Adafruit_SCD30.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PID_v1.h>
#include <Wire.h>
#include <SparkFun_FS3000_Arduino_Library.h>  //Click here to get the library: http://librarymanager/All#SparkFun_FS3000
#include <LiquidCrystal.h>
#include "LEDCode.h"


//// Define Pins/Addresses
#define TCAADDR 0x70
#define ONE_WIRE_BUS A1

// pins for RGB
const int RPIN = 13;
const int GPIN = 11;
const int BPIN = 12;
const int APIN = 10;

// pin numbers for outputs
const int valve1 = 2;
const int valve2 = 3;
const int valve3 = 4;
const int fan1 = 5;
const int fan2 = 6;
const int pump1 = 7;
const int heat = 8;

OneWire oneWire(ONE_WIRE_BUS);        // initialize oneWire instance
DallasTemperature sensors(&oneWire);  // pass oneWire reference to DallasTemperature

Adafruit_SCD30 scd30_1;  // Define each CO2 sensor
Adafruit_SCD30 scd30_2;
FS3000 flowSensor;  //Define FlowSensor
uint8_t sensor1[8] = { 0x28, 0x66, 0x99, 0x94, 0x97, 0xFF, 0x03, 0xF5 };  // Addresses of 3 DS18B20s
uint8_t sensor2[8] = { 0x28, 0xF9, 0x29, 0x94, 0x97, 0x0F, 0x03, 0xB6 };
uint8_t sensor3[8] = { 0x28, 0x3A, 0x5F, 0x94, 0x97, 0x03, 0x03, 0xCC };

// Heater PID Settings
double Setpoint, Input, Output;
PID myPID(&Input, &Output, &Setpoint, 2, 5, 1, DIRECT); // Kp, Ki, Kd values

// LCD interface pins
const int rs = 48, en = 49, d4 = 50, d5 = 51, d6 = 52, d7 = 53;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Function prototypes
void adsorption();
void desorption();
void restingState();
bool minNumbOfPpl();
void updateLedStrip();
void tcaSelect(uint8_t);
double getUsersMaxCO2();
double getAverageFlow();
double getUsersAverageFlow(double);
void printTemperature(DeviceAddress);
float getTemperatureC(DeviceAddress);
float getTemperatureF(DeviceAddress);
void displayMessage(String, String, int);
void displayTimeLeftOnLCD(unsigned long);
void displayUserBlowMessagesOnLCD(unsigned long, double);

enum State {
  ADSORPTION,
  DESORPTION
};

// Vars for LCD message display
String space = "                "; // 16 spaces to fill the LCD screen
String scrollMessageTop, scrollMessageBottom;

// track state of the machine
int currentState;
// track number of people blowing
static int numOfPpl = 0;
// flow rate threshold for when someone stopped blowing
const int FLOW_MIN_THRESHOLD = 2;
// flow rate threshold for it to count as someone blowing
const int FLOW_MAX_THRESHOLD = 1;
// number of people blown threshold
const int PEOPLE_THRESHOLD = 5;

// Timer
unsigned long desorptionStartTime;
const unsigned long desorptionDuration = 300000; // 5 minutes in milliseconds

// Colors
uint8_t* currentColor;
uint8_t* newColor;
//TODO Make these colors nicer
uint8_t stage0[] = {254, 240, 1, 0};
uint8_t stage1[] = {255, 206, 3, 0};
uint8_t stage2[] = {253, 154, 1, 0};
uint8_t stage3[] = {253, 97, 4, 0};
uint8_t stage4[] = {255, 44, 5, 0};
uint8_t stage5[] = {240, 5, 5, 0};

void setup() {
    // Set the pin modes for the electrical devices
    pinMode(valve1, OUTPUT);
    pinMode(valve2, OUTPUT);
    pinMode(valve3, OUTPUT);
    pinMode(fan1, OUTPUT);
    pinMode(fan2, OUTPUT);
    pinMode(pump1, OUTPUT);
    pinMode(heat, OUTPUT);

    // Set the initial states of the electrical devices
    currentState = ADSORPTION;
    currentColor = stage0;

    // Heater Controls (do this workie??? Who the hell knows)
    Setpoint = 70.0; // set desired temperature
    myPID.SetMode(AUTOMATIC);   
    myPID.SetSampleTime(60000); // 1 minute cycle time

    // Begin serial communication
    Serial.begin(115200);
    sensors.begin();  // initialize temp sensors
    while (!Serial){
      delay(10); 
    }

    Serial.println("All Sensors Initializing!");
    Wire.begin();

    // INLET SETUP
    tcaSelect(1);            // TCA channel for bme1
    if (!scd30_1.begin()) {
      Serial.println("Failed to find SCD30 #1 chip");
      while (1) {
        delay(10);
      }
    }

    Serial.println("SCD30 #1 Found!");
    Serial.print("Measurement Interval: "); Serial.print(scd30_1.getMeasurementInterval()); Serial.println(" seconds");

    // OUTLET SETUP
    tcaSelect(7);            // TCA channel for bme1
    if (!scd30_2.begin()) {
      Serial.println("Failed to find SCD30 #2 chip");
      while (1) {
        delay(10);
      }
    }
    Serial.println("SCD30 #2 Found!");
    Serial.print("Measurement Interval: "); Serial.print(scd30_2.getMeasurementInterval()); Serial.println(" seconds");


    // FLOW SENSORS
    tcaSelect(2);
    if (flowSensor.begin() == false){  // Begin communication over I2C
      Serial.println("The sensor did not respond. Please check wiring.");
      while (1);  // Freeze
    }

    // Set the range FS3000-1005 (0-7.23 m/sec) --->>>  AIRFLOW_RANGE_7_MPS
    flowSensor.setRange(AIRFLOW_RANGE_7_MPS);

    Serial.println("Sensor is connected properly.");

    // LED init
    LEDSetup(RPIN, GPIN, BPIN, APIN, true);
    updateLedStrip();
    
    //LCD Setup
    lcd.begin(16, 2);
}

void loop() {
  double currentFlow;
  unsigned long desorptionTimeLeft;
  switch (currentState) {
    case ADSORPTION:
      adsorption();
      currentFlow = getAverageFlow();
      if(currentFlow >= FLOW_MAX_THRESHOLD){ // when someone blows
//        digitalWrite(fan1, LOW);  //Comment this if you don't want fan to stop when user blows
        unsigned long flowStartTime = millis();
        double usersFlowRate = getUsersAverageFlow(currentFlow);
        unsigned long elapsedTime = millis() - flowStartTime;
        displayUserBlowMessagesOnLCD(elapsedTime, usersFlowRate);  // Print all the messages for user on LCD
//        digitalWrite(fan1, HIGH); //Comment this if you don't want fan to stop when user blows
        numOfPpl++;
        Serial.println((String)numOfPpl + " have blown");
        updateLedStrip();
      }
      if (minNumbOfPpl()) { // when 5 people have blown
        Serial.println("5 people blew!");
        currentState = DESORPTION;
        desorptionStartTime = millis(); // Start the timer
        numOfPpl = 0; // Reset count
      }
      break;   
    case DESORPTION:
      desorption();
      Serial.println("TEMPERATURES");
      sensors.requestTemperatures();
      Serial.print("Sensor 2: ");
      printTemperature(sensor2);
      Serial.print("Sensor 3: ");
      printTemperature(sensor3);
      currentFlow = getAverageFlow();
      if(currentFlow >= FLOW_MAX_THRESHOLD){ // when someone blows during desorption
        desorptionTimeLeft = desorptionDuration - (millis() - desorptionStartTime);
        displayTimeLeftOnLCD(desorptionTimeLeft);  // Print desorption time left on LCD
      }
      if (millis() - desorptionStartTime >= desorptionDuration) {
        Serial.println("Exiting Desorption!");
        currentState = ADSORPTION;
        updateLedStrip();
      }
      break;
    default:
      break;
  }
}

double getUsersMaxCO2(){
  tcaSelect(1);
  double highestFlow = 20000.0/1000000.0;
  double breathConc;
  double maxI = 12; // Number of times loop should run. maxI * 5 = seconds to measure CO2
  for(int i = 0; i < maxI; i++){
    if(scd30_1.dataReady() && scd30_1.read()){
      breathConc = scd30_1.CO2/1000000.0;
      Serial.println(scd30_1.CO2);
      Serial.println(breathConc);
      if(breathConc > highestFlow){
        highestFlow = breathConc;
        Serial.println("New Highest: " + (String)highestFlow);
      }
    }
    randomSeed(analogRead(0));
    int randomNumber = random(1, 11);
    if(i == maxI - 2 && randomNumber == 10){
      displayMessage("Still working", "on the CO2 ;)", 1000);//Delay of like 5 seconds and some
    }
    else{
      displayMessage("Calculating CO2", "Produced...", 1000);//Delay of like 5 seconds and some
    }
  }
  return highestFlow;
}

void displayUserBlowMessagesOnLCD(unsigned long elapsedTime, double usersFlowRate){
  if (elapsedTime == 0 || isnan(elapsedTime) || usersFlowRate == 0 || isnan(usersFlowRate)){  //Invalid values
    return;
  }
//  double breathConc = getUsersMaxCO2();
  double breathConc = 0.04;
  float tubeArea = 0.00064516;
  float densityCO2 = 1977;
  float carbonEmitted = (elapsedTime/1000)*usersFlowRate*tubeArea*densityCO2*breathConc;
  carbonEmitted = round(carbonEmitted * 100.0) / 100.0;
  String topMessage = "Carbon Emitted: ";
  String bottomMessage = (String)carbonEmitted + " grams";
  displayMessage(topMessage, bottomMessage, 5000);

//  Example of how this works (luv u bb - julian)
//  topMessage = "Algea Produced: ";
//  bottomMessage = "testalgea";
//  int delayTime = 5000; //5 seconds
//  displayMessage(topMessage, bottomMessage, delayTime);
}

void displayTimeLeftOnLCD(unsigned long desorptionTimeLeft){
  unsigned long desorptionTimeLeftSeconds = desorptionTimeLeft/1000;
  unsigned int minutesLeft = desorptionTimeLeftSeconds / 60;
  unsigned int secondsLeft = desorptionTimeLeftSeconds % 60;
  String topMessage = "Desorbing, wait" + space;
  String bottomMessage = (String)minutesLeft + ":" + (String)secondsLeft + " minutes" + space;
  displayMessage(topMessage, bottomMessage, 5000);
}

void displayMessage(String topMessage, String bottomMessage, int delayTime){
  scrollMessageTop = topMessage + space;
  scrollMessageBottom = bottomMessage + space;
  for (int i = 0; i <= scrollMessageTop.length() - 16; i++) {
    lcd.clear(); // Clear the display
    // Display a portion of the top message
    lcd.setCursor(0, 0);
    lcd.print(scrollMessageTop.substring(i, i + 16));
    // Display a portion of the bottom message
    lcd.setCursor(0, 1);
    lcd.print(scrollMessageBottom.substring(i, i + 16));
    if (i == 0) {
      delay(delayTime); // Pause for 3 seconds when the full message is displayed
    } else {
      delay(300); // Adjust delay for scrolling speed
    }
  }
}

double getUsersAverageFlow(double currentFlow){
  double numOfFlowReadings = 0;
  double sumOfFlowReadings = 0;
  while(currentFlow > FLOW_MIN_THRESHOLD){  //Wait for person to stop blowing (or a timer or something)
    sumOfFlowReadings += currentFlow;
    numOfFlowReadings++;
    currentFlow = getAverageFlow();
    // Add extra delay if needed
  }
  return sumOfFlowReadings / numOfFlowReadings;
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

double getAverageFlow(){
  tcaSelect(2);
  double Meas1 = flowSensor.readMetersPerSecond();
  delay(200);
  double Meas2 = flowSensor.readMetersPerSecond();
  delay(200);
  double Meas3 = flowSensor.readMetersPerSecond();
  delay(200);
  double Meas4 = flowSensor.readMetersPerSecond();
  double mean = (Meas1 + Meas2 + Meas3 + Meas4) / 4;
  Serial.println("Flow: " + (String)mean);
  return mean;
}

bool minNumbOfPpl(){
  return numOfPpl >= PEOPLE_THRESHOLD;
}

void adsorption() {
  digitalWrite(valve1, HIGH);
  digitalWrite(valve2, HIGH);
  digitalWrite(valve3, LOW);
  digitalWrite(fan1, HIGH);
  digitalWrite(fan2, LOW);
  digitalWrite(pump1, LOW);
  digitalWrite(heat, LOW);
}

void desorption() {
  digitalWrite(valve1, HIGH);
  digitalWrite(valve2, LOW);
  digitalWrite(valve3, HIGH);
  digitalWrite(fan1, HIGH);
  digitalWrite(fan2, HIGH);
  digitalWrite(pump1, HIGH);
  analogWrite(pump1, 175);
  digitalWrite(heat, HIGH); //Heating
}

void restingState(){
  digitalWrite(valve1, LOW);
  digitalWrite(valve2, LOW);
  digitalWrite(valve3, LOW);
  digitalWrite(fan1, LOW);
  digitalWrite(fan2, LOW);
  digitalWrite(pump1, LOW);
  digitalWrite(heat, LOW);
}

// Select MUX output channel
void tcaSelect(uint8_t channel) {
  if (channel > 7)
    return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << channel);
  Wire.endTransmission();
}

void printTemperature(DeviceAddress deviceAddress) {
  Serial.print(getTemperatureC(deviceAddress));
  Serial.print(" ");
  Serial.print("C  |  ");
  Serial.print(getTemperatureF(deviceAddress));
  Serial.print(" ");
  Serial.println("F");
}

float getTemperatureC(DeviceAddress deviceAddress){
  return sensors.getTempC(deviceAddress);
}

float getTemperatureF(DeviceAddress deviceAddress){
  float tempC = sensors.getTempC(deviceAddress);
  return DallasTemperature::toFahrenheit(tempC);
}
