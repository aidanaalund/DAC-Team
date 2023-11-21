//// Libraries
#include <Adafruit_SCD30.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PID_v1.h>
#include <Wire.h>
#include <SparkFun_FS3000_Arduino_Library.h>  //Click here to get the library: http://librarymanager/All#SparkFun_FS3000

// pin numbers for outputs
// TODO set pin numbers
#define valve1 ;    // top valve
#define valve2 ;    // bottom valve
#define valve3 ;    // to algae
#define fan1 ;    // top fan
#define fan2 ;    // fan 2 (to algae feeder & vacuum pump)
#define pump1 ;    // vacuum pump
#define heat ;  // heat

// MUX select numbers
// TODO set channel numbers
#define co2in ;
#define co2out ;
#define flowIn ;
#define flowOut ;

// Function prototypes
void adsorption();
void desorption();
void restingState();
boolean hasFiveMinsElapsed()
boolean minNumbOfPpl();
void tcaSelect(uint8_t);
void printTemperature(DeviceAddress);
float getTemperatureC(DeviceAddress);
float getTemperatureF(DeviceAddress);

// int to track state of the machine. used to index into array of machine states
static int currentState = 0;
// int to track number of people blowing
static int numbOfPpl = 0;

//// Define Pins/Addresses
#define TCAADDR 0x70  // Multiplexer I2C address
// TODO set pin number
#define ONE_WIRE_BUS  // Temperature probes data pin

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

// Timer
unsigned long previousMillis = 0; // Last time timer was updated
const unsigned long INTERVAL = 300000; // 5 minute interval

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
    restingState()

    // Heater Controls
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
    tcaSelect(4);            // TCA channel for bme1
    if (!scd30_1.begin()) {  // Try to initialize!
      Serial.println("Failed to find SCD30 #1 chip");
      while (1) {
        delay(10);
      }
    }

    Serial.println("SCD30 #1 Found!");
    Serial.print("Measurement Interval: "); Serial.print(scd30_1.getMeasurementInterval()); Serial.println(" seconds");

    // OUTLET SETUP
    tcaSelect(7);            // TCA channel for bme1
    if (!scd30_2.begin()) {  // Try to initialize!
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

    // Set the range to match which version of the sensor you are using.
    // FS3000-1005 (0-7.23 m/sec) --->>>  AIRFLOW_RANGE_7_MPS
    // FS3000-1015 (0-15 m/sec)   --->>>  AIRFLOW_RANGE_15_MPS
    flowSensor.setRange(AIRFLOW_RANGE_7_MPS);

    Serial.println("Sensor is connected properly.");
}

void loop() {
  if(minNumberofPpl() && !hasIntervalElapsed()){
    
    desorption();
  }
  else{
    adsorption();
  }
}

//Need work on timer functionality
//Min Number of people functionality
//Flow counting functionality

void hasIntervalElapsed(){
  unsigned long currentMillis = millis();  // Get the current time
  if (currentMillis - previousMillis >= INTERVAL){
    previousMillis = currentMillis;
    return true;
  }
  return false;
}



void minNumbOfPpl(){
  return false;
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
  digitalWrite(heat, LOW);
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
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print(tempC);
  Serial.print(" ");
  Serial.print("C  |  ");
  Serial.print(DallasTemperature::toFahrenheit(tempC));
  Serial.print(" ");
  Serial.println("F");
}

float getTemperatureC(DeviceAddress deviceAddress){
  return sensors.getTempC(deviceAddress);
}

float getTemperatureF(DeviceAddress deviceAddress){
  float tempC = sensors.getTempC(deviceAddress);
  return DallasTemperature::toFahrenheit(tempC));
}