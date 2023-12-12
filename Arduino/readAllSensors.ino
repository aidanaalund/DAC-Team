//// Libraries
#include <Adafruit_SCD30.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SparkFun_FS3000_Arduino_Library.h> //Click here to get the library: http://librarymanager/All#SparkFun_FS3000

//// Define Pins/Addresses
#define TCAADDR 0x70   // Multiplexer I2C address
#define ONE_WIRE_BUS 7 // Temperature probes data pin

OneWire oneWire(ONE_WIRE_BUS);       // initialize oneWire instance
DallasTemperature sensors(&oneWire); // pass oneWire reference to DallasTemperature

Adafruit_SCD30 scd30_1; // Define each CO2 sensor
Adafruit_SCD30 scd30_2;
FS3000 fs;
uint8_t sensor1[8] = {0x28, 0x66, 0x99, 0x94, 0x97, 0xFF, 0x03, 0xF5}; // Addresses of 3 DS18B20s
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

//// Setup
void setup(void)
{
  Serial.begin(115200);
  sensors.begin(); // initialize temp sensors
  while (!Serial)
    delay(10); // will pause until serial console opens

  Serial.println("All Sensors Initializing!");

  Wire.begin();

  // INLET SETUP
  tcaselect(4); // TCA channel for bme1
  if (!scd30_1.begin())
  { // Try to initialize!
    Serial.println("Failed to find SCD30 #1 chip");
    while (1)
    {
      delay(10);
    }
  }
  Serial.println("SCD30 #1 Found!");
  Serial.print("Measurement Interval: ");
  Serial.print(scd30_1.getMeasurementInterval());
  Serial.println(" seconds");

  // OUTLET SETUP
  tcaselect(7); // TCA channel for bme1
  if (!scd30_2.begin())
  { // Try to initialize!
    Serial.println("Failed to find SCD30 #2 chip");
    while (1)
    {
      delay(10);
    }
  }
  Serial.println("SCD30 #2 Found!");
  Serial.print("Measurement Interval: ");
  Serial.print(scd30_2.getMeasurementInterval());
  Serial.println(" seconds");

  // FLOW SENSORS
  tcaselect(2);
  if (fs.begin() == false) // Begin communication over I2C
  {
    Serial.println("The sensor did not respond. Please check wiring.");
    while (1)
      ; // Freeze
  }

  // Set the range to match which version of the sensor you are using.
  //  FS3000-1005 (0-7.23 m/sec) --->>>  AIRFLOW_RANGE_7_MPS
  // FS3000-1015 (0-15 m/sec)   --->>>  AIRFLOW_RANGE_15_MPS
  fs.setRange(AIRFLOW_RANGE_7_MPS);
  // fs.setRange(AIRFLOW_RANGE_15_MPS);

  Serial.println("Sensor is connected properly.");
}

//// LOOP
void loop()
{
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

    // FLOW
    tcaselect(2);
    Serial.println("");
    Serial.println("AIRFLOW");
    Serial.print("Speed: ");
    double Meas1 = fs.readMetersPerSecond();
    delay(200);
    double Meas2 = fs.readMetersPerSecond();
    delay(200);
    double Meas3 = fs.readMetersPerSecond();
    delay(200);
    double Meas4 = fs.readMetersPerSecond();
    double Avg = (Meas1 + Meas2 + Meas3 + Meas4) / 4;
    Serial.print(Avg); // note, this returns a float from 0-7.23 for the FS3000-1005, and 0-15 for the FS3000-1015
    Serial.println(" m/s");

    Serial.println("----------------------------");
  }
  delay(2000);
}