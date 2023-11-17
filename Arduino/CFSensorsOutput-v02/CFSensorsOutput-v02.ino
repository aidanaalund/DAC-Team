

//// Libraries
#include <Adafruit_SCD30.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PID_v1.h>
#include <Wire.h>
#include <SparkFun_FS3000_Arduino_Library.h>  //Click here to get the library: http://librarymanager/All#SparkFun_FS3000

// pin numbers for outputs
// TODO set pin numbers
#define V1 ;    // top valve
#define V2 ;    // bottom valve
#define V3 ;    // to algae
#define F1 ;    // top fan
#define F2 ;    // fan 2 (to algae feeder & vacuum pump)
#define P1 ;    // vacuum pump
#define Heat ;  // heat

// MUX select numbers
// TODO set channel numbers
#define co2in ;
#define co2out ;
#define flowIn ;
#define flowOut ;

/*
Current plan for automation:
unsure of plan until we meet the person who originally wrote the code.
Regardless we should make functions that correspond to each state of the machine.

Two approaches we could take to automate would be with switch cases, or an array of pointers to the functions for each state.
We could use the array because each stage happens sequentially so once one is done, we can incrememt do the next.

The DAC needs to start when 5 people are blowing. I thnk the easiest way to do this would be
if(numbOfPeople == 5){
    do one DAC cycle
    DAC goes back to resting state
}
and then numbOfPeople can be tracked in loop and we can calculate this with data from senors
*/

// functions for each state
// not sure if we will need all of these
boolean adsorbtion();
boolean heating();
boolean desoportion();
boolean cooling();
boolean restingState();
// function to check number of people blowing
boolean minNumbOfPpl();

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
FS3000 fs;                                                                // I have NO CLUE what this does
                                                                          // Addresses of 3 DS18B20s (thermocouples)
uint8_t sensor1[8] = { 0x28, 0x66, 0x99, 0x94, 0x97, 0xFF, 0x03, 0xF5 };  // Addresses of 3 DS18B20s
uint8_t sensor2[8] = { 0x28, 0xF9, 0x29, 0x94, 0x97, 0x0F, 0x03, 0xB6 };
uint8_t sensor3[8] = { 0x28, 0x3A, 0x5F, 0x94, 0x97, 0x03, 0x03, 0xCC };


// Heater PID Settings
double Setpoint, Input, Output;
PID myPID(&Input, &Output, &Setpoint, 2, 5, 1, DIRECT); // Kp, Ki, Kd values

//// Function for selecing MUX output channel
void tcaselect(uint8_t channel) {
  if (channel > 7)
    return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << channel);
  Wire.endTransmission();
}

//// Function for requesting and printing temperature
void printTemperature(DeviceAddress deviceAddress) {
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print(tempC);
  Serial.print(" ");
  Serial.print("C  |  ");
  Serial.print(DallasTemperature::toFahrenheit(tempC));
  Serial.print(" ");
  Serial.println("F");
}

