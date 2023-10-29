// Define the pins for the electrical devices
const int V1 = 2;   // top valve
const int V2 = 3;   // bottom valve
const int V3 = 4;   // to algae
const int F1 = 5;   // top fan
const int F2 = 6;   // fan 2 (to algae feeder & vacuum pump)
const int P1 = 7;   // vacuum pump
const int Heat = 8; // heat

//// Libraries
#include <Adafruit_SCD30.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PID_v1.h>
#include <Wire.h>
#include <SparkFun_FS3000_Arduino_Library.h> //Click here to get the library: http://librarymanager/All#SparkFun_FS3000

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

// Define the current case (initialized to 'O' for off)
// These are the keys that will change the if statements under CASE_INTERVAL
String CurrentCase = "O";
String NewCase = "O";

void setup()
{
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

  // Flow setup
  Serial.begin(115200);
  Wire.begin();
  fs.setRange(AIRFLOW_RANGE_7_MPS);

  // Begin serial communication
  Serial.begin(115200);
  sensors.begin(); // initialize temp sensors
  while (!Serial)
    delay(10); // will pause until serial console opens

  Serial.println("All Sensors Initializing!");

  Wire.begin();

  // INLET SETUP for sensors
  tcaselect(4); // TCA channel for bme1
  if (!scd30_1.begin())
  { // Try to initialize!
    Serial.println("Failed to find SCD30 #1 chip");
    //    while (1) {
    //      delay(10);
    //    }
  }
  Serial.println("SCD30 #1 Found!");
  Serial.print("Measurement Interval: ");
  Serial.print(scd30_1.getMeasurementInterval());
  Serial.println(" seconds");

  // OUTLET SETUP for sensors
  tcaselect(7); // TCA channel for bme1
  if (!scd30_2.begin())
  { // Try to initialize!
    Serial.println("Failed to find SCD30 #2 chip");
    //    while (1) {
    //      delay(10);
    //    }
  }
  Serial.println("SCD30 #2 Found!");
  Serial.print("Measurement Interval: ");
  Serial.print(scd30_2.getMeasurementInterval());
  Serial.println(" seconds");
  Serial.println("A = Adsorption");
  Serial.println("D = Desorption");
  Serial.println("H = Heating");
  Serial.println("O = Off");
  // This should be our serial monitor setup
}

