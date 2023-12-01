// ARDUINO PIN SETUP
// NOTE: ALL OF THESE PINS NEED TO HAVE PWM CAPABILITIES SINCE WE ARE USING DIGITAL OUTS
#define BLUEPIN 3
#define REDPIN 5
#define GREENPIN 6
#define ALPHAPIN 9

// LIGHTING
#define FADESPEED 5 // rate at which each color fades
#define LIGHTS_TOGGLE true // set to false (0) when you want lights off on upload
#define MOCK_RED_CAP 256 // testing

// reference code from this page:
// https://learn.adafruit.com/rgb-led-strips/arduino-code

// initial pins
void whiteCode();

void setup() {
  pinMode(BLUEPIN, OUTPUT);
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  // uncomment these two lines when working with the waterproof strip (this will just test all 4 on at once)
  //pinMode(ALPHAPIN, OUTPUT); //(waterproof LED strip has an extra wire)
  //whiteCode();
}

/* Analog write with a LIGHTS_ON flag check*/
void toggleWrite(uint8_t pin, int val) {
  analogWrite(pin, val & LIGHTS_TOGGLE);
}

// Code for fade
void fadeCode() {
  int r, g, b, w;
 
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
  // fade from red to yellow
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
  // fade from teal to blue
  for (g = 255; g > 0; g--) { 
    toggleWrite(GREENPIN, g);
    delay(FADESPEED);
  } 
}

void whiteCode() {
  int r,g,b,w;
  for (r = 0; r < 256; r++) { 
    toggleWrite(REDPIN, r);
    delay(FADESPEED);
  }
  for (b = 0; b < 256; b++) { 
    toggleWrite(BLUEPIN, b);
    delay(FADESPEED);
  }
  for (g = 0; g < 256; g++) { 
    toggleWrite(GREENPIN, g);
    delay(FADESPEED);
  }  
  for(w = 0; w < 256; w++){
    toggleWrite(ALPHAPIN, w);
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
  // fadeCode();
  // whiteCode();
}
