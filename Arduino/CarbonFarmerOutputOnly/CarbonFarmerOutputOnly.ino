const int V1 = 2;
const int V2 = 3;
const int V3 = 4;
const int F1 = 5;
const int F2 = 6;
const int P1 = 7;
const int Heat = 8;

String CurrentCase = "No Key Pressed";

void setup() {
  // set the output pins for each electrical device
  pinMode(V1, OUTPUT);
  pinMode(V2, OUTPUT);
  pinMode(V3, OUTPUT);
  pinMode(F1, OUTPUT);
  pinMode(F2, OUTPUT);
  pinMode(P1, OUTPUT);

  Serial.begin(115200);
}

void loop() {
  String input = Serial.readStringUntil('\n'); // read user input from serial monitor

  if (input == "A") { // Case 'A' means high voltage to V1, F1, V2 and low voltage to all else
    digitalWrite(V1, HIGH);
    digitalWrite(V2, HIGH);
    digitalWrite(F1, HIGH);
    digitalWrite(V3, LOW);
    digitalWrite(F2, LOW);
    digitalWrite(P1, LOW);
    digitalWrite(Heat, LOW);
    Serial.println("Adsorbin time mfs");
    CurrentCase = "Adorbin";
  } else if (input == "D") { // Case 'D' means high voltage to V1, F1, V3, F2, P1 and low voltage to all else
    digitalWrite(V1, HIGH);
    digitalWrite(V2, LOW);
    digitalWrite(V3, HIGH);
    digitalWrite(F1, HIGH);
    digitalWrite(F2, HIGH);
    digitalWrite(P1, HIGH);
    digitalWrite(Heat, LOW);
    Serial.println("Desorbin time mfs");
    CurrentCase = "Desorbin";
  } else if (input == "H") { // Case 'H' means high voltage to V1, V2, V3, and low voltage to all else
    digitalWrite(V1, LOW);
    digitalWrite(V2, LOW);
    digitalWrite(V3, LOW);
    digitalWrite(F1, LOW);
    digitalWrite(F2, LOW);
    digitalWrite(P1, LOW);
    digitalWrite(Heat, HIGH);
    //      Input = scd30_1.temperature; // read temperature from sensor
    //      myPID.Compute(); // calculate PID output
    //      if (Output > 127) {
    //        digitalWrite(Heat, HIGH); // turn Heat output fully on
    //      } else {
    //        digitalWrite(Heat, LOW); // turn Heat output fully off
    //      }
    Serial.println("Jimmy Heaters");
    CurrentCase = "Heatin";
  }
  else if (input == "O") { // Case 'O' means low voltage to all 6 electrical devices
    digitalWrite(V1, LOW);
    digitalWrite(V2, LOW);
    digitalWrite(V3, LOW);
    digitalWrite(F1, LOW);
    digitalWrite(F2, LOW);
    digitalWrite(P1, LOW);
    digitalWrite(Heat, LOW);
    Serial.println("Off 2 sleep");
    CurrentCase = "Sleep";
  }
  else {
    Serial.println(CurrentCase);
    delay(3000);
  }
}