void loop()
{
  // Check the user input every CASE_INTERVAL milliseconds
  if (millis() - lastCaseTime >= CASE_INTERVAL)
  {
    lastCaseTime = millis();
    String NewCase = Serial.readString();

    // delete this if statement if things are fucking up. Likely not needed.  This is really best instituted with case function and the case function can skip
    // all unnecessary if statements (i.e. if NewCase isn;t A D H or O we don't need to run the if comparators)
    if (NewCase == "A" || NewCase == "D" || NewCase == "H" || NewCase == "O")
    {
      CurrentCase = NewCase;
    }

    if (CurrentCase == "A")
    {                         // Adsorb
      digitalWrite(V1, HIGH); // top valve open
      digitalWrite(V2, HIGH); // bottom valve open
      digitalWrite(F1, HIGH); // fan 1 on
      digitalWrite(V3, LOW);  // close to algae
      digitalWrite(F2, LOW);  // fan 2 off
      digitalWrite(P1, LOW);  // pump off
    }
    else if (CurrentCase == "D")
    {                         // Desorb
      digitalWrite(V1, HIGH); // top valve open
      digitalWrite(V2, LOW);  // bottom valve close
      digitalWrite(V3, HIGH); // open to algae
      digitalWrite(F1, HIGH); // fan 1 on
      digitalWrite(F2, HIGH); // fan 2 on
      digitalWrite(P1, HIGH); // pump on
    }
    else if (CurrentCase == "H")
    {                        // Heat
      digitalWrite(V1, LOW); // everything closed
      digitalWrite(V2, LOW);
      digitalWrite(V3, LOW);
      digitalWrite(F1, LOW); // fans off
      digitalWrite(F2, LOW);
      digitalWrite(P1, LOW); // pump off

      Input = scd30_1.temperature; // read temperature from sensor
      myPID.Compute();             // calculate PID output
      if (Output > 127)            // what/where is the output from? global?
      {
        digitalWrite(Heat, HIGH); // turn Heat output fully on
      }
      else
      {
        digitalWrite(Heat, LOW); // turn Heat output fully off
      }
    }
    else if (CurrentCase == "O") // off = cooling
    {
      digitalWrite(V1, LOW); // valves closed
      digitalWrite(V2, LOW);
      digitalWrite(V3, LOW);
      digitalWrite(F1, LOW); // fans off
      digitalWrite(F2, LOW);
      digitalWrite(P1, LOW);   // pump off
      digitalWrite(Heat, LOW); // heat off
    }
    else
    {
      Serial.print("Continuing with ");
      Serial.print(CurrentCase);
    }
  }

  // Read the sensor inputs every SENSOR_INTERVAL milliseconds.
  if (millis() - lastSensorTime >= SENSOR_INTERVAL)
  {
    lastSensorTime = millis();
    // INLET
    tcaselect(4);
    if (scd30_1.dataReady())
    {
      Serial.println("INLET");
      if (!scd30_1.read())
      {
        Serial.println("Error reading sensor data");
        return;
      }

      Serial.print("Temperature: ");
      Serial.print(scd30_1.temperature);
      Serial.println(" degrees C");

      Serial.print("Relative Humidity: ");
      Serial.print(scd30_1.relative_humidity);
      Serial.println(" %");

      Serial.print("CO2: ");
      Serial.print(scd30_1.CO2, 3);
      Serial.println(" ppm");
      Serial.println("");

      Serial.print("\tm/s: ");
      double Meas1 = fs.readMetersPerSecond();
      delay(200);
      double Meas2 = fs.readMetersPerSecond();
      delay(200);
      double Meas3 = fs.readMetersPerSecond();
      delay(200);
      double Meas4 = fs.readMetersPerSecond();
      double Avg = (Meas1 + Meas2 + Meas3 + Meas4) / 4;
      Serial.println(Avg);
    }

    // OUTLET
    tcaselect(7);
    if (scd30_2.dataReady())
    {
      Serial.println("OUTLET");
      if (!scd30_2.read())
      {
        Serial.println("Error reading sensor data");
        return;
      }

      Serial.print("Temperature: ");
      Serial.print(scd30_2.temperature);
      Serial.println(" degrees C");

      Serial.print("Relative Humidity: ");
      Serial.print(scd30_2.relative_humidity);
      Serial.println(" %");

      Serial.print("CO2: ");
      Serial.print(scd30_2.CO2, 3);
      Serial.println(" ppm");
      Serial.println("");

      // TEMPERATURES
      Serial.println("TEMPERATURES");
      sensors.requestTemperatures();
      Serial.print("Sensor 1: ");
      printTemperature(sensor1);
      Serial.print("Sensor 2: ");
      printTemperature(sensor2);
      Serial.print("Sensor 3: ");
      printTemperature(sensor3);

      Serial.print("\tm/s: ");
      double Meas1 = fs.readMetersPerSecond();
      delay(200);
      double Meas2 = fs.readMetersPerSecond();
      delay(200);
      double Meas3 = fs.readMetersPerSecond();
      delay(200);
      double Meas4 = fs.readMetersPerSecond();
      double Avg = (Meas1 + Meas2 + Meas3 + Meas4) / 4;
      Serial.println(Avg);

      Serial.println("----------------------------");
    }
  }

  delay(10000) // I'm just scared of running this nonstop, so I let computer take 10s breaks
      ;
}

////This could replace the if else if case selection to speed things up
// switch (menuChoice) {
//     case 1:
//       // temp sensor code goes here
//       Serial.print("The temperature is: ");
//       Serial.println(temp);
//       break;
//
//     case 2:
//       // humidity sensor code goes here
//       Serial.print("The humidity is: ");
//       Serial.println(Rh);
//       break;
//
//     case 3:
//       // pressure sensor code goes here
//       Serial.print("The barometric pressure is: ");
//       Serial.println(pressure);
//       break;
//
//     default:
//       Serial.println("Please choose a valid selection");
//   }
//
