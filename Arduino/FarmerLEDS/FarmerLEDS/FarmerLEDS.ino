// ARDUINO PIN SETUP
#define BLUEPIN 3
#define REDPIN 5
#define GREENPIN 6

// LIGHTING
#define FADESPEED 5 // rate at which each color fades
#define LIGHTS_TOGGLE true // set to false (0) when you want lights off on upload

// reference code from this page:
// https://learn.adafruit.com/rgb-led-strips/arduino-code

// initial pins
void setup() {
  pinMode(BLUEPIN, OUTPUT);
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  // pinMode(ALPHAPIN, OUTPUT); (waterproof LED strip has an extra wire)
}

/* Analog write with a LIGHTS_ON flag check*/
void toggleWrite(uint8_t pin, int val) {
  analogWrite(pin, val & LIGHTS_TOGGLE)
}

// Code for fade
void fadeCode() {
  int r, g, b;
 
  // fade from blue to violet
  for (r = 0; r < 256; r++) { 
    toggleWrite(REDPIN, r);
    delay(FADESPEED);
  } 
  // fade from violet to red
  for (b = 255; b > 0; b--) { 
    toggleWrite(BLUEPIN, b);
    delay(FADESPEED);
  }
  //fade from red to yellow
  for (g = 0; g < 256; g++) { 
    toggleWrite(GREENPIN, g);
    delay(FADESPEED);
  } 
  // fade from yellow to green
  for (r = 255; r > 0; r--) { 
    toggleWrite(REDPIN, r);
    delay(FADESPEED);
  } 
  // fade from green to teal
  for (b = 0; b < 256; b++) { 
    toggleWrite(BLUEPIN, b);
    delay(FADESPEED);
  } 
  fade from teal to blue
  for (g = 255; g > 0; g--) { 
    toggleWrite(GREENPIN, g);
    delay(FADESPEED);
  } 
}

// Yellow to Pure Green
void LEDCode() {
  int r, g, b;

  // Whatever
  for (r = MOCK_RED_CAP; r > 0; r--) {
    toggleWrite(REDPIN, r);
    delay(FADESPEED);
  }

  for (r = 0; r < MOCK_RED_CAP; r++) {
    toggleWrite(REDPIN, r);
    delay(FADESPEED);
  }
}

void loop() {
  fadeCode();
}