// function to get temp in Celcius
float getTemperatureC(DeviceAddress deviceAddress{
  return sensors.getTempC(deviceAddress);
// function to get temp in Fahrenheit
float getTemperatureF(DeviceAddress deviceAddress{
    float tempC = sensors.getTempC(deviceAddress);
  return  DallasTemperature::toFahrenheit(tempC));
}

  // Define the time intervals for the sensor readings and case checking
const unsigned long SENSOR_INTERVAL = 60000;  // 1 min
const unsigned long CASE_INTERVAL = 60000;      // 1 min

// Define the last time the sensors were read and the case was checked
unsigned long lastSensorTime = 0;
unsigned long lastCaseTime = 0;

void setup() {
    // Set pins for sensors to output
    // pinMode(sensorName, OUTPUT);

    // Set the initial states of the electrical devices
    // low == close/off
    // high == open/on
    // digitalWrite(sensorName, state);

    // Set the pin modes for the electrical devices
    pinMode(V1, OUTPUT);
    pinMode(V2, OUTPUT);
    pinMode(V3, OUTPUT);
    pinMode(F1, OUTPUT);
    pinMode(F2, OUTPUT);
    pinMode(P1, OUTPUT);
    pinMode(Heat, OUTPUT);

    // Set the initial states of the electrical devices
    // low = close
    digitalWrite(V1, LOW);
    digitalWrite(V2, LOW);
    digitalWrite(V3, LOW);
    // low = off
    digitalWrite(F1, LOW);
    digitalWrite(F2, LOW);
    digitalWrite(P1, LOW);
    digitalWrite(Heat, LOW);

    // Heater Controls
  Setpoint = 70.0;            // set desired temperature
  myPID.SetMode(AUTOMATIC);   // set PID to automatic mode
  myPID.SetSampleTime(60000); // set PID cycle time to 1 minute


    // copied from readAllSensors

    // begin serial communication
    Serial.begin(115200);
    sensors.begin();  // initialize temp sensors
    while (!Serial)
      delay(10);  // will pause until serial console opens

    Serial.println("All Sensors Initializing!");

    Wire.begin();

    // INLET SETUP
    tcaselect(4);            // TCA channel for bme1
    if (!scd30_1.begin()) {  // Try to initialize!
      Serial.println("Failed to find SCD30 #1 chip");
      while (1) {
        delay(10);
      }
    }
    Serial.println("SCD30 #1 Found!");
    Serial.print("Measurement Interval: ");
    Serial.print(scd30_1.getMeasurementInterval());
    Serial.println(" seconds");

    // OUTLET SETUP
    tcaselect(7);            // TCA channel for bme1
    if (!scd30_2.begin()) {  // Try to initialize!
      Serial.println("Failed to find SCD30 #2 chip");
      while (1) {
        delay(10);
      }
    }
    Serial.println("SCD30 #2 Found!");
    Serial.print("Measurement Interval: ");
    Serial.print(scd30_2.getMeasurementInterval());
    Serial.println(" seconds");

    // FLOW SENSORS
    tcaselect(2);
    if (fs.begin() == false)  // Begin communication over I2C
    {
      Serial.println("The sensor did not respond. Please check wiring.");
      while (1)
        ;  // Freeze
    }
    // Set the range to match which version of the sensor you are using.
  //  FS3000-1005 (0-7.23 m/sec) --->>>  AIRFLOW_RANGE_7_MPS
  // FS3000-1015 (0-15 m/sec)   --->>>  AIRFLOW_RANGE_15_MPS
  fs.setRange(AIRFLOW_RANGE_7_MPS);
  // fs.setRange(AIRFLOW_RANGE_15_MPS);

  Serial.println("Sensor is connected properly.");
}

    void loop() {
      if(minNumbOfPpl() /* ||  20 minutes passed*/){
        bool done = false;
        // use a do while instead
        while(!done){
          done = heating();
        }
        done = false;
        while (!done){
           done = desorption();
        }
           done = false;
        while(!done){
           done = cooling();
        }
      } else {
        adsorption();
      }
    }


// TODO: function to check if the correct number of people are blowing, return true when there is enough.
boolean minNumbOfPpl(){
  return false;
}

  // functions for each state
  /*
 current plan for each function is once called, it will run until it complete, once complete it returns true
*/
    // TODO:
    boolean adsorption() {
      digitalWrite(V1, HIGH); // top valve open
      digitalWrite(V2, HIGH); // bottom valve open
      digitalWrite(F1, HIGH); // fan 1 on
      digitalWrite(V3, LOW);  // close to algae
      digitalWrite(F2, LOW);  // fan 2 off
      digitalWrite(P1, LOW);  // pump off
      return true;
    }

    // TODO:
    boolean heating() {
      return true;
    }
    // TODO:
    // return true when 5 min passed. Use millis
    boolean desorption() {
      if(/* not 5 min passed*/){
      digitalWrite(V1, HIGH); // top valve open
      digitalWrite(V2, LOW);  // bottom valve close
      digitalWrite(V3, HIGH); // open to algae
      digitalWrite(F1, HIGH); // fan 1 on
      digitalWrite(F2, HIGH); // fan 2 on
      digitalWrite(P1, HIGH); // pump on
        return false;
      }
      return true;
    }
    // TODO:
    boolean cooling(){
      return true:
    }

    // TODO:
    boolean
    restingState() {
      return true;
    }
