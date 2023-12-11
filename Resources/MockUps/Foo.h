#pragma once
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <Arduino.h>

#include <iostream>
#include <cmath>

#pragma once
#include <iostream>
#include <cmath>

namespace lalg {
    //
    // VECTORS
    //
    struct vec4 { float r, g, b, a; };
    struct ivec4 { int r, g, b, a; };

    // type conversion
    vec4 to_vec4(const ivec4& v) { return { (float)v.r, (float)v.g, (float)v.b, (float)v.a }; }
    ivec4 to_ivec4(const vec4& v) { return { (int)v.r, (int)v.g, (int)v.b, (int)v.a }; }

    // vector operations
    vec4 operator+(const vec4& v1, const vec4& v2) { return { v1.r + v2.r, v1.g + v2.g, v1.b + v2.b, v1.a + v2.a }; }
    vec4 operator-(const vec4& v1, const vec4& v2) { return { v1.r - v2.r, v1.g - v2.g, v1.b - v2.b, v1.a - v2.a }; }
    vec4 operator*(const float scalar, const vec4& v) { return { scalar * v.r, scalar * v.g, scalar * v.b, scalar * v.a }; }
    vec4 operator*(const vec4& v, const float scalar) { return scalar * v; }
    vec4 operator/(const vec4& v, const float scalar) { return { v.r / scalar, v.g / scalar, v.b / scalar, v.a / scalar }; }
    vec4 operator/(const float scalar, const vec4& v) { return scalar * v; }

    // vector functions
    float magnitude(const vec4& v) { return sqrt(v.r * v.r + v.g * v.g + v.b * v.b + v.a * v.a); }
    vec4 cross(const vec4& v1, const vec4& v2) { return { v1.g * v2.b - v1.b * v2.g, v1.b * v2.r - v1.r * v2.b, v1.r * v2.g - v1.g * v2.r, 0 }; }
    vec4 normalize(const vec4& v) { return v / magnitude(v); }
    float dot(const vec4& v1, const vec4& v2) { return v1.r * v2.r + v1.g * v2.g + v1.b * v2.b + v1.a * v2.a; }
    void printVec(const vec4& v) { std::printf("[%f, %f, %f, %f]\n", v.r, v.g, v.b, v.a); }

    //
    // MATRICES
    //
    struct mat4 { vec4 r1, r2, r3, r4; };

    // get a 0-indexed column from mat4
    vec4 getColumn(const mat4& m, const uint8_t column) {
        if (column > 3) return { 0, 0, 0, 0 }; // need some placeholder error thing
        vec4* cVec = (vec4*)((float*)&m + column);
        return { cVec->r, (cVec + 1)->r, (cVec + 2)->r, (cVec + 3)->r };
    }

    // modify the passed in matrix, and set a 0-indexed column equal to the specifed vector
    void setColumn(const mat4& m, const vec4& v, const uint8_t column) {
        if (column <= 3) {
            vec4* cVec = (vec4*)((float*)&m + column);
            cVec->r = v.r;
            (cVec + 1)->r = v.g;
            (cVec + 2)->r = v.b;
            (cVec + 3)->r = v.a;
        }
    }

    // convert rows to columns
    mat4 transpose(const mat4& m) { return { getColumn(m,0), getColumn(m,1), getColumn(m,2), getColumn(m,3) }; }

    // returns a translation matrix (we're using the alpha channel as our homogenous coordinate)
    mat4 const translationMat(const float tx, const float ty, const float tz) {
        return {{1, 0, 0, tx},
                {0, 1, 0, ty},
                {0, 0, 1, tz},
                {0, 0, 0, 1}};
    }

    // returns a scaling matrix using the alpha channel as a homogenous coordinate
    mat4 const scalingMat(const float sx, const float sy, const float sz) {
        return {{sx, 0,  0, 0},
                {0, sy,  0, 0},
                {0,  0, sz, 0},
                {0,  0,  0, 1}};
    }

    // returns the identity matrix
    mat4 const eye() {
        return {{1, 0, 0, 0},
                {0, 1, 0, 0},
                {0, 0, 1, 0},
                {0, 0, 0, 1}};
    }

    // rotation with respect to the x (or red) axis
    mat4 const rotationMatX(float theta) {
        return { {1,           0,                0, 0},
                {0, cosf(theta), -1 * sinf(theta), 0},
                {0, sinf(theta),      cosf(theta), 0},
                {0,           0,                0, 1} };
    }

    // rotation with respect to the y (or green) axis
    mat4 const rotationMatY(float theta) {
        return { {     cosf(theta), 0, sinf(theta), 0},
                {               0, 1,           0, 0},
                {-1 * sinf(theta), 0, cosf(theta), 0},
                {               0, 0,           0, 1} };
    }

    // rotation with respect to the z (or blue) axis
    mat4 const rotationMatZ(float theta) {
        return {{cosf(theta), -1 * sinf(theta), 0, 0},
                {sinf(theta),      cosf(theta), 0, 0},
                {          0,                0, 1, 0},
                {          0,                0, 0, 1} };
    }

    // matrix vector and matrix matrix operations
    vec4 operator*(const mat4& m, const vec4& v) { return { dot(m.r1, v), dot(m.r2, v), dot(m.r3, v), dot(m.r4, v) }; }
    mat4 operator+(const mat4& m1, const mat4& m2) { return { m1.r1 + m2.r1, m1.r2 + m2.r2, m1.r3 + m2.r3, m1.r4 + m2.r4 }; }
    mat4 operator*(const mat4& m1, const mat4& m2) {
        mat4 col_major = { m1 * getColumn(m2, 0), m1 * getColumn(m2, 1), m1 * getColumn(m2, 2), m1 * getColumn(m2, 3) };
        return transpose(col_major);
    }

    void printMat(const mat4& m) {
        printVec(m.r1);
        printVec(m.r2);
        printVec(m.r3);
        printVec(m.r4);
    }
}


// LED CODE
namespace led {
  // PINS
  const int RED_PIN = 6;
  const int GREEN_PIN = 5;
  const int BLUE_PIN = 3;
  const int ALPHA_PIN = 9;

  // SWITCHES
  bool LIGHTS_ON = false;

  // CONSTANTS
  const int MAX_BRIGHTNESS = 255;
  const int FADE_SPEED = 5;

  // pin initalization
  void ledPinSetup() {
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
    pinMode(ALPHA_PIN, OUTPUT);
  }

  void toggleWrite(int pin, uint8_t val) {
    digitalWrite(pin, val & LIGHTS_ON);
  }

  void ledWrite(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    toggleWrite(RED_PIN, r);
    toggleWrite(GREEN_PIN, g);
    toggleWrite(BLUE_PIN, b);
    toggleWrite(ALPHA_PIN, a);
  }
}

namespace interaction {
  int numPeople = 0;
  const int MAX_PEOPLE = 5;
}

namespace farmer {
  // definition changes for readability
  typedef bool fan;
  typedef bool valve;
  typedef bool pump;

  // PINS
  const int V1 = 2; 
  const int V2 = 3;
  const int V3 = 4;
  const int F1 = 5;
  const int F2 = 6;
  const int P1 = 7;
  const int Heat = 8;

  // pin initalization
  void farmerPinSetup() {
    pinMode(V1, OUTPUT);
    pinMode(V2, OUTPUT);
    pinMode(V3, OUTPUT);
    pinMode(F1, OUTPUT);
    pinMode(F2, OUTPUT);
    pinMode(P1, OUTPUT);
  }

  enum outlet { CONTAIN, ATMOSPHERE, TO_ALGEA, ENUM_SIZE = 3 };
  
  struct fStates {
    float temp;
    fan fan1;
    fan fan2;
    valve V1;
    valve V2;
    valve V3;
    pump pump1;
    outlet out;
  };
  
  // set pin voltages based on current states in fStates f
  static void updateOutputPins(fStates* f) {
    digitalWrite(V1, f->V1);      // valve 1
    digitalWrite(V2, f->V2);      // valve 2
    digitalWrite(V3, f->V3);      // valve 3
    digitalWrite(F1, f->fan1);    // fan 1
    digitalWrite(F2, f->fan2);    // fan 2
    digitalWrite(P1, f->pump1);   // fan 3
    digitalWrite(Heat, f->temp);  // heat
  }

  // using a random function as a placeholder for a CO2 sensor check
  int checkCO2() {
    srand((unsigned)time(0));
    int i;
    i = (rand() % 6) + 1;
    return i;
  }
  
  // cooling state function
  bool cooling(fStates *f) {
    std::cout << "COOLING" << std::endl;
    f->V1 = LOW;
    f->V2 = LOW;
    f->V3 = LOW;
    f->fan1 = LOW;
    f->fan2 = LOW;
    f->temp = LOW;
    f->out = CONTAIN;
    f->pump1 = LOW;
    return true; // return checkSorbent();
  };
  
  // heating state function
  bool heating(fStates *f) {
    std::cout << "HEATING" << std::endl;
    f->fan1 = true;
    f->fan2 = true;
    f->temp = true;
    f->out = CONTAIN;
    f->pump1 = false;
    // regulate temperature ()
    return true; // return checkSorbent();
  };
  
  // desorption state function
  bool desorption(fStates *f) {
    std::cout << "DESORPTION" << std::endl;
    f->V1 = HIGH;
    f->V2 = LOW;
    f->V3 = HIGH;
    f->fan1 = HIGH;
    f->fan2 = HIGH;
    f->temp = LOW;
    f->pump1 = HIGH;
    f->out = TO_ALGEA;
    // regulate temperature ()
    return checkCO2(/* multiplier parameter, 1.1 */); // CO2_out <= 1.1 * CO2_in
  };
  
  // adsorption state function
  bool adsorption(fStates *f) {
    std::cout << "ADSORPTION" << std::endl;
    f->V1 = HIGH;
    f->V2 = HIGH;
    f->V3 = LOW;
    f->fan1 = HIGH;
    f->fan2 = LOW;
    f->temp = LOW;
    f->pump1 = LOW;
    f->out = ATMOSPHERE;
    return checkCO2(/* multiplier parameter, 0.9 */) <= 2; // CO2_out >= 0.9 * CO2_in
  };

  // min state, wait for number of interactors
  bool awaitMinPeople(fStates *f) {
    // same states as cooling
    f->V1 = LOW;
    f->V2 = LOW;
    f->V3 = LOW;
    f->fan1 = LOW;
    f->fan2 = LOW;
    f->temp = LOW;
    f->out = CONTAIN;
    f->pump1 = LOW;

    // LED code & interaction checking
    led::ledWrite(0, led::MAX_BRIGHTNESS * (interaction::numPeople / interaction::MAX_PEOPLE), 0, 0);
    bool can_move_on = interaction::numPeople >= interaction::MAX_PEOPLE;
    interaction::numPeople = interaction::numPeople * !can_move_on;
    return can_move_on;
  }
  
  // a 'true' return is an indication
  // that the conditions for switching states was met
  bool (*stateFunc[5])(fStates *f);
  const int MAX_STATES = 5;
}
